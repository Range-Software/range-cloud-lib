#include "rcl_cloud_ai_query_response.h"

void RCloudAIQueryResponse::_init(const RCloudAIQueryResponse *pAIQueryResponse)
{
    this->status = Unknown;
    if (pAIQueryResponse)
    {
        this->id = pAIQueryResponse->id;
        this->status = pAIQueryResponse->status;
        this->responseMessage = pAIQueryResponse->responseMessage;
        this->aiQueryRequest = pAIQueryResponse->aiQueryRequest;
    }
}

RCloudAIQueryResponse::RCloudAIQueryResponse()
{
    this->_init();
}

RCloudAIQueryResponse::RCloudAIQueryResponse(const RCloudAIQueryResponse &aiQueryResponse)
{
    this->_init(&aiQueryResponse);
}

RCloudAIQueryResponse::~RCloudAIQueryResponse()
{

}

RCloudAIQueryResponse &RCloudAIQueryResponse::operator=(const RCloudAIQueryResponse &aiQueryResponse)
{
    this->_init(&aiQueryResponse);
    return (*this);
}

const QUuid &RCloudAIQueryResponse::getId() const
{
    return this->id;
}

void RCloudAIQueryResponse::setId(const QUuid &id)
{
    this->id = id;
}

RCloudAIQueryResponse::Status RCloudAIQueryResponse::getStatus() const
{
    return this->status;
}

void RCloudAIQueryResponse::setStatus(Status status)
{
    this->status = status;
}

const QString &RCloudAIQueryResponse::getResponseMessage() const
{
    return this->responseMessage;
}

void RCloudAIQueryResponse::setResponseMessage(const QString &responseMessage)
{
    this->responseMessage = responseMessage;
}

const RCloudAIQueryRequest &RCloudAIQueryResponse::getAIQueryRequest() const
{
    return this->aiQueryRequest;
}

void RCloudAIQueryResponse::setAIQueryRequest(const RCloudAIQueryRequest &aiQueryRequest)
{
    this->aiQueryRequest = aiQueryRequest;
}

RCloudAIQueryResponse RCloudAIQueryResponse::fromJson(const QJsonObject &json)
{
    RCloudAIQueryResponse response;

    if (const QJsonValue &v = json["id"]; v.isString())
    {
        response.id = QUuid(v.toString());
    }

    if (const QJsonValue &v = json["status"]; v.isString())
    {
        response.status = RCloudAIQueryResponse::statusFromString(v.toString());
    }

    if (const QJsonValue &v = json["response"]; v.isString())
    {
        response.responseMessage = v.toString();
    }

    if (const QJsonValue &v = json["request"]; v.isObject())
    {
        response.aiQueryRequest = RCloudAIQueryRequest::fromJson(v.toObject());
    }

    return response;
}

QJsonObject RCloudAIQueryResponse::toJson() const
{
    QJsonObject json;
    json["id"] = this->id.toString(QUuid::WithoutBraces);
    json["status"] = RCloudAIQueryResponse::statusToString(this->status);
    json["response"] = this->responseMessage;
    json["request"] = this->aiQueryRequest.toJson();

    return json;
}

QString RCloudAIQueryResponse::statusToString(Status status)
{
    switch (status)
    {
        case Pending:   return QStringLiteral("pending");
        case Completed: return QStringLiteral("completed");
        default:        return QStringLiteral("unknown");
    }
}

RCloudAIQueryResponse::Status RCloudAIQueryResponse::statusFromString(const QString &statusString)
{
    if (statusString == QStringLiteral("pending"))
    {
        return Pending;
    }
    if (statusString == QStringLiteral("completed"))
    {
        return Completed;
    }
    return Unknown;
}
