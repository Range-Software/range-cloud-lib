#include "rcl_cloud_action_info.h"


void RCloudActionInfo::_init(const RCloudActionInfo *pActionInfo)
{
    if (pActionInfo)
    {
        this->name = pActionInfo->name;
        this->accessRights = pActionInfo->accessRights;
    }
}

RCloudActionInfo::RCloudActionInfo()
{
    this->_init();
}

RCloudActionInfo::RCloudActionInfo(const RCloudActionInfo &actionInfo)
{
    this->_init(&actionInfo);
}

RCloudActionInfo::~RCloudActionInfo()
{

}

RCloudActionInfo &RCloudActionInfo::operator=(const RCloudActionInfo &actionInfo)
{
    this->_init(&actionInfo);
    return (*this);
}

const QString &RCloudActionInfo::getName() const
{
    return this->name;
}

void RCloudActionInfo::setName(const QString &name)
{
    this->name = name;
}

const RAccessRights &RCloudActionInfo::getAccessRights() const
{
    return this->accessRights;
}

void RCloudActionInfo::setAccessRights(const RAccessRights &accessRights)
{
    this->accessRights = accessRights;
}

bool RCloudActionInfo::isNull() const
{
    return (this->name.isNull() || this->name.isEmpty());
}

RCloudActionInfo RCloudActionInfo::fromJson(const QJsonObject &json)
{
    RCloudActionInfo actionInfo;

    if (const QJsonValue &v = json["name"]; v.isString())
    {
        actionInfo.name = v.toString();
    }
    if (const QJsonValue &v = json["access"]; v.isObject())
    {
        actionInfo.accessRights = RAccessRights::fromJson(v.toObject());
    }

    return actionInfo;
}

QJsonObject RCloudActionInfo::toJson() const
{
    QJsonObject json;

    json["name"] = this->name;
    json["access"] = this->accessRights.toJson();

    return json;
}
