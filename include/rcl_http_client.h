#ifndef RCL_HTTP_CLIENT_H
#define RCL_HTTP_CLIENT_H

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMutex>
#include <QObject>

#include "rcl_http_client_settings.h"
#include "rcl_http_message.h"

class RHttpClient : public QObject
{

    Q_OBJECT

    public:

        enum Type
        {
            Public  = 0,
            Private = 1 << 0,
        };

    private:

        //! Client type.
        Type type;
        //! Client settings.
        RHttpClientSettings httpClientSettings;
        //! Network manager.
        QNetworkAccessManager *networkManager;
        //! Network reply.
        QNetworkReply *networkReply;

        //! Application error.
        RError::Type applcationErrorCode;
        QString applicationErrorString;

        //! Network error.
        QNetworkReply::NetworkError networkErrorCode;
        QString networkErrorString;

        //! HTTP error.
        QHttpServerResponder::StatusCode httpErrorCode;
        QString httpErrorString;

        RHttpMessage replyMessage;

        QByteArray responseBytes;

        //! Mutex locking pending request.
        QMutex requestMutex;

    public:

        //! Constructor
        explicit RHttpClient(RHttpClient::Type type, const RHttpClientSettings &httpClientSettings, QObject *parent = nullptr);

        //! Send message.
        void sendRequest(const RHttpMessage &httpMessageRequest, RHttpMessage &httpMessageReply);

        //! Abort current request.
        void abort();

    private:

        //! Build SSL configuration.
        QSslConfiguration buildSslConfiguration() const;

        //! Load private key and try all available algorithms.
        QSslKey loadPrivateKey() const;

        //! Returns name of given algorithm.
        static QString getKeyAlgorithmName(QSsl::KeyAlgorithm algorithm);

    public slots:

        void setHttpClientSettings(const RHttpClientSettings &httpClientSettings);

    protected slots:

        void onAuthenticationRequired(QNetworkReply *, QAuthenticator *);

        void onProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *authenticator);

        void onReadyRead();

        void onErrorOccurred(QNetworkReply::NetworkError code);

        void onFinished();

        void onSslErrors(const QList<QSslError> &errors);

        void onRequestAvailable(const RHttpMessage &httpMessageRequest);

        void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

        void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    signals:

        void requestAvailable(const RHttpMessage &httpMessageRequest);

        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    public:

        static QString buildUrl(const QString &address, const uint port, const QString &topic = QString());

};

#endif // RCL_HTTP_CLIENT_H
