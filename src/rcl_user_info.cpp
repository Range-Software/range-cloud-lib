#include <QJsonArray>

#include "rcl_user_info.h"

const QString RUserInfo::rootUser = "root";
const QString RUserInfo::rootGroup = "root";
const QString RUserInfo::guestUser = "guest";
const QString RUserInfo::guestGroup = "guest";
const QString RUserInfo::userGroup = "users";

void RUserInfo::_init(const RUserInfo *pUserInfo)
{
    if (pUserInfo)
    {
        this->name = pUserInfo->name;
        this->groupNames = pUserInfo->groupNames;
    }
}

RUserInfo::RUserInfo()
{
    this->_init();
}

RUserInfo::RUserInfo(const RUserInfo &userInfo)
{
    this->_init(&userInfo);
}

RUserInfo::~RUserInfo()
{

}

RUserInfo &RUserInfo::operator=(const RUserInfo &userInfo)
{
    this->_init(&userInfo);
    return (*this);
}

const QString &RUserInfo::getName() const
{
    return this->name;
}

void RUserInfo::setName(const QString &name)
{
    this->name = name;
}

const QList<QString> &RUserInfo::getGroupNames() const
{
    return this->groupNames;
}

QList<QString> &RUserInfo::getGroupNames()
{
    return this->groupNames;
}

void RUserInfo::setGroupNames(const QList<QString> &groupNames)
{
    this->groupNames = groupNames;
}

bool RUserInfo::isNull() const
{
    return (this->name.isNull() || this->name.isEmpty());
}

bool RUserInfo::isUser(const QString &name) const
{
    return (this->name == name);
}

bool RUserInfo::hasGroup(const QString &groupName) const
{
    return this->groupNames.contains(groupName);
}

RUserInfo RUserInfo::fromJson(const QJsonObject &json)
{
    RUserInfo userInfo;

    userInfo.name = RUserInfo::userNameFromJson(json);

    if (const QJsonValue &v = json["groups"]; v.isArray())
    {
        const QJsonArray &groupNames = v.toArray();

        userInfo.groupNames.reserve(groupNames.size());
        for (const QJsonValue &groupName : groupNames)
        {
            if (groupName.isString())
            {
                userInfo.groupNames.append(groupName.toString());
            }
        }
    }

    return userInfo;
}

QJsonObject RUserInfo::toJson() const
{
    QJsonObject json;

    json = RUserInfo::userNameToJson(this->name);

    QJsonArray groupsArray;
    for (const QString &groupName : this->groupNames)
    {
        groupsArray.append(groupName);
    }
    json["groups"] = groupsArray;

    return json;
}

QString RUserInfo::userNameFromJson(const QJsonObject &json)
{
    if (const QJsonValue &v = json["name"]; v.isString())
    {
        return v.toString();
    }
    return QString();
}

QJsonObject RUserInfo::userNameToJson(const QString &userName)
{
    QJsonObject json;

    json["name"] = userName;

    return json;
}

bool RUserInfo::isNameValid(const QString &name)
{
    static QRegularExpression re("^[a-zA-Z0-9@.-]{4,32}$");
    return re.match(name).hasMatch();
}
