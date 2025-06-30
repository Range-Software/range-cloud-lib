#include <QJsonArray>
#include <QRegularExpression>

#include "rcl_access_owner.h"

void RAccessOwner::_init(const RAccessOwner *pAccessOwner)
{
    if (pAccessOwner)
    {
        this->user = pAccessOwner->user;
        this->group = pAccessOwner->group;
    }
}

RAccessOwner::RAccessOwner()
{
    this->_init();
}

RAccessOwner::RAccessOwner(const RAccessOwner &accessOwner)
{
    this->_init(&accessOwner);
}

RAccessOwner &RAccessOwner::operator=(const RAccessOwner &accessOwner)
{
    this->_init(&accessOwner);
    return (*this);
}

bool RAccessOwner::isValid() const
{
    return RAccessOwner::isUserValid(this->user) && RAccessOwner::isGroupValid(this->group);
}

const QString &RAccessOwner::getUser() const
{
    return this->user;
}

void RAccessOwner::setUser(const QString &user)
{
    this->user = user;
}

const QString &RAccessOwner::getGroup() const
{
    return this->group;
}

void RAccessOwner::setGroup(const QString &group)
{
    this->group = group;
}

RAccessOwner RAccessOwner::fromString(const QString &line, const char delimiter)
{
    RAccessOwner accessOwner;

    QStringList fields = line.split(delimiter);
    int index = 0;
    if (fields.count() > index) accessOwner.user = fields.at(index++);
    if (fields.count() > index) accessOwner.group = fields.at(index++);

    return accessOwner;
}

QString RAccessOwner::toString(const char delimiter) const
{
    QString infoString;

    infoString = this->getUser() + delimiter +
                 this->getGroup();

    return infoString;
}

RAccessOwner RAccessOwner::fromJson(const QJsonObject &json)
{
    RAccessOwner accessOwner;

    if (const QJsonValue &v = json["user"]; v.isString())
    {
        accessOwner.user = v.toString();
    }
    if (const QJsonValue &v = json["group"]; v.isString())
    {
        accessOwner.group = v.toString();
    }

    return accessOwner;
}

QJsonObject RAccessOwner::toJson() const
{
    QJsonObject json;

    json["user"] = this->user;
    json["group"] = this->group;

    return json;
}

bool RAccessOwner::isUserValid(const QString &user)
{
    static QRegularExpression re("^[a-zA-Z0-9@_.-]{1,256}$");
    return re.match(user).hasMatch();
}

bool RAccessOwner::isGroupValid(const QString &group)
{
    static QRegularExpression re("^[a-zA-Z0-9@_.-]{1,256}$");
    return re.match(group).hasMatch();
}
