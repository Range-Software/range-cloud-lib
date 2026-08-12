#include "rcl_cloud_ai_query_request.h"

void RCloudAIQueryRequest::_init(const RCloudAIQueryRequest *pAIQueryRequest)
{
    if (pAIQueryRequest)
    {
        this->executor = pAIQueryRequest->executor;
        this->application = pAIQueryRequest->application;
        this->model = pAIQueryRequest->model;
        this->query = pAIQueryRequest->query;
    }
}

RCloudAIQueryRequest::RCloudAIQueryRequest()
{
    this->_init();
}

RCloudAIQueryRequest::RCloudAIQueryRequest(const RCloudAIQueryRequest &aiQueryRequest)
{
    this->_init(&aiQueryRequest);
}

RCloudAIQueryRequest::~RCloudAIQueryRequest()
{

}

RCloudAIQueryRequest &RCloudAIQueryRequest::operator=(const RCloudAIQueryRequest &aiQueryRequest)
{
    this->_init(&aiQueryRequest);
    return (*this);
}

const RUserInfo &RCloudAIQueryRequest::getExecutor() const
{
    return this->executor;
}

void RCloudAIQueryRequest::setExecutor(const RUserInfo &executor)
{
    this->executor = executor;
}

const QString &RCloudAIQueryRequest::getApplication() const
{
    return this->application;
}

void RCloudAIQueryRequest::setApplication(const QString &application)
{
    this->application = application;
}

const QString &RCloudAIQueryRequest::getModel() const
{
    return this->model;
}

void RCloudAIQueryRequest::setModel(const QString &model)
{
    this->model = model;
}

const RAIQuery &RCloudAIQueryRequest::getQuery() const
{
    return this->query;
}

void RCloudAIQueryRequest::setQuery(const RAIQuery &query)
{
    this->query = query;
}

RCloudAIQueryRequest RCloudAIQueryRequest::fromJson(const QJsonObject &json)
{
    RCloudAIQueryRequest request;

    if (const QJsonValue &v = json["application"]; v.isString())
    {
        request.application = v.toString();
    }

    if (const QJsonValue &v = json["model"]; v.isString())
    {
        request.model = v.toString();
    }

    if (const QJsonValue &v = json["query"]; v.isObject())
    {
        request.query = RAIQuery::fromJson(v.toObject());
    }

    return request;
}

QJsonObject RCloudAIQueryRequest::toJson() const
{
    QJsonObject json;
    json["application"] = this->application;
    json["model"] = this->model;
    json["query"] = this->query.toJson();

    return json;
}
