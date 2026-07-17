#include "rcl_ai_query.h"

void RAIQuery::_init(const RAIQuery *pAIQuery)
{
    if (pAIQuery)
    {
        this->question = pAIQuery->question;
        this->fileId = pAIQuery->fileId;
        this->context = pAIQuery->context;
        this->language = pAIQuery->language;
    }
}

RAIQuery::RAIQuery()
{
    this->_init();
}

RAIQuery::RAIQuery(const RAIQuery &aiQuery)
{
    this->_init(&aiQuery);
}

RAIQuery::~RAIQuery()
{

}

RAIQuery &RAIQuery::operator =(const RAIQuery &aiQuery)
{
    this->_init(&aiQuery);
    return (*this);
}

const QString &RAIQuery::getQuestion() const
{
    return this->question;
}

void RAIQuery::setQuestion(const QString &question)
{
    this->question = question;
}

const QUuid &RAIQuery::getFileId() const
{
    return this->fileId;
}

void RAIQuery::setFileId(const QUuid &fileId)
{
    this->fileId = fileId;
}

const QString &RAIQuery::getContext() const
{
    return this->context;
}

void RAIQuery::setContext(const QString &context)
{
    this->context = context;
}

const QString &RAIQuery::getLanguage() const
{
    return this->language;
}

void RAIQuery::setLanguage(const QString &language)
{
    this->language = language;
}

bool RAIQuery::isEmpty() const
{
    return this->question.trimmed().isEmpty();
}

QString RAIQuery::buildPrompt() const
{
    QString prompt;
    if (!this->context.isEmpty())
    {
        prompt += QString("Context:\n%1\n\n").arg(this->context);
    }
    if (!this->language.isEmpty())
    {
        prompt += QString("Answer in language: %1\n\n").arg(this->language);
    }
    prompt += this->question;
    return prompt;
}

RAIQuery RAIQuery::fromJson(const QJsonObject &json)
{
    RAIQuery aiQuery;

    if (const QJsonValue &v = json["question"]; v.isString())
    {
        aiQuery.question = v.toString();
    }

    if (const QJsonValue &v = json["fileId"]; v.isString())
    {
        aiQuery.fileId = QUuid(v.toString());
    }

    if (const QJsonValue &v = json["context"]; v.isString())
    {
        aiQuery.context = v.toString();
    }

    if (const QJsonValue &v = json["language"]; v.isString())
    {
        aiQuery.language = v.toString();
    }

    return aiQuery;
}

QJsonObject RAIQuery::toJson() const
{
    QJsonObject json;

    json["question"] = this->question;
    json["fileId"] = this->fileId.toString(QUuid::WithoutBraces);
    json["context"] = this->context;
    json["language"] = this->language;

    return json;
}
