#include "rcl_http_server_settings.h"

void RHttpServerSettings::_init(const RHttpServerSettings *pHttpServerSettings)
{
    this->RHttpSettings::_init(pHttpServerSettings);
    if (pHttpServerSettings)
    {
        this->rateLimitPerSecond = pHttpServerSettings->rateLimitPerSecond;
    }
}

RHttpServerSettings::RHttpServerSettings()
{
    this->_init();

}

RHttpServerSettings::RHttpServerSettings(const RHttpServerSettings &httpServerSettings)
    : RHttpSettings(httpServerSettings)
{
    this->_init(&httpServerSettings);
}

RHttpServerSettings::~RHttpServerSettings()
{

}

RHttpServerSettings &RHttpServerSettings::operator =(const RHttpServerSettings &httpServerSettings)
{
    this->_init(&httpServerSettings);
    return (*this);
}

quint32 RHttpServerSettings::getRateLimitPerSecond() const
{
    return this->rateLimitPerSecond;
}

void RHttpServerSettings::setRateLimitPerSecond(quint32 rateLimitPerSecond)
{
    this->rateLimitPerSecond = rateLimitPerSecond;
}

