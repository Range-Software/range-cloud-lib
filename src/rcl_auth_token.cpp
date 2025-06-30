#include <QDateTime>

#include "rcl_auth_token.h"

void RAuthToken::_init(const RAuthToken *pAuthInfo)
{
    if (pAuthInfo)
    {
        this->id = pAuthInfo->id;
        this->content = pAuthInfo->content;
        this->resourceName = pAuthInfo->resourceName;
        this->validityDate = pAuthInfo->validityDate;
    }
}

RAuthToken::RAuthToken()
    : id{QUuid::createUuid()}
{
    this->_init();
}

RAuthToken::RAuthToken(const RAuthToken &authInfo)
{
    this->_init(&authInfo);
}

RAuthToken::~RAuthToken()
{

}

RAuthToken &RAuthToken::operator =(const RAuthToken &authInfo)
{
    this->_init(&authInfo);
    return (*this);
}

const QUuid &RAuthToken::getId() const
{
    return this->id;
}

const QString &RAuthToken::getContent() const
{
    return this->content;
}

void RAuthToken::setContent(const QString &content)
{
    this->content = content;
}

const QString &RAuthToken::getResourceName() const
{
    return this->resourceName;
}

void RAuthToken::setResourceName(const QString &resourceName)
{
    this->resourceName = resourceName;
}

qint64 RAuthToken::getValidityDate() const
{
    return this->validityDate;
}

void RAuthToken::setValidityDate(qint64 validityDate)
{
    this->validityDate = validityDate;
}

bool RAuthToken::isNull() const
{
    return (this->resourceName.isEmpty() || this->content.isEmpty());
}

RAuthToken RAuthToken::fromJson(const QJsonObject &json)
{
    RAuthToken authToken;

    if (const QJsonValue &v = json["id"]; v.isString())
    {
        authToken.id = QUuid(v.toString());
    }
    if (const QJsonValue &v = json["content"]; v.isString())
    {
        authToken.content = v.toString();
    }
    if (const QJsonValue &v = json["resource"]; v.isString())
    {
        authToken.resourceName = v.toString();
    }
    if (const QJsonValue &v = json["validity"]; v.isString())
    {
        authToken.validityDate = v.toString().toLongLong();
    }

    return authToken;
}

QJsonObject RAuthToken::toJson() const
{
    QJsonObject json;

    json["id"] = this->id.toString(QUuid::WithoutBraces);
    json["content"] = this->content;
    json["resource"] = this->resourceName;
    json["validity"] = QString::number(this->validityDate);

    return json;
}

QString RAuthToken::generateTokenContent()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

quint64 RAuthToken::validityMonthsFromNow(uint months)
{
    return QDateTime::currentDateTime().addMonths(months).toSecsSinceEpoch();
}
