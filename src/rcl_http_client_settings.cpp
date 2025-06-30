#include "rcl_http_client_settings.h"

const uint RHttpClientSettings::defaultTimeout = 10000;

void RHttpClientSettings::_init(const RHttpClientSettings *pHttpClientSettings)
{
    this->RHttpSettings::_init(pHttpClientSettings);
    if (pHttpClientSettings)
    {
        this->url = pHttpClientSettings->url;
        this->timeout = pHttpClientSettings->timeout;
        this->proxySettings = pHttpClientSettings->proxySettings;
    }
}

RHttpClientSettings::RHttpClientSettings()
    : timeout(RHttpClientSettings::defaultTimeout)
{
    this->_init();
}

RHttpClientSettings::RHttpClientSettings(const RHttpClientSettings &httpClientSettings)
    : RHttpSettings(httpClientSettings)
{
    this->_init(&httpClientSettings);
}

RHttpClientSettings::~RHttpClientSettings()
{

}

RHttpClientSettings &RHttpClientSettings::operator =(const RHttpClientSettings &httpClientSettings)
{
    this->_init(&httpClientSettings);
    return (*this);
}

const QString &RHttpClientSettings::getUrl() const
{
    return this->url;
}

void RHttpClientSettings::setUrl(const QString &url)
{
    this->url = url;
}

uint RHttpClientSettings::getTimeout() const
{
    return this->timeout;
}

void RHttpClientSettings::setTimeout(uint timeout)
{
    this->timeout = timeout;
}

const RHttpProxySettings &RHttpClientSettings::getProxySettings() const
{
    return this->proxySettings;
}

void RHttpClientSettings::setProxySettings(const RHttpProxySettings &proxySettings)
{
    this->proxySettings = proxySettings;
}
