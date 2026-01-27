#include "rcl_http_server_settings.h"
#include <QFileInfo>

void RHttpServerSettings::_init(const RHttpServerSettings *pHttpServerSettings)
{
    this->RHttpSettings::_init(pHttpServerSettings);
    if (pHttpServerSettings)
    {
        this->rateLimitPerSecond = pHttpServerSettings->rateLimitPerSecond;
        this->responseTimeoutMs = pHttpServerSettings->responseTimeoutMs;
        this->handlerCleanupIntervalMs = pHttpServerSettings->handlerCleanupIntervalMs;
        this->maxStaleHandlerAgeMs = pHttpServerSettings->maxStaleHandlerAgeMs;
    }
    else
    {
        this->rateLimitPerSecond = defaultRateLimitPerSecond;
        this->responseTimeoutMs = defaultResponseTimeoutMs;
        this->handlerCleanupIntervalMs = defaultHandlerCleanupIntervalMs;
        this->maxStaleHandlerAgeMs = defaultMaxStaleHandlerAgeMs;
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

quint32 RHttpServerSettings::getResponseTimeoutMs() const
{
    return this->responseTimeoutMs;
}

void RHttpServerSettings::setResponseTimeoutMs(quint32 timeoutMs)
{
    this->responseTimeoutMs = timeoutMs;
}

quint32 RHttpServerSettings::getHandlerCleanupIntervalMs() const
{
    return this->handlerCleanupIntervalMs;
}

void RHttpServerSettings::setHandlerCleanupIntervalMs(quint32 intervalMs)
{
    this->handlerCleanupIntervalMs = intervalMs;
}

quint32 RHttpServerSettings::getMaxStaleHandlerAgeMs() const
{
    return this->maxStaleHandlerAgeMs;
}

void RHttpServerSettings::setMaxStaleHandlerAgeMs(quint32 ageMs)
{
    this->maxStaleHandlerAgeMs = ageMs;
}

bool RHttpServerSettings::validate(QString *errorMessage) const
{
    // Validate port
    if (this->getPort() == 0)
    {
        if (errorMessage)
        {
            *errorMessage = "Invalid port: port must be greater than 0";
        }
        return false;
    }

    // Validate TLS key store files
    QFileInfo privateKeyFile(this->getTlsKeyStore().getKeyFile());
    if (!privateKeyFile.exists())
    {
        if (errorMessage)
        {
            *errorMessage = QString("Private key file does not exist: %1").arg(privateKeyFile.filePath());
        }
        return false;
    }
    if (!privateKeyFile.isReadable())
    {
        if (errorMessage)
        {
            *errorMessage = QString("Private key file is not readable: %1").arg(privateKeyFile.filePath());
        }
        return false;
    }

    QFileInfo certificateFile(this->getTlsKeyStore().getCertificateFile());
    if (!certificateFile.exists())
    {
        if (errorMessage)
        {
            *errorMessage = QString("Certificate file does not exist: %1").arg(certificateFile.filePath());
        }
        return false;
    }
    if (!certificateFile.isReadable())
    {
        if (errorMessage)
        {
            *errorMessage = QString("Certificate file is not readable: %1").arg(certificateFile.filePath());
        }
        return false;
    }

    // Validate TLS trust store files
    QFileInfo caCertificateFile(this->getTlsTrustStore().getCertificateFile());
    if (!caCertificateFile.exists())
    {
        if (errorMessage)
        {
            *errorMessage = QString("CA certificate file does not exist: %1").arg(caCertificateFile.filePath());
        }
        return false;
    }
    if (!caCertificateFile.isReadable())
    {
        if (errorMessage)
        {
            *errorMessage = QString("CA certificate file is not readable: %1").arg(caCertificateFile.filePath());
        }
        return false;
    }

    // Validate timeouts
    if (this->responseTimeoutMs == 0)
    {
        if (errorMessage)
        {
            *errorMessage = "Invalid response timeout: must be greater than 0";
        }
        return false;
    }

    if (this->maxStaleHandlerAgeMs < this->responseTimeoutMs)
    {
        if (errorMessage)
        {
            *errorMessage = "Invalid configuration: maxStaleHandlerAgeMs must be >= responseTimeoutMs";
        }
        return false;
    }

    return true;
}

