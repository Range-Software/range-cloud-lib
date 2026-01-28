#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QNetworkProxy>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_utils.h>

#include "rcl_http_client.h"

RHttpClient::RHttpClient(Type type, const RHttpClientSettings &httpClientSettings, QObject *parent)
    : QObject{parent}
    , type{type}
    , httpClientSettings{httpClientSettings}
    , networkErrorCode{QNetworkReply::NoError}
    , httpErrorCode{QHttpServerResponder::StatusCode::Ok}
{
    R_LOG_TRACE_IN;
    this->networkManager = new QNetworkAccessManager(this);

    this->setHttpClientSettings(httpClientSettings);

    QObject::connect(this->networkManager,&QNetworkAccessManager::authenticationRequired,this,&RHttpClient::onAuthenticationRequired);
    QObject::connect(this->networkManager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&RHttpClient::onProxyAuthenticationRequired);
    QObject::connect(this,&RHttpClient::requestAvailable,this,&RHttpClient::onRequestAvailable);

    R_LOG_TRACE_OUT;
}

void RHttpClient::sendRequest(const RHttpMessage &httpMessageRequest, RHttpMessage &httpMessageReply)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_MESSAGE("Request mutex -> lock.\n");
    this->requestMutex.lock();
    this->applcationErrorCode = RError::None;
    this->networkErrorCode = QNetworkReply::NoError;
    this->httpErrorCode = QHttpServerResponder::StatusCode::Ok;
    this->replyMessage = httpMessageRequest;
    this->responseBytes.clear();

    this->networkManager->clearConnectionCache();

    emit this->requestAvailable(httpMessageRequest);

    RLogger::trace("Current thread: \'%p\', object thread: \'%p\'\n", QThread::currentThread(), this->thread());

    R_LOG_TRACE_MESSAGE("Request mutex -> tryLock.\n");
    while (!this->requestMutex.tryLock())
    {
        R_LOG_TRACE_MESSAGE("Process events.");
        QCoreApplication::processEvents();
        QThread::msleep(1000);
    }

    httpMessageReply = this->replyMessage;

    if (this->applcationErrorCode != RError::None)
    {
        httpMessageReply.setErrorType(this->applcationErrorCode);
        httpMessageReply.setBody(this->applicationErrorString.toUtf8());
    }

    if (this->networkErrorCode != QNetworkReply::NoError)
    {
        if (httpMessageReply.getErrorType() == RError::None)
        {
            httpMessageReply.setErrorType(RError::Connection);
        }
        if (httpMessageReply.getBody().isEmpty())
        {
            if (int(this->httpErrorCode) == 0)
            {
                httpMessageReply.setBody(this->networkErrorString.toUtf8());
            }
            else
            {
                httpMessageReply.setBody(QString("%1: %2 (%3)").arg(QString::number(int(this->httpErrorCode)),this->httpErrorString,this->networkErrorString).toUtf8());
            }
        }
    }

    R_LOG_TRACE_MESSAGE("Request mutex -> unlock.\n");
    this->requestMutex.unlock();

    R_LOG_TRACE_OUT;
}

QSslConfiguration RHttpClient::buildSslConfiguration() const
{
    R_LOG_TRACE_IN;

    QSslConfiguration sslConfig;

    if (!this->httpClientSettings.getTlsTrustStore().getCertificateFile().isEmpty())
    {
        RLogger::debug("CA certificate:     %s\n",this->httpClientSettings.getTlsTrustStore().getCertificateFile().toUtf8().constData());

        const auto caCertificates = QSslCertificate::fromPath(this->httpClientSettings.getTlsTrustStore().getCertificateFile(),QSsl::EncodingFormat::Pem);
        if (caCertificates.empty())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Application,R_ERROR_REF,
                         "Couldn't retrieve SSL CA certificate from file \"%s\".",
                         this->httpClientSettings.getTlsTrustStore().getCertificateFile().toUtf8().constData());
        }
        this->checkCertificateExpiry(caCertificates, this->httpClientSettings.getTlsTrustStore().getCertificateFile());

        sslConfig.setCaCertificates(caCertificates);
    }

    if (this->type == RHttpClient::Private)
    {
        RLogger::debug("Key:                %s\n",this->httpClientSettings.getTlsKeyStore().getKeyFile().toUtf8().constData());
        RLogger::debug("Client certificate: %s\n",this->httpClientSettings.getTlsKeyStore().getCertificateFile().toUtf8().constData());

        if (this->httpClientSettings.getTlsKeyStore().getKeyFile().isEmpty())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::InvalidInput,R_ERROR_REF,"SSL client key file not provided.");
        }

        if (this->httpClientSettings.getTlsKeyStore().getCertificateFile().isEmpty())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::InvalidInput,R_ERROR_REF,"SSL client certificate file not provided.");
        }

        const auto clientCertificates = QSslCertificate::fromPath(this->httpClientSettings.getTlsKeyStore().getCertificateFile(),QSsl::EncodingFormat::Pem);
        if (clientCertificates.empty())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Application,R_ERROR_REF,
                         "Couldn't retrieve SSL client certificate from file \"%s\".",
                         this->httpClientSettings.getTlsKeyStore().getCertificateFile().toUtf8().constData());
        }
        this->checkCertificateExpiry(clientCertificates, this->httpClientSettings.getTlsKeyStore().getCertificateFile());

        QSslKey privateKey(this->loadPrivateKey());
        if (privateKey.isNull())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Application,R_ERROR_REF,
                         "Couldn't retrieve SSL key from file \"%s\".",
                         this->httpClientSettings.getTlsKeyStore().getKeyFile().toUtf8().constData());
        }

        sslConfig.setLocalCertificateChain(clientCertificates);
        sslConfig.setPrivateKey(privateKey);
    }

    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    // sslConfig.setPeerVerifyMode(QSslSocket::AutoVerifyPeer);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

    R_LOG_TRACE_RETURN(sslConfig);
}

QSslKey RHttpClient::loadPrivateKey() const
{
    const QString &keyFile = this->httpClientSettings.getTlsKeyStore().getKeyFile();
    QSsl::EncodingFormat encoding = QSsl::Pem;
    const QByteArray &passPhrase = this->httpClientSettings.getTlsKeyStore().getPassword().toUtf8();

    QList<QSsl::KeyAlgorithm> algorithms;
    algorithms.append(QSsl::Rsa);
    algorithms.append(QSsl::Dsa);
    algorithms.append(QSsl::Ec);
    algorithms.append(QSsl::Dh);
    algorithms.append(QSsl::Opaque);

    for (QSsl::KeyAlgorithm algorithm : algorithms)
    {
        QFile privateKeyFile(keyFile);
        if (!privateKeyFile.open(QIODevice::ReadOnly))
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Application,R_ERROR_REF,
                         "Couldn't open SSL key file \"%s\" for reading.",
                         keyFile.toUtf8().constData());
        }

        RLogger::debug("Trying SSL algorithm: %s\n",
                       RHttpClient::getKeyAlgorithmName(algorithm).toUtf8().constData());
        QSslKey sslKey(&privateKeyFile,algorithm,encoding,QSsl::PrivateKey,passPhrase);
        privateKeyFile.close();
        if (!sslKey.isNull())
        {
            RLogger::debug("Private key SSL algorithm: %s\n",
                           RHttpClient::getKeyAlgorithmName(algorithm).toUtf8().constData());
            return sslKey;
        }
    }
    return QSslKey();
}

void RHttpClient::checkCertificateExpiry(const QList<QSslCertificate> &certificates, const QString &certificateFile) const
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

        RLogger::info("Certificate \"%s\" valid from %s to %s\n",
                      subjectName.toUtf8().constData(),
                      effectiveDate.toString(Qt::ISODate).toUtf8().constData(),
                      expiryDate.toString(Qt::ISODate).toUtf8().constData());

        if (currentDateTime < effectiveDate)
        {
            throw RError(RError::Application, R_ERROR_REF,
                         "Certificate \"%s\" from file \"%s\" is not yet valid (effective date: %s).",
                         subjectName.toUtf8().constData(),
                         certificateFile.toUtf8().constData(),
                         effectiveDate.toString(Qt::ISODate).toUtf8().constData());
        }

        if (currentDateTime > expiryDate)
        {
            throw RError(RError::Application, R_ERROR_REF,
                         "Certificate \"%s\" from file \"%s\" has expired (expiry date: %s).",
                         subjectName.toUtf8().constData(),
                         certificateFile.toUtf8().constData(),
                         expiryDate.toString(Qt::ISODate).toUtf8().constData());
        }

        qint64 daysUntilExpiry = currentDateTime.daysTo(expiryDate);
        if (daysUntilExpiry <= 30)
        {
            RLogger::warning("Certificate \"%s\" from file \"%s\" will expire in %lld days (%s).\n",
                            subjectName.toUtf8().constData(),
                            certificateFile.toUtf8().constData(),
                            daysUntilExpiry,
                            expiryDate.toString(Qt::ISODate).toUtf8().constData());
        }
    }
}

QString RHttpClient::getKeyAlgorithmName(QSsl::KeyAlgorithm algorithm)
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

void RHttpClient::setHttpClientSettings(const RHttpClientSettings &httpClientSettings)
{
    R_LOG_TRACE_IN;
    this->httpClientSettings = httpClientSettings;
    if (this->httpClientSettings.getProxySettings().getType() == RHttpProxySettings::SystemProxy)
    {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    }
    else if (this->httpClientSettings.getProxySettings().getType() == RHttpProxySettings::ManualProxy)
    {
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        QNetworkProxy networkProxy(QNetworkProxy::HttpProxy,
                                   httpClientSettings.getProxySettings().getHost(),
                                   httpClientSettings.getProxySettings().getPort(),
                                   httpClientSettings.getProxySettings().getUser(),
                                   httpClientSettings.getProxySettings().getPassword());
        this->networkManager->setProxy(networkProxy);
    }
    this->networkManager->setTransferTimeout(this->httpClientSettings.getTimeout());
    RLogger::debug("Transfer timeout: %d [ms]\n",this->networkManager->transferTimeout());
    R_LOG_TRACE_OUT;
}

void RHttpClient::abort()
{
    R_LOG_TRACE_IN;
    if (this->networkReply)
    {
        R_LOG_TRACE_MESSAGE("Aborting ...\n");
        QTimer::singleShot(0, this->networkReply, SIGNAL(abort()));
    }
    R_LOG_TRACE_OUT;
}

void RHttpClient::onAuthenticationRequired(QNetworkReply *, QAuthenticator *)
{
    R_LOG_TRACE_IN;
    // authenticator->setUser(this->httpClientSettings.getUser());
    // authenticator->setPassword(this->httpClientSettings.getPassword());
    R_LOG_TRACE_OUT;
}

void RHttpClient::onProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *authenticator)
{
    R_LOG_TRACE_IN;
    authenticator->setUser(this->httpClientSettings.getProxySettings().getUser());
    authenticator->setPassword(this->httpClientSettings.getProxySettings().getPassword());
    R_LOG_TRACE_OUT;
}

void RHttpClient::onReadyRead()
{
    R_LOG_TRACE_IN;
    this->responseBytes.append(this->networkReply->readAll());
    R_LOG_TRACE_OUT;
}

void RHttpClient::onErrorOccurred(QNetworkReply::NetworkError code)
{
    R_LOG_TRACE_IN;

    if (code != this->networkReply->error())
    {
        RLogger::warning("Unexpected behavior, error codes do not match (%d : %d).\n",code,this->networkReply->error());
    }

    this->networkErrorCode = this->networkReply->error();
    this->networkErrorString = this->networkReply->errorString();
    this->httpErrorCode = QHttpServerResponder::StatusCode(this->networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt());
    this->httpErrorString = this->networkReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();

    if (this->networkErrorCode == QNetworkReply::OperationCanceledError)
    {
        this->responseBytes.clear();
    }

    if (int(this->httpErrorCode) == 0)
    {
        RLogger::error("HTTP Request has failed. %s\n",this->networkErrorString.toUtf8().constData());
    }
    else
    {
        RLogger::error("HTTP Request has failed. %d: %s (%s)\n",
                       this->httpErrorCode,this->httpErrorString.toUtf8().constData(),this->networkErrorString.toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

void RHttpClient::onFinished()
{
    R_LOG_TRACE_IN;
    RLogger::debug("Client request finished\n");

    this->networkErrorCode = this->networkReply->error();
    this->networkErrorString = this->networkReply->errorString();

    this->httpErrorCode = QHttpServerResponder::StatusCode(this->networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt());
    this->httpErrorString = this->networkReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();

    this->replyMessage.setBody(this->responseBytes);
    this->replyMessage.setErrorType(RHttpMessage::statusCodeToErrorType(this->httpErrorCode));

    this->networkReply->deleteLater();
    this->networkReply = nullptr;

    R_LOG_TRACE_MESSAGE("Request mutex -> unlock.\n");
    this->requestMutex.unlock();
    R_LOG_TRACE_OUT;
}

void RHttpClient::onSslErrors(const QList<QSslError> &errors)
{
    R_LOG_TRACE_IN;
    foreach (QSslError sslError, errors)
    {
        RLogger::error("SSL error: %s (%s).\n",
                       sslError.errorString().toUtf8().constData(),
                       sslError.certificate().subjectDisplayName().toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

void RHttpClient::onRequestAvailable(const RHttpMessage &httpMessageRequest)
{
    R_LOG_TRACE_IN;

    QUrl url(QString("%1/%2").arg(this->httpClientSettings.getUrl(),httpMessageRequest.getTo()));
    url.setQuery(httpMessageRequest.getUrlQuery());

    RLogger::info("Request url = \"%s\"\n",url.toDisplayString().toUtf8().constData());

    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, RVendor::name() + "/" + RVendor::version().toString());
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    try
    {
        networkRequest.setSslConfiguration(this->buildSslConfiguration());
    }
    catch (const RError &e)
    {
        this->applcationErrorCode = e.getType();
        this->applicationErrorString = QString("Failed to prepare SSL configuration. %1").arg(e.getMessage());

        RLogger::error("HttpClient: %s\n", this->applicationErrorString.toUtf8().constData());

        R_LOG_TRACE_MESSAGE("Request mutex -> unlock.\n");
        this->requestMutex.unlock();
        R_LOG_TRACE_OUT;
        return;
    }

    if (httpMessageRequest.getMethod() == QHttpServerRequest::Method::Get)
    {
        R_LOG_TRACE_MESSAGE("HTTP GET");
        this->networkReply = this->networkManager->get(networkRequest);
    }
    else if (httpMessageRequest.getMethod() == QHttpServerRequest::Method::Put)
    {
        R_LOG_TRACE_MESSAGE("HTTP PUT");
        this->networkReply = this->networkManager->put(networkRequest,httpMessageRequest.getBody());
    }
    else if (httpMessageRequest.getMethod() == QHttpServerRequest::Method::Post)
    {
        R_LOG_TRACE_MESSAGE("HTTP POST");
        this->networkReply = this->networkManager->post(networkRequest,httpMessageRequest.getBody());
    }
    else
    {
        RLogger::error("HttpClient: Unsupported HTTP method \"%s\".\n", RHttpMessage::httpMethodToString(httpMessageRequest.getMethod()).toUtf8().constData());
    }

    QObject::connect(this->networkReply, &QIODevice::readyRead, this, &RHttpClient::onReadyRead);
    QObject::connect(this->networkReply, &QNetworkReply::errorOccurred, this, &RHttpClient::onErrorOccurred);
    QObject::connect(this->networkReply, &QNetworkReply::finished, this, &RHttpClient::onFinished);
    QObject::connect(this->networkReply, &QNetworkReply::sslErrors, this, &RHttpClient::onSslErrors);
    QObject::connect(this->networkReply, &QNetworkReply::uploadProgress, this, &RHttpClient::onUploadProgress);
    QObject::connect(this->networkReply, &QNetworkReply::downloadProgress, this, &RHttpClient::onDownloadProgress);

    R_LOG_TRACE_OUT;
}

void RHttpClient::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    R_LOG_TRACE_IN;
    emit this->uploadProgress(bytesSent,bytesTotal);
    R_LOG_TRACE_OUT;
}

void RHttpClient::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    R_LOG_TRACE_IN;
    emit this->downloadProgress(bytesReceived,bytesTotal);
    R_LOG_TRACE_OUT;
}

QString RHttpClient::buildUrl(const QString &address, const uint port, const QString &topic)
{
    if (topic.isEmpty())
    {
        return "https://" + address + ":" + QString::number(port);
    }
    else
    {
        return "https://" + address + ":" + QString::number(port) + "/" + topic;
    }
}
