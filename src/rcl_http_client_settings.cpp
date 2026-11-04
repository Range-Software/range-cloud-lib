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
        this->verification = pHttpClientSettings->verification;
    }
}

RHttpClientSettings::RHttpClientSettings()
    : timeout(RHttpClientSettings::defaultTimeout)
    , verification(RHttpClientSettings::FullVerification)
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

RHttpClientSettings::Verification RHttpClientSettings::getVerification() const
{
    return this->verification;
}

void RHttpClientSettings::setVerification(Verification verification)
{
    this->verification = verification;
}

QString RHttpClientSettings::verificationToKey(Verification verification)
{
    switch (verification)
    {
        case CertificateVerification: return "certificate";
        case NoVerification:          return "none";
        default:                      return "full";
    }
}

RHttpClientSettings::Verification RHttpClientSettings::verificationFromKey(const QString &key)
{
    if (key == "certificate")
    {
        return CertificateVerification;
    }
    if (key == "none")
    {
        return NoVerification;
    }
    return FullVerification;
}
