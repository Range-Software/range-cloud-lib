#ifndef RCL_HTTP_CLIENT_SETTINGS
#define RCL_HTTP_CLIENT_SETTINGS

#include <QString>

#include "rcl_http_settings.h"
#include "rcl_http_proxy_settings.h"

class RHttpClientSettings : public RHttpSettings
{

    protected:

        //! Internal initialization function.
        void _init(const RHttpClientSettings *pHttpClientSettings = nullptr);

    protected:

        //! URL.
        QString url;
        //! Connection timeout in milliseconds.
        uint timeout;
        //! Proxy type.
        RHttpProxySettings proxySettings;

    public:

        static const uint defaultTimeout;

    public:

        //! Constructor.
        RHttpClientSettings();

        //! Copy constructor.
        RHttpClientSettings(const RHttpClientSettings &httpClientSettings);

        //! Destructor.
        ~RHttpClientSettings();

        //! Assignment operator (message).
        RHttpClientSettings &operator =(const RHttpClientSettings &httpClientSettings);

        //! Get const reference to url.
        const QString &getUrl() const;

        //! Set new url.
        void setUrl(const QString &url);

        //! Return connection timeout.
        uint getTimeout() const;

        //! Set new connection timeout.
        void setTimeout(uint timeout);

        //! Get proxy settings.
        const RHttpProxySettings &getProxySettings() const;

        //! Set new proxy settings.
        void setProxySettings(const RHttpProxySettings &proxySettings);

};

#endif // RCL_HTTP_CLIENT_SETTINGS
