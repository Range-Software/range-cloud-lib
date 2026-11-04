#ifndef RCL_HTTP_CLIENT_SETTINGS
#define RCL_HTTP_CLIENT_SETTINGS

#include <QString>

#include "rcl_http_settings.h"
#include "rcl_http_proxy_settings.h"

class RHttpClientSettings : public RHttpSettings
{

    public:

        //! Host certificate verification.
        enum Verification
        {
            //! Verify certificate chain and host name.
            FullVerification = 0,
            //! Verify certificate chain only, skip host name verification.
            CertificateVerification,
            //! No verification.
            NoVerification
        };

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
        //! Host certificate verification.
        Verification verification;

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

        //! Return host certificate verification.
        Verification getVerification() const;

        //! Set new host certificate verification.
        void setVerification(Verification verification);

        //! Return verification key.
        static QString verificationToKey(Verification verification);

        //! Return verification from key.
        static Verification verificationFromKey(const QString &key);

};

#endif // RCL_HTTP_CLIENT_SETTINGS
