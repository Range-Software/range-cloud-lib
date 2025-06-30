#include <QJsonArray>

#include "rcl_cloud_process_request.h"

void RCloudProcessRequest::_init(const RCloudProcessRequest *pProcessRequest)
{
    if (pProcessRequest)
    {
        this->executor = pProcessRequest->executor;
        this->name = pProcessRequest->name;
        this->argumentValues = pProcessRequest->argumentValues;
    }
}

RCloudProcessRequest::RCloudProcessRequest()
{
    this->_init();
}

RCloudProcessRequest::RCloudProcessRequest(const RCloudProcessRequest &processRequest)
{
    this->_init(&processRequest);
}

RCloudProcessRequest::~RCloudProcessRequest()
{

}

RCloudProcessRequest &RCloudProcessRequest::operator=(const RCloudProcessRequest &processRequest)
{
    this->_init(&processRequest);
    return (*this);
}

const RUserInfo &RCloudProcessRequest::getExecutor() const
{
    return this->executor;
}

void RCloudProcessRequest::setExecutor(const RUserInfo &executor)
{
    this->executor = executor;
}

const QString &RCloudProcessRequest::getName() const
{
    return name;
}

void RCloudProcessRequest::setName(const QString &name)
{
    this->name = name;
}

const QMap<QString, QString> &RCloudProcessRequest::getArgumentValues() const
{
    return this->argumentValues;
}

void RCloudProcessRequest::setArgumentValues(const QMap<QString, QString> &argumentValues)
{
    this->argumentValues = argumentValues;
}

RCloudProcessRequest RCloudProcessRequest::fromJson(const QJsonObject &json)
{
    RCloudProcessRequest request;

    if (const QJsonValue &v = json["name"]; v.isString())
    {
        request.name = v.toString();
    }

    if (const QJsonValue &v = json["arguments"]; v.isObject())
    {
        request.argumentValues = RCloudProcessRequest::mapFromJson(v.toObject());
    }

    return request;
}

QJsonObject RCloudProcessRequest::toJson() const
{
    QJsonObject json;
    json["name"] = this->name;
    json["arguments"] = RCloudProcessRequest::mapToJson(this->argumentValues);

    return json;
}

QMap<QString, QString> RCloudProcessRequest::mapFromJson(const QJsonObject &json)
{
    QMap<QString, QString> map;
    for (auto iter = json.constBegin(); iter != json.constEnd(); ++iter)
    {
        if (const QJsonValue &v = json[iter.key()]; v.isString())
        {
            map.insert(iter.key(),v.toString(iter.key()));
        }
    }
    return map;
}

QJsonObject RCloudProcessRequest::mapToJson(const QMap<QString, QString> &map)
{
    QJsonObject json;

    for (auto it = map.cbegin(), end = map.cend(); it != end; ++it)
    {
        json[it.key()] = *it;
    }

    return json;
}
