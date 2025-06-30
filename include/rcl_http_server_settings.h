#ifndef RCL_HTTP_SERVER_SETTINGS
#define RCL_HTTP_SERVER_SETTINGS

#include <QString>

#include "rcl_http_settings.h"

class RHttpServerSettings : public RHttpSettings
{

    public:

    static quint32 constexpr defaultRateLimitPerSecond = 10;

    protected:

        quint32 rateLimitPerSecond;

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

};

#endif // RCL_HTTP_SERVER_SETTINGS
