#ifndef RCL_CLOUD_SESSION_INFO_H
#define RCL_CLOUD_SESSION_INFO_H

#include <QString>
#include <QJsonObject>

#include "rcl_tls_key_store.h"
#include "rcl_tls_trust_store.h"

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
            //! Host or CA trust store.
            RTlsTrustStore trustStore;
        } host;

        // Client
        struct Client
        {
            //! Client keyStore.
            RTlsKeyStore keyStore;
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

        //! Return const reference to host trust store.
        const RTlsTrustStore &getHostTrustStore() const;

        //! Set new host trust store.
        void setHostTrustStore(const RTlsTrustStore &trustStore);

        //! Return const reference to client key store.
        const RTlsKeyStore &getClientKeyStore() const;

        //! Set new client key store.
        void setClientKeyStore(const RTlsKeyStore &keyStore);

        //! Create session info object from Json.
        static RCloudSessionInfo fromJson(const QJsonObject &json);

        //! Create Json from session info object.
        QJsonObject toJson() const;

};

#endif // RCL_CLOUD_SESSION_INFO_H
