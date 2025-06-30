#include "rcl_http_settings.h"

void RHttpSettings::_init(const RHttpSettings *pHttpSettings)
{
    if (pHttpSettings)
    {
        this->port = pHttpSettings->port;
        this->tlsKeyStore = pHttpSettings->tlsKeyStore;
        this->tlsTrustStore = pHttpSettings->tlsTrustStore;
    }
}

RHttpSettings::RHttpSettings()
{
    this->_init();

}

RHttpSettings::RHttpSettings(const RHttpSettings &httpSettings)
{
    this->_init(&httpSettings);
}

RHttpSettings::~RHttpSettings()
{

}

RHttpSettings &RHttpSettings::operator =(const RHttpSettings &httpSettings)
{
    this->_init(&httpSettings);
    return (*this);
}

quint16 RHttpSettings::getPort() const
{
    return this->port;
}

void RHttpSettings::setPort(quint16 port)
{
    this->port = port;
}

const RTlsKeyStore &RHttpSettings::getTlsKeyStore() const
{
    return this->tlsKeyStore;
}

void RHttpSettings::setTlsKeyStore(const RTlsKeyStore &tlsKeyStore)
{
    this->tlsKeyStore = tlsKeyStore;
}

const RTlsTrustStore &RHttpSettings::getTlsTrustStore() const
{
    return this->tlsTrustStore;
}

void RHttpSettings::setTlsTrustStore(const RTlsTrustStore &tlsTrustStore)
{
    this->tlsTrustStore = tlsTrustStore;
}
