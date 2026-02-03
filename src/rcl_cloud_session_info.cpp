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

bool RCloudSessionInfo::isValid() const
{
    if (this->name.isEmpty() ||
        this->host.hostName.isEmpty())
    {
        return false;
    }
    return true;
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

const RTlsTrustStore &RCloudSessionInfo::getHostTrustStore() const
{
    return this->host.trustStore;
}

void RCloudSessionInfo::setHostTrustStore(const RTlsTrustStore &trustStore)
{
    this->host.trustStore = trustStore;
}

const RTlsKeyStore &RCloudSessionInfo::getClientKeyStore() const
{
    return this->client.keyStore;
}

void RCloudSessionInfo::setClientKeyStore(const RTlsKeyStore &keyStore)
{
    this->client.keyStore = keyStore;
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
        if (const QJsonValue &v = hostJson["trustStore"]; v.isObject())
        {
            session.host.trustStore = RTlsTrustStore::fromJson(v.toObject());
        }
    }

    if (const QJsonValue &v = json["client"]; v.isObject())
    {
        QJsonObject hostJson = v.toObject();
        if (const QJsonValue &v = hostJson["keyStore"]; v.isObject())
        {
            session.client.keyStore = RTlsKeyStore::fromJson(v.toObject());
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
    jsonHost["trustStore"] = this->host.trustStore.toJson();

    json["host"] = jsonHost;

    QJsonObject jsonClient;
    jsonClient["keyStore"] = this->client.keyStore.toJson();

    json["client"] = jsonClient;

    return json;
}
