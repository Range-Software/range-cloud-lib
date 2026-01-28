#include <QMap>
#include <QUuid>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslServer>
#include <QLoggingCategory>
#include <QtConcurrentRun>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QDateTime>
#include <memory>

#include "rcl_cloud_action.h"
#include "rcl_http_server.h"
#include <rbl_logger.h>
#include <rbl_error.h>
#include <rbl_utils.h>

class RHttpServerHandler
{
    private:

        //! Handler ID.
        QUuid id;
        //! Service name.
        QString serviceName;
        //! Mutex to protect response data.
        QMutex syncMutex;
        //! Condition variable for response availability.
        QWaitCondition responseCondition;
        //! Response ready flag.
        bool responseReady;
        //! Response message.
        RHttpMessage responseMessage;

    public:

        RHttpServerHandler()
            : id(QUuid::createUuid())
            , serviceName("HttpServerHandler_" + id.toString(QUuid::WithoutBraces))
            , responseReady(false)
        {
            R_LOG_TRACE_IN;
            R_LOG_TRACE_OUT;
        }

        const QUuid &getId() const
        {
            return this->id;
        }

        void sendReply(const RHttpMessage &httpMessage)
        {
            R_LOG_TRACE_IN;
            QMutexLocker locker(&this->syncMutex);
            this->responseMessage = httpMessage;
            this->responseReady = true;
            this->responseCondition.wakeOne();
            R_LOG_TRACE_OUT;
        }

        bool fetchResponseMessage(int timeout, RHttpMessage &responseMessage)
        {
            R_LOG_TRACE_IN;
            QMutexLocker locker(&this->syncMutex);
            RLogger::debug("[%s] Waiting for response (timeout: %d ms)\n",
                          this->serviceName.toUtf8().constData(), timeout);

            bool success = this->responseCondition.wait(&this->syncMutex, timeout);

            if (success && this->responseReady)
            {
                responseMessage = this->responseMessage;
                RLogger::debug("[%s] Response received\n",this->serviceName.toUtf8().constData());
            }
            else
            {
                RLogger::warning("[%s] Response timeout after %d ms\n",
                               this->serviceName.toUtf8().constData(), timeout);
            }
            R_LOG_TRACE_OUT;
            return success && this->responseReady;
        }

};

RHttpServer::RHttpServer(Type type, const RHttpServerSettings &httpServerSettings, QObject *parent)
    : QObject{parent}
    , type{type}
    , httpServerSettings{httpServerSettings}
    , pSslServer{nullptr}
    , pHttpServer{nullptr}
    , pCleanupTimer{nullptr}
    , pAuthTokenValidator{nullptr}
{
    R_LOG_TRACE_IN;

    // Validate settings
    QString validationError;
    if (!this->httpServerSettings.validate(&validationError))
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application, R_ERROR_REF,
                     "Invalid HTTP server settings: %s",
                     validationError.toUtf8().constData());
    }

    this->pSslServer = new QSslServer(this);
    this->pHttpServer = new QHttpServer(this);

#if QT_VERSION > QT_VERSION_CHECK(6, 9, 0)
    QHttpServerConfiguration httpServerConfiguration = this->pHttpServer->configuration();
    httpServerConfiguration.setRateLimitPerSecond(this->httpServerSettings.getRateLimitPerSecond());
    this->pHttpServer->setConfiguration(httpServerConfiguration);
#else
    RLogger::warning("[%s] Rate limiting is not available in Qt %s (requires Qt 6.10+). "
                    "Server will accept unlimited requests per second.\n",
                    this->getServiceName().toUtf8().constData(),
                    qVersion());
#endif

    this->buildApiRoutes();

    this->pSslServer->setSslConfiguration(this->buildSslConfiguration());

    // Setup cleanup timer
    this->pCleanupTimer = new QTimer(this);
    this->pCleanupTimer->setInterval(this->httpServerSettings.getHandlerCleanupIntervalMs());
    QObject::connect(this->pCleanupTimer, &QTimer::timeout, this, &RHttpServer::cleanupStaleHandlers);

    QLoggingCategory::setFilterRules("qt.httpserver=true\n"
                                     "appname.access=true");

    R_LOG_TRACE_OUT;
}

void RHttpServer::setAuthTokenValidator(RAuthTokenValidator *pAuthTokenValidator)
{
    this->pAuthTokenValidator = pAuthTokenValidator;
}

void RHttpServer::start()
{
    R_LOG_TRACE_IN;

    try {
        RLogger::info("[%s] Establishing HTTP server on port: \"%u\"\n",this->getServiceName().toUtf8().constData(),this->httpServerSettings.getPort());

        if (!this->pSslServer->listen(QHostAddress::Any,this->httpServerSettings.getPort()) || !this->pHttpServer->bind(this->pSslServer))
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Application,R_ERROR_REF,"HTTP Server failed to listen on a port \"%u\".",this->pSslServer->serverPort());
        }
        RLogger::info("[%s] HTTP Server is listening on a port \"%u\".\n",this->getServiceName().toUtf8().constData(),this->pSslServer->serverPort());

        // Start cleanup timer
        this->pCleanupTimer->start();
        RLogger::info("[%s] Handler cleanup timer started (interval: %u ms, max age: %u ms)\n",
                     this->getServiceName().toUtf8().constData(),
                     this->httpServerSettings.getHandlerCleanupIntervalMs(),
                     this->httpServerSettings.getMaxStaleHandlerAgeMs());

        emit this->started();
        emit this->ready();
    }
    catch (const std::exception &e)
    {
        RLogger::error("%s\n",e.what());
        emit this->failed();
    }
    catch (const RError &e)
    {
        RLogger::error("%s\n",e.getMessage().toUtf8().constData());
        emit this->failed();
    }

    R_LOG_TRACE_OUT;
}

void RHttpServer::stop()
{
    RLogger::info("[%s] Signal service to stop.\n",this->getServiceName().toUtf8().constData());

    // Stop cleanup timer
    if (this->pCleanupTimer)
    {
        this->pCleanupTimer->stop();
    }

    foreach (QTcpServer *tcpServer, this->pHttpServer->servers())
    {
        tcpServer->close();
    }
    emit this->finished();

    RLogger::info("[%s] Service has been stopped.\n",this->getServiceName().toUtf8().constData());
}

void RHttpServer::sendMessageReply(const RHttpMessage &httpMessage)
{
    R_LOG_TRACE_IN;
    QMutexLocker locker(&this->syncMutex);
    QMap<QUuid,RHttpServerHandler*>::iterator iter = this->serverHandlers.find(httpMessage.getHandlerId());
    if (iter == this->serverHandlers.end())
    {
        RLogger::warning("[%s] HTTP message cannot be sent. Handler (id: \"%s\") does not exist.\n",
                         this->getServiceName().toUtf8().constData(),
                         httpMessage.getHandlerId().toString(QUuid::WithoutBraces).toUtf8().constData());
        R_LOG_TRACE_OUT;
        return;
    }
    iter.value()->sendReply(httpMessage);
    R_LOG_TRACE_OUT;
}

bool RHttpServer::containsServerHandlerId(const QUuid &serverHandlerId)
{
    R_LOG_TRACE_IN;
    QMutexLocker locker(&this->syncMutex);
    bool contains = this->serverHandlers.contains(serverHandlerId);
    R_LOG_TRACE_RETURN(contains);
}

QSslConfiguration RHttpServer::buildSslConfiguration() const
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] CA certificate:     %s\n",this->getServiceName().toUtf8().constData(),this->httpServerSettings.getTlsTrustStore().getCertificateFile().toUtf8().constData());
    RLogger::info("[%s] Client certificate: %s\n",this->getServiceName().toUtf8().constData(),this->httpServerSettings.getTlsKeyStore().getCertificateFile().toUtf8().constData());
    RLogger::info("[%s] Key:                %s\n",this->getServiceName().toUtf8().constData(),this->httpServerSettings.getTlsKeyStore().getKeyFile().toUtf8().constData());

    const auto caCertificates = QSslCertificate::fromPath(this->httpServerSettings.getTlsTrustStore().getCertificateFile(),QSsl::EncodingFormat::Pem);
    if (caCertificates.empty())
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application,R_ERROR_REF,
                     "Couldn't retrieve SSL CA certificate from file \"%s\".",
                     this->httpServerSettings.getTlsTrustStore().getCertificateFile().toUtf8().constData());
    }
    this->checkCertificateExpiry(caCertificates, this->httpServerSettings.getTlsTrustStore().getCertificateFile());

    const auto clientCertificates = QSslCertificate::fromPath(this->httpServerSettings.getTlsKeyStore().getCertificateFile(),QSsl::EncodingFormat::Pem);
    if (clientCertificates.empty())
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application,R_ERROR_REF,
                     "Couldn't retrieve SSL client certificate from file \"%s\".",
                     this->httpServerSettings.getTlsKeyStore().getCertificateFile().toUtf8().constData());
    }
    this->checkCertificateExpiry(clientCertificates, this->httpServerSettings.getTlsKeyStore().getCertificateFile());

    QSslKey privateKey(this->loadPrivateKey());
    if (privateKey.isNull())
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application,R_ERROR_REF,
                     "Couldn't retrieve SSL key from file \"%s\".",
                     this->httpServerSettings.getTlsKeyStore().getKeyFile().toUtf8().constData());
    }

    QSslConfiguration sslConfig;
    sslConfig.setCaCertificates(caCertificates);
    sslConfig.setLocalCertificateChain(clientCertificates);
    sslConfig.setPrivateKey(privateKey);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    switch (this->type)
    {
        case RHttpServer::Public:
        {
            sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
            break;
        }
        case RHttpServer::Private:
        {
            sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
            break;
        }
        default:
        {
            break;
        }
    }

    R_LOG_TRACE_RETURN(sslConfig);
}

QSslKey RHttpServer::loadPrivateKey() const
{
    const QString &keyFile = this->httpServerSettings.getTlsKeyStore().getKeyFile();
    QSsl::EncodingFormat encoding = QSsl::Pem;
    const QByteArray &passPhrase = this->httpServerSettings.getTlsKeyStore().getPassword().toUtf8();

    // Read key file once
    QFile privateKeyFile(keyFile);
    if (!privateKeyFile.open(QIODevice::ReadOnly))
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application,R_ERROR_REF,
                     "Couldn't open SSL key file \"%s\" for reading.",
                     keyFile.toUtf8().constData());
    }

    QByteArray keyData = privateKeyFile.readAll();
    privateKeyFile.close();

    if (keyData.isEmpty())
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Application,R_ERROR_REF,
                     "SSL key file \"%s\" is empty.",
                     keyFile.toUtf8().constData());
    }

    // Try different algorithms with the same data
    QList<QSsl::KeyAlgorithm> algorithms;
    algorithms.append(QSsl::Rsa);
    algorithms.append(QSsl::Dsa);
    algorithms.append(QSsl::Ec);
    algorithms.append(QSsl::Dh);
    algorithms.append(QSsl::Opaque);

    for (QSsl::KeyAlgorithm algorithm : algorithms)
    {
        RLogger::debug("[%s] Trying SSL algorithm: %s\n",
                       this->getServiceName().toUtf8().constData(),
                       RHttpServer::getKeyAlgorithmName(algorithm).toUtf8().constData());

        QSslKey sslKey(keyData, algorithm, encoding, QSsl::PrivateKey, passPhrase);

        if (!sslKey.isNull())
        {
            RLogger::info("[%s] Private key loaded successfully using algorithm: %s\n",
                          this->getServiceName().toUtf8().constData(),
                          RHttpServer::getKeyAlgorithmName(algorithm).toUtf8().constData());
            return sslKey;
        }
    }

    RLogger::error("[%s] Failed to load private key from \"%s\" with any supported algorithm\n",
                   this->getServiceName().toUtf8().constData(),
                   keyFile.toUtf8().constData());
    return QSslKey();
}

void RHttpServer::checkCertificateExpiry(const QList<QSslCertificate> &certificates, const QString &certificateFile) const
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    for (const QSslCertificate &certificate : certificates)
    {
        if (certificate.isNull())
        {
            continue;
        }

        QString subjectName = certificate.subjectDisplayName();
        QDateTime expiryDate = certificate.expiryDate();
        QDateTime effectiveDate = certificate.effectiveDate();

        RLogger::info("[%s] Certificate \"%s\" valid from %s to %s\n",
                      this->getServiceName().toUtf8().constData(),
                      subjectName.toUtf8().constData(),
                      effectiveDate.toString(Qt::ISODate).toUtf8().constData(),
                      expiryDate.toString(Qt::ISODate).toUtf8().constData());

        if (currentDateTime < effectiveDate)
        {
            throw RError(RError::Application, R_ERROR_REF,
                         "[%s] Certificate \"%s\" from file \"%s\" is not yet valid (effective date: %s).",
                         this->getServiceName().toUtf8().constData(),
                         subjectName.toUtf8().constData(),
                         certificateFile.toUtf8().constData(),
                         effectiveDate.toString(Qt::ISODate).toUtf8().constData());
        }

        if (currentDateTime > expiryDate)
        {
            throw RError(RError::Application, R_ERROR_REF,
                         "[%s] Certificate \"%s\" from file \"%s\" has expired (expiry date: %s).",
                         this->getServiceName().toUtf8().constData(),
                         subjectName.toUtf8().constData(),
                         certificateFile.toUtf8().constData(),
                         expiryDate.toString(Qt::ISODate).toUtf8().constData());
        }

        qint64 daysUntilExpiry = currentDateTime.daysTo(expiryDate);
        if (daysUntilExpiry <= 30)
        {
            RLogger::warning("[%s] Certificate \"%s\" from file \"%s\" will expire in %lld days (%s).\n",
                            this->getServiceName().toUtf8().constData(),
                            subjectName.toUtf8().constData(),
                            certificateFile.toUtf8().constData(),
                            daysUntilExpiry,
                            expiryDate.toString(Qt::ISODate).toUtf8().constData());
        }
    }
}

QString RHttpServer::getKeyAlgorithmName(QSsl::KeyAlgorithm algorithm)
{
    switch (algorithm)
    {
        case QSsl::Opaque: return "Opaque";
        case QSsl::Rsa:    return "RSA";
        case QSsl::Dsa:    return "DSA";
        case QSsl::Ec:     return "Elliptic Curve";
        case QSsl::Dh:     return "Diffie-Hellman";
        default:           return QString("Unknown (%1)").arg(QString::number(algorithm));
    }
}

void RHttpServer::buildApiRoutes()
{
    this->pHttpServer->route("/", []() { return RVendor::name(); });

    QMap<QString,QString> actionMap = RCloudAction::getActionMap();
    for (auto iter = actionMap.cbegin(); iter != actionMap.cend(); ++iter)
    {
        this->buildApiRoute(iter.key());
    }

    this->pHttpServer->setMissingHandler(this->pHttpServer,[this](const QHttpServerRequest &request, QHttpServerResponder &responder)
    {
        QString commonName = RTlsTrustStore::findCN(request.sslConfiguration().peerCertificate());
        RLogger::info("[%s] Request: user = \"%s\" url = \"%s\"\n",
                      this->getServiceName().toUtf8().constData(),
                      commonName.toUtf8().constData(),
                      request.url().toString().toUtf8().constData());
        responder.sendResponse(QHttpServerResponse("Not found"));
    });

    this->pHttpServer->addAfterRequestHandler(this->pHttpServer, [](const QHttpServerRequest &, QHttpServerResponse &resp)
    {
        QHttpHeaders h = resp.headers();
        h.append(QHttpHeaders::WellKnownHeader::Server, "Range Cloud HTTPs Server");
        resp.setHeaders(std::move(h));
    });
}

void RHttpServer::buildApiRoute(const QString &actionKey)
{
    this->pHttpServer->route(QString("/%1/").arg(actionKey),RHttpMessage::findMethodForAction(actionKey),[=](const QHttpServerRequest &request)
    {
        QString resourceName(request.query().queryItemValue(RCloudAction::Resource::Name::key));
        QUuid id(request.query().queryItemValue(RCloudAction::Resource::Id::key));

        QString userName;
        if (this->type == RHttpServer::Public)
        {
            QString authUser = request.query().queryItemValue(RCloudAction::Auth::User::key);
            QString authToken = request.query().queryItemValue(RCloudAction::Auth::Token::key);

            if (this->authenticateToken(authUser,authToken))
            {
                userName = authUser;
            }
            else
            {
                RLogger::warning("[%s] Failed to authenticate: user = \"%s\" token = \"%s\"\n",
                                 this->getServiceName().toUtf8().constData(),
                                 authUser.toUtf8().constData(),
                                 authToken.toUtf8().constData());
            }
        }
        else if (this->type == RHttpServer::Private)
        {
            userName = RTlsTrustStore::findCN(request.sslConfiguration().peerCertificate());
        }

        QString fromAddress = QString("%1:%2").arg(request.remoteAddress().toString(),QString::number(request.remotePort()));

        RLogger::info("[%s] Request: user = \"%s\" (%s), url = \"%s\"\n",
                      this->getServiceName().toUtf8().constData(),
                      userName.toUtf8().constData(),
                      fromAddress.toUtf8().constData(),
                      request.url().toDisplayString().toUtf8().constData());

        return QtConcurrent::run([=](const QByteArray body) {
            return this->processRequest(actionKey,userName,fromAddress,resourceName,id,body);
        },request.body());
    });
}

bool RHttpServer::authenticateToken(const QString &user, const QString &token) const
{
    if (this->pAuthTokenValidator)
    {
        return this->pAuthTokenValidator->validate(user,token);
    }
    return false;
}

QHttpServerResponse RHttpServer::processRequest(
    const QString &action,
    const QString &owner,
    const QString &fromAddress,
    const QString &resourceName,
    const QUuid &id,
    const QByteArray &data)
{
    RHttpMessage message;
    message.setOwner(owner);

    QMap<QString, QString> properties;

    properties.insert(RCloudAction::Action::key,action);
    properties.insert(RCloudAction::Resource::Name::key,resourceName);
    properties.insert(RCloudAction::Resource::Id::key,id.toString(QUuid::WithBraces));

    message.setProperties(properties);
    message.setBody(data);
    message.setFrom(fromAddress);

    std::unique_ptr<RHttpServerHandler> serverHandler(new RHttpServerHandler);
    this->serverHandlerAdd(serverHandler.get());
    message.setHandlerId(serverHandler->getId());
    RLogger::debug("[%s] Received message with payload size = \"%ld\"\n",this->getServiceName().toUtf8().constData(),message.getBody().size());

    emit this->requestAvailable(message);

    RHttpMessage responseMessage;
    const quint32 timeout = this->httpServerSettings.getResponseTimeoutMs();
    bool success = serverHandler->fetchResponseMessage(timeout, responseMessage);

    if (!success)
    {
        RLogger::error("[%s] Request timeout after %u ms for action '%s' from %s\n",
                      this->getServiceName().toUtf8().constData(),
                      timeout,
                      action.toUtf8().constData(),
                      fromAddress.toUtf8().constData());

        responseMessage.setErrorType(RError::Timeout);
        responseMessage.setBody(QString("Request timeout after %1 ms").arg(timeout).toUtf8());
    }

    this->serverHandlerRemove(serverHandler->getId());

    RLogger::debug("[%s] Create server response\n",this->getServiceName().toUtf8().constData());
    QHttpServerResponse response(responseMessage.getBody(),RHttpMessage::errorTypeToStatusCode(responseMessage.getErrorType()));
    response.setHeaders(responseMessage.getResponseHeaders());
    RLogger::debug("[%s] Response is ready\n",this->getServiceName().toUtf8().constData());
    return response;
}

QString RHttpServer::getServiceName() const
{
    switch (this->type)
    {
        case RHttpServer::Public:
        {
            return QString("PublicHttpService");
        }
        case RHttpServer::Private:
        {
            return QString("PrivateHttpService");
        }
        default:
        {
            return QString("DefaultHttpService");
        }
    }
}

void RHttpServer::serverHandlerAdd(RHttpServerHandler *serverHandler)
{
    R_LOG_TRACE_IN;
    QMutexLocker locker(&this->syncMutex);
    this->serverHandlers.insert(serverHandler->getId(),serverHandler);
    this->handlerCreationTimes.insert(serverHandler->getId(), QDateTime::currentDateTime());
    RLogger::debug("[%s] Added handler %s (total handlers: %d)\n",
                  this->getServiceName().toUtf8().constData(),
                  serverHandler->getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                  this->serverHandlers.size());
    R_LOG_TRACE_OUT;
}

void RHttpServer::serverHandlerRemove(const QUuid &serverHandlerId)
{
    R_LOG_TRACE_IN;
    QMutexLocker locker(&this->syncMutex);
    this->serverHandlers.remove(serverHandlerId);
    this->handlerCreationTimes.remove(serverHandlerId);
    RLogger::debug("[%s] Removed handler %s (remaining handlers: %d)\n",
                  this->getServiceName().toUtf8().constData(),
                  serverHandlerId.toString(QUuid::WithoutBraces).toUtf8().constData(),
                  this->serverHandlers.size());
    R_LOG_TRACE_OUT;
}

void RHttpServer::cleanupStaleHandlers()
{
    R_LOG_TRACE_IN;
    QMutexLocker locker(&this->syncMutex);

    QDateTime currentTime = QDateTime::currentDateTime();
    qint64 maxAgeMs = this->httpServerSettings.getMaxStaleHandlerAgeMs();

    QList<QUuid> staleHandlerIds;

    // Find stale handlers
    for (auto iter = this->handlerCreationTimes.constBegin(); iter != this->handlerCreationTimes.constEnd(); ++iter)
    {
        qint64 ageMs = iter.value().msecsTo(currentTime);
        if (ageMs > maxAgeMs)
        {
            staleHandlerIds.append(iter.key());
            RLogger::warning("[%s] Found stale handler %s (age: %lld ms, max: %lld ms)\n",
                           this->getServiceName().toUtf8().constData(),
                           iter.key().toString(QUuid::WithoutBraces).toUtf8().constData(),
                           ageMs,
                           maxAgeMs);
        }
    }

    // Remove stale handlers
    for (const QUuid &handlerId : staleHandlerIds)
    {
        RHttpServerHandler *handler = this->serverHandlers.value(handlerId, nullptr);
        if (handler)
        {
            // Send timeout response to wake up waiting thread
            RHttpMessage timeoutResponse;
            timeoutResponse.setErrorType(RError::Timeout);
            timeoutResponse.setBody("Handler cleanup: request exceeded maximum age");
            handler->sendReply(timeoutResponse);

            delete handler;
        }
        this->serverHandlers.remove(handlerId);
        this->handlerCreationTimes.remove(handlerId);
    }

    if (!staleHandlerIds.isEmpty())
    {
        RLogger::warning("[%s] Cleaned up %d stale handler(s). Remaining handlers: %d\n",
                       this->getServiceName().toUtf8().constData(),
                       staleHandlerIds.size(),
                       this->serverHandlers.size());
    }

    // Log warning if handler count is high
    if (this->serverHandlers.size() > 100)
    {
        RLogger::warning("[%s] High number of active handlers: %d. This may indicate a resource leak.\n",
                       this->getServiceName().toUtf8().constData(),
                       this->serverHandlers.size());
    }

    R_LOG_TRACE_OUT;
}

void RHttpServer::onStartedEncryptionHandshake(QSslSocket *socket)
{
    if (socket->waitForEncrypted())
    {
        QString commonName = RTlsTrustStore::findCN(socket->peerCertificate());
        RLogger::info("[%s] Incoming connection from \"%s\"\n",this->getServiceName().toUtf8().constData(),commonName.toUtf8().constData());
    }
}
