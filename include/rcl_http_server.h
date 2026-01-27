#ifndef RCL_HTTP_SERVER_H
#define RCL_HTTP_SERVER_H

#include <QObject>
#include <QHttpServerResponse>
#include <QString>
#include <QMap>
#include <QMutex>
#include <QUuid>
#include <QSslSocket>
#include <QSslServer>
#include <QTimer>
#include <QDateTime>
#include <memory>

#include "rcl_auth_token_validator.h"
#include "rcl_http_message.h"
#include "rcl_http_server_settings.h"

class QHttpServer;
class RHttpServerHandler;

class RHttpServer : public QObject
{

    Q_OBJECT

    public:

        enum Type
        {
            Public  = 0,
            Private = 1
        };

    private:

        //! Mutex.
        QMutex syncMutex;

        //! Server type.
        Type type;
        //! Server settings.
        RHttpServerSettings httpServerSettings;
        //! Pointer to SSL server.
        QSslServer *pSslServer;
        //! Pointer to HTTP server.
        QHttpServer *pHttpServer;
        //! Map of server handlers.
        QMap<QUuid,RHttpServerHandler*> serverHandlers;
        //! Map of handler creation times.
        QMap<QUuid,QDateTime> handlerCreationTimes;
        //! Timer for handler cleanup.
        QTimer *pCleanupTimer;
        //! Authentication token validator.
        RAuthTokenValidator *pAuthTokenValidator;

    public:

        //! Constructor.
        explicit RHttpServer(RHttpServer::Type type, const RHttpServerSettings &httpServerSettings, QObject *parent = nullptr);

        //! Set authentication token validator.
        void setAuthTokenValidator(RAuthTokenValidator *pAuthTokenValidator);

        //! Start service.
        void start();

        //! Stop service.
        void stop();

        //! Send message.
        void sendMessageReply(const RHttpMessage &httpMessage);

        //! Check if server contains given handler ID.
        bool containsServerHandlerId(const QUuid &serverHandlerId);

    private:

        //! Build SSL configuration.
        QSslConfiguration buildSslConfiguration() const;

        //! Load private key and try all available algorithms.
        QSslKey loadPrivateKey() const;

        //! Returns name of given algorithm.
        static QString getKeyAlgorithmName(QSsl::KeyAlgorithm algorithm);

        //! Build API routes.
        void buildApiRoutes();

        void buildApiRoute(const QString &actionKey);

        //! Authenticate user and token.
        bool authenticateToken(const QString &user, const QString &token) const;

        //! Process request.
        QHttpServerResponse processRequest(const QString &action,
                                           const QString &owner,
                                           const QString &fromAddress,
                                           const QString &resourceName,
                                           const QUuid &id,
                                           const QByteArray &data);

        //! Return service name.
        QString getServiceName() const;

    private slots:

        void serverHandlerAdd(RHttpServerHandler *serverHandler);

        void serverHandlerRemove(const QUuid &serverHandlerId);

        void cleanupStaleHandlers();

        void onStartedEncryptionHandshake(QSslSocket *socket);

    signals:

        //! Service is ready.
        void ready();

        //! Service has started.
        void started();

        //! Service has finished.
        void finished();

        //! Service has failed.
        void failed();

        void requestAvailable(const RHttpMessage &httpMessage);

};

#endif // RCL_HTTP_SERVER_H
