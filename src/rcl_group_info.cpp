#include "rcl_group_info.h"

void RGroupInfo::_init(const RGroupInfo *pGroupInfo)
{
    if (pGroupInfo)
    {
        this->name = pGroupInfo->name;
    }
}

RGroupInfo::RGroupInfo()
{
    this->_init();
}

RGroupInfo::RGroupInfo(const RGroupInfo &groupInfo)
{
    this->_init(&groupInfo);
}

RGroupInfo::~RGroupInfo()
{

}

RGroupInfo &RGroupInfo::operator=(const RGroupInfo &groupInfo)
{
    this->_init(&groupInfo);
    return (*this);
}

const QString &RGroupInfo::getName() const
{
    return this->name;
}

void RGroupInfo::setName(const QString &name)
{
    this->name = name;
}

bool RGroupInfo::isNull() const
{
    return (this->name.isNull() || this->name.isEmpty());
}

RGroupInfo RGroupInfo::fromJson(const QJsonObject &json)
{
    RGroupInfo groupInfo;

    groupInfo.name = RGroupInfo::groupNameFromJson(json);

    return groupInfo;
}

QJsonObject RGroupInfo::toJson() const
{

    return RGroupInfo::groupNameToJson(this->name);
}

QString RGroupInfo::groupNameFromJson(const QJsonObject &json)
{
    if (const QJsonValue &v = json["name"]; v.isString())
    {
        return v.toString();
    }
    return QString();
}

QJsonObject RGroupInfo::groupNameToJson(const QString &groupName)
{
    QJsonObject json;

    json["name"] = groupName;

    return json;
}

bool RGroupInfo::isNameValid(const QString &name)
{
    static QRegularExpression re("^[a-zA-Z0-9@.-]{4,32}$");
    return re.match(name).hasMatch();
}
