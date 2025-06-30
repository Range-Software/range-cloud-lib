#ifndef RCL_HTTP_SETTINGS
#define RCL_HTTP_SETTINGS

#include <QString>

#include "rcl_tls_key_store.h"
#include "rcl_tls_trust_store.h"

class RHttpSettings
{

    protected:

        //! Internal initialization function.
        void _init(const RHttpSettings *pHttpSettings = nullptr);

    protected:

        //! Port.
        quint16 port;
        //! TLS keystore.
        RTlsKeyStore tlsKeyStore;
        //! TLS truststore.
        RTlsTrustStore tlsTrustStore;

    public:

        //! Constructor.
        RHttpSettings();

        //! Copy constructor.
        RHttpSettings(const RHttpSettings &httpSettings);

        //! Destructor.
        ~RHttpSettings();

        //! Assignment operator (message).
        RHttpSettings &operator =(const RHttpSettings &httpSettings);

        //! Get port.
        quint16 getPort() const;

        //! Set new port.
        void setPort(quint16 port);

        //! Get const reference to TLS keystore.
        const RTlsKeyStore &getTlsKeyStore() const;

        //! Set new TLS keystore.
        void setTlsKeyStore(const RTlsKeyStore &tlsKeyStore);

        //! Get const reference to TLS truststore.
        const RTlsTrustStore &getTlsTrustStore() const;

        //! Set new TLS truststore.
        void setTlsTrustStore(const RTlsTrustStore &tlsTrustStore);

};

#endif // RCL_HTTP_SETTINGS
