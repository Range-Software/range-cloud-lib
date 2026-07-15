#include "rcl_cloud_ai_query_response.h"

void RCloudAIQueryResponse::_init(const RCloudAIQueryResponse *pAIQueryResponse)
{
    if (pAIQueryResponse)
    {
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
    json["response"] = this->responseMessage;
    json["request"] = this->aiQueryRequest.toJson();

    return json;
}
