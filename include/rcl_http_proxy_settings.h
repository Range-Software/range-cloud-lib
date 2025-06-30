#ifndef RCL_HTTP_PROXY_SETTINGS_H
#define RCL_HTTP_PROXY_SETTINGS_H

#include <QString>

class RHttpProxySettings
{

    public:

        //! Proxy type.
        enum Type
        {
            NoProxy = 0,
            ManualProxy,
            SystemProxy
        };

    protected:

        //! Internal initialization function.
        void _init(const RHttpProxySettings *pHttpProxySettings = nullptr);

    protected:

        //! Proxy type.
        Type type;
        //! Proxy host.
        QString host;
        //! Proxy port.
        quint16 port;
        //! Proxy user.
        QString user;
        //! Proxy password.
        QString password;

    public:

        //! Constructor.
        RHttpProxySettings();

        //! Copy constructor.
        RHttpProxySettings(const RHttpProxySettings &httpProxySettings);

        //! Destructor.
        ~RHttpProxySettings();

        //! Assignment operator (message).
        RHttpProxySettings &operator =(const RHttpProxySettings &httpProxySettings);

        //! Get proxy type.
        Type getType() const;

        //! Set new proxy type.
        void setType(Type type);

        //! Get const reference to proxy host.
        const QString &getHost() const;

        //! Set new proxy host.
        void setHost(const QString &host);

        //! Get const reference to proxy port.
        quint16 getPort() const;

        //! Set new proxy port.
        void setPort(quint16 port);

        //! Get const reference to proxy user.
        const QString &getUser() const;

        //! Set new proxy password.
        void setUser(const QString &user);

        //! Get const reference to proxy password.
        const QString &getPassword() const;

        //! Set new proxy password.
        void setPassword(const QString &password);

};

#endif // RCL_HTTP_PROXY_SETTINGS_H
