#include "rcl_cloud_session_info.h"

void RCloudSessionInfo::_init(const RCloudSessionInfo *pSession)
{
    if (pSession)
    {
        this->name = pSession->name;
        this->host = pSession->host;
        this->client = pSession->client;
    }
}

RCloudSessionInfo::RCloudSessionInfo()
{
    this->_init();
}

RCloudSessionInfo::RCloudSessionInfo(const RCloudSessionInfo &session)
{
    this->_init(&session);
}

RCloudSessionInfo::~RCloudSessionInfo()
{

}

RCloudSessionInfo &RCloudSessionInfo::operator =(const RCloudSessionInfo &session)
{
    this->_init(&session);
    return (*this);
}

const QString &RCloudSessionInfo::getName() const
{
    return this->name;
}

void RCloudSessionInfo::setName(const QString &name)
{
    this->name = name;
}

const QString &RCloudSessionInfo::getHostName() const
{
    return this->host.hostName;
}

void RCloudSessionInfo::setHostName(const QString &hostName)
{
    this->host.hostName = hostName;
}

quint16 RCloudSessionInfo::getPublicPort() const
{
    return this->host.publicPort;
}

void RCloudSessionInfo::setPublicPort(quint16 publicPort)
{
    this->host.publicPort = publicPort;
}

quint16 RCloudSessionInfo::getPrivatePort() const
{
    return this->host.privatePort;
}

void RCloudSessionInfo::setPrivatePort(quint16 privatePort)
{
    this->host.privatePort = privatePort;
}

uint RCloudSessionInfo::getTimeout() const
{
    return this->host.timeout;
}

void RCloudSessionInfo::setTimeout(uint timeout)
{
    this->host.timeout = timeout;
}

const QString &RCloudSessionInfo::getHostCertificate() const
{
    return this->host.certificate;
}

void RCloudSessionInfo::setHostCertificate(const QString &certificate)
{
    this->host.certificate = certificate;
}

const QString &RCloudSessionInfo::getClientPrivateKey() const
{
    return this->client.privateKey;
}

void RCloudSessionInfo::setClientPrivateKey(const QString &privateKey)
{
    this->client.privateKey = privateKey;
}

const QString &RCloudSessionInfo::getClientPrivateKeyPassword() const
{
    return this->client.privateKeyPassword;
}

void RCloudSessionInfo::setClientPrivateKeyPassword(const QString &privateKeyPassword)
{
    this->client.privateKeyPassword = privateKeyPassword;
}

const QString &RCloudSessionInfo::getClientCertificate() const
{
    return this->client.certificate;
}

void RCloudSessionInfo::setClientCertificate(const QString &certificate)
{
    this->client.certificate = certificate;
}

RCloudSessionInfo RCloudSessionInfo::fromJson(const QJsonObject &json)
{
    RCloudSessionInfo session;

    if (const QJsonValue &v = json["name"]; v.isString())
    {
        session.name = v.toString();
    }

    if (const QJsonValue &v = json["host"]; v.isObject())
    {
        QJsonObject hostJson = v.toObject();
        if (const QJsonValue &v = hostJson["hostName"]; v.isString())
        {
            session.host.hostName = v.toString();
        }
        if (const QJsonValue &v = hostJson["publicPort"]; v.isDouble())
        {
            session.host.publicPort = v.toInt();
        }
        if (const QJsonValue &v = hostJson["privatePort"]; v.isDouble())
        {
            session.host.privatePort = v.toInt();
        }
        if (const QJsonValue &v = hostJson["timeout"]; v.isDouble())
        {
            session.host.timeout = v.toInt();
        }
        if (const QJsonValue &v = hostJson["certificate"]; v.isString())
        {
            session.host.certificate = v.toString();
        }
    }

    if (const QJsonValue &v = json["client"]; v.isObject())
    {
        QJsonObject hostJson = v.toObject();
        if (const QJsonValue &v = hostJson["privateKey"]; v.isString())
        {
            session.client.privateKey = v.toString();
        }
        if (const QJsonValue &v = hostJson["privateKeyPassword"]; v.isString())
        {
            session.client.privateKeyPassword = v.toString();
        }
        if (const QJsonValue &v = hostJson["certificate"]; v.isString())
        {
            session.client.certificate = v.toString();
        }
    }

    return session;
}

QJsonObject RCloudSessionInfo::toJson() const
{
    QJsonObject json;

    json["name"] = this->name;

    QJsonObject jsonHost;
    jsonHost["hostName"] = this->host.hostName;
    jsonHost["publicPort"] = this->host.publicPort;
    jsonHost["privatePort"] = this->host.privatePort;
    jsonHost["timeout"] = int(this->host.timeout);
    jsonHost["certificate"] = this->host.certificate;

    json["host"] = jsonHost;

    QJsonObject jsonClient;
    jsonClient["privateKey"] = this->client.privateKey;
    jsonClient["privateKeyPassword"] = this->client.privateKeyPassword;
    jsonClient["certificate"] = this->client.certificate;

    json["client"] = jsonClient;

    return json;
}
