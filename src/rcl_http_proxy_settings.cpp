#include "rcl_http_proxy_settings.h"

void RHttpProxySettings::_init(const RHttpProxySettings *pHttpProxySettings)
{
    if (pHttpProxySettings)
    {
        this->type = pHttpProxySettings->type;
        this->host = pHttpProxySettings->host;
        this->port = pHttpProxySettings->port;
        this->user = pHttpProxySettings->user;
        this->password = pHttpProxySettings->password;
    }
}

RHttpProxySettings::RHttpProxySettings()
{
    this->_init();

}

RHttpProxySettings::RHttpProxySettings(const RHttpProxySettings &httpProxySettings)
    : type(Type::SystemProxy)
{
    this->_init(&httpProxySettings);
}

RHttpProxySettings::~RHttpProxySettings()
{

}

RHttpProxySettings &RHttpProxySettings::operator =(const RHttpProxySettings &httpProxySettings)
{
    this->_init(&httpProxySettings);
    return (*this);
}

RHttpProxySettings::Type RHttpProxySettings::getType() const
{
    return this->type;
}

void RHttpProxySettings::setType(Type type)
{
    this->type = type;
}

const QString &RHttpProxySettings::getHost() const
{
    return this->host;
}

void RHttpProxySettings::setHost(const QString &host)
{
    this->host = host;
}

quint16 RHttpProxySettings::getPort() const
{
    return this->port;
}

void RHttpProxySettings::setPort(quint16 port)
{
    this->port = port;
}

const QString &RHttpProxySettings::getUser() const
{
    return this->user;
}

void RHttpProxySettings::setUser(const QString &user)
{
    this->user = user;
}

const QString &RHttpProxySettings::getPassword() const
{
    return this->password;
}

void RHttpProxySettings::setPassword(const QString &password)
{
    this->password = password;
}
