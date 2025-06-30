#ifndef RCL_CLOUD_SESSION_INFO_H
#define RCL_CLOUD_SESSION_INFO_H

#include <QString>
#include <QJsonObject>

class RCloudSessionInfo
{

    protected:

        //! Internal initialization function.
        void _init(const RCloudSessionInfo *pSession = nullptr);

    protected:

        //! Session name.
        QString name;

        // Host
        struct Host
        {
            //! Host name.
            QString hostName;
            //! Public port.
            quint16 publicPort;
            //! Private port.
            quint16 privatePort;
            //! Connection timeout in milliseconds.
            uint timeout;
            //! Host or CA certificate.
            QString certificate;
        } host;

        // Client
        struct Client
        {
            //! Private key.
            QString privateKey;
            //! Private key password.
            QString privateKeyPassword;
            //! Client certificate.
            QString certificate;
        } client;

    public:

        //! Constructor.
        RCloudSessionInfo();

        //! Copy constructor.
        RCloudSessionInfo(const RCloudSessionInfo &session);

        //! Destructor.
        ~RCloudSessionInfo();

        //! Assignment operator.
        RCloudSessionInfo &operator =(const RCloudSessionInfo &session);

        //! Return const reference to name.
        const QString &getName() const;

        //! Set new name.
        void setName(const QString &name);

        //! Return const reference to host-name.
        const QString &getHostName() const;

        //! Set new host-name.
        void setHostName(const QString &hostName);

        //! Return public port.
        quint16 getPublicPort() const;

        //! Set new public port.
        void setPublicPort(quint16 publicPort);

        //! Return private port.
        quint16 getPrivatePort() const;

        //! Set new private port.
        void setPrivatePort(quint16 privatePort);

        //! Return timeout.
        uint getTimeout() const;

        //! Set new timeout.
        void setTimeout(uint timeout);

        //! Return const reference to host certificate.
        const QString &getHostCertificate() const;

        //! Set new host certificate.
        void setHostCertificate(const QString &certificate);

        //! Return const reference to client private key.
        const QString &getClientPrivateKey() const;

        //! Set new client private key.
        void setClientPrivateKey(const QString &privateKey);

        //! Return const reference to client private key password.
        const QString &getClientPrivateKeyPassword() const;

        //! Set new client private key password.
        void setClientPrivateKeyPassword(const QString &privateKeyPassword);

        //! Return const reference to client private key.
        const QString &getClientCertificate() const;

        //! Set new client certificate.
        void setClientCertificate(const QString &certificate);

        //! Create user object from Json.
        static RCloudSessionInfo fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;

};

#endif // RCL_CLOUD_SESSION_INFO_H
