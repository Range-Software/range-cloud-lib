#ifndef RCL_HTTP_SERVER_SETTINGS
#define RCL_HTTP_SERVER_SETTINGS

#include <QString>

#include "rcl_http_settings.h"

class RHttpServerSettings : public RHttpSettings
{

    public:

    static quint32 constexpr defaultRateLimitPerSecond = 10;
    static quint32 constexpr defaultResponseTimeoutMs = 10000;
    static quint32 constexpr defaultHandlerCleanupIntervalMs = 60000;
    static quint32 constexpr defaultMaxStaleHandlerAgeMs = 120000;

    protected:

        quint32 rateLimitPerSecond;
        quint32 responseTimeoutMs;
        quint32 handlerCleanupIntervalMs;
        quint32 maxStaleHandlerAgeMs;

    protected:

        //! Internal initialization function.
        void _init(const RHttpServerSettings *pHttpServerSettings = nullptr);

    public:

        //! Constructor.
        RHttpServerSettings();

        //! Copy constructor.
        RHttpServerSettings(const RHttpServerSettings &httpServerSettings);

        //! Destructor.
        ~RHttpServerSettings();

        //! Assignment operator (message).
        RHttpServerSettings &operator =(const RHttpServerSettings &httpServerSettings);

        //! Return maximum number of incoming requests per second per IP accepted by the server.
        quint32 getRateLimitPerSecond() const;

        //! Set maximum number of incoming requests per second per IP accepted by the server.
        void setRateLimitPerSecond(quint32 rateLimitPerSecond);

        //! Return response timeout in milliseconds.
        quint32 getResponseTimeoutMs() const;

        //! Set response timeout in milliseconds.
        void setResponseTimeoutMs(quint32 timeoutMs);

        //! Return handler cleanup interval in milliseconds.
        quint32 getHandlerCleanupIntervalMs() const;

        //! Set handler cleanup interval in milliseconds.
        void setHandlerCleanupIntervalMs(quint32 intervalMs);

        //! Return maximum stale handler age in milliseconds.
        quint32 getMaxStaleHandlerAgeMs() const;

        //! Set maximum stale handler age in milliseconds.
        void setMaxStaleHandlerAgeMs(quint32 ageMs);

        //! Validate settings and return true if valid.
        bool validate(QString *errorMessage = nullptr) const;

};

#endif // RCL_HTTP_SERVER_SETTINGS
