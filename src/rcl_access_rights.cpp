#include <QJsonArray>

#include <rbl_logger.h>

#include "rcl_access_rights.h"

void RAccessRights::_init(const RAccessRights *pAccessRights)
{
    if (pAccessRights)
    {
        this->owner = pAccessRights->owner;
        this->mode = pAccessRights->mode;
    }
}

RAccessRights::RAccessRights()
{
    this->_init();
}

RAccessRights::RAccessRights(const RAccessRights &accessRights)
{
    this->_init(&accessRights);
}

RAccessRights &RAccessRights::operator=(const RAccessRights &accessRights)
{
    this->_init(&accessRights);
    return (*this);
}

bool RAccessRights::isValid() const
{
    return this->owner.isValid() && this->mode.isValid();
}

const RAccessOwner &RAccessRights::getOwner() const
{
    return this->owner;
}

void RAccessRights::setOwner(const RAccessOwner &owner)
{
    this->owner = owner;
}

const RAccessMode &RAccessRights::getMode() const
{
    return this->mode;
}

void RAccessRights::setMode(const RAccessMode &mode)
{
    this->mode = mode;
}

bool RAccessRights::isUserAuthorized(const RUserInfo &userInfo, RAccessMode::Mode mode) const
{
    return (userInfo.getName() == RUserInfo::rootUser ||
            (this->mode.getOtherModeMask() & mode) ||
            (this->mode.getGroupModeMask() & mode && userInfo.getGroupNames().contains(this->owner.getGroup())) ||
            (this->mode.getUserModeMask()  & mode && userInfo.getName() == this->owner.getUser()));
}

QString RAccessRights::toString(const char delimiter) const
{
    QString infoString;

    infoString = this->mode.toString(delimiter) + delimiter +
                 this->owner.toString(delimiter);

    return infoString;
}

RAccessRights RAccessRights::fromString(const QString &line, const char delimiter)
{
    QStringList fields = line.split(delimiter);

    int expectedNuberOfFields = 5;
    if (fields.size() != expectedNuberOfFields)
    {
        RLogger::warning("Invalid line. Has \'%d\' fields but expected are \'%d\'\n",fields.size(),expectedNuberOfFields);
    }

    int index = 0;

    RAccessMode accessMode;
    if (fields.size() > index) accessMode.setUserModeMask(fields.at(index++).toInt());
    if (fields.size() > index) accessMode.setGroupModeMask(fields.at(index++).toInt());
    if (fields.size() > index) accessMode.setOtherModeMask(fields.at(index++).toInt());

    RAccessOwner accessOwner;
    if (fields.size() > index) accessOwner.setUser(fields.at(index++));
    if (fields.size() > index) accessOwner.setGroup(fields.at(index++));

    RAccessRights accessRights;
    accessRights.setOwner(accessOwner);
    accessRights.setMode(accessMode);

    return accessRights;
}

RAccessRights RAccessRights::fromJson(const QJsonObject &json)
{
    RAccessRights accessRights;

    if (const QJsonValue &v = json["owner"]; v.isObject())
    {
        accessRights.owner = RAccessOwner::fromJson(v.toObject());
    }

    if (const QJsonValue &v = json["mode"]; v.isObject())
    {
        accessRights.mode = RAccessMode::fromJson(v.toObject());
    }

    return accessRights;
}

QJsonObject RAccessRights::toJson() const
{
    QJsonObject json;

    json["owner"] = this->owner.toJson();
    json["mode"] = this->mode.toJson();

    return json;
}
