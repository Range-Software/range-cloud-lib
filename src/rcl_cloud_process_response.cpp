#include <QJsonArray>

#include "rcl_cloud_process_response.h"

void RCloudProcessResponse::_init(const RCloudProcessResponse *pProcessResponse)
{
    if (pProcessResponse)
    {
        this->responseMessage = pProcessResponse->responseMessage;
        this->processRequest = pProcessResponse->processRequest;
    }
}

RCloudProcessResponse::RCloudProcessResponse()
{
    this->_init();
}

RCloudProcessResponse::RCloudProcessResponse(const RCloudProcessResponse &processResponse)
{
    this->_init(&processResponse);
}

RCloudProcessResponse::~RCloudProcessResponse()
{

}

RCloudProcessResponse &RCloudProcessResponse::operator=(const RCloudProcessResponse &processResponse)
{
    this->_init(&processResponse);
    return (*this);
}

const QString &RCloudProcessResponse::getResponseMessage() const
{
    return responseMessage;
}

void RCloudProcessResponse::setResponseMessage(const QString &responseMessage)
{
    this->responseMessage = responseMessage;
}

const RCloudProcessRequest &RCloudProcessResponse::getProcessRequest() const
{
    return this->processRequest;
}

void RCloudProcessResponse::setProcessRequest(const RCloudProcessRequest &processRequest)
{
    this->processRequest = processRequest;
}

RCloudProcessResponse RCloudProcessResponse::fromJson(const QJsonObject &json)
{
    RCloudProcessResponse response;

    if (const QJsonValue &v = json["response"]; v.isString())
    {
        response.responseMessage = v.toString();
    }

    if (const QJsonValue &v = json["request"]; v.isObject())
    {
        response.processRequest = RCloudProcessRequest::fromJson(v.toObject());
    }

    return response;
}

QJsonObject RCloudProcessResponse::toJson() const
{
    QJsonObject json;
    json["response"] = this->responseMessage;
    json["request"] = this->processRequest.toJson();

    return json;
}
