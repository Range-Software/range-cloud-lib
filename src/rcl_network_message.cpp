#include "rcl_network_message.h"
#include "rcl_cloud_action.h"
#include <rbl_logger.h>

void RNetworkMessage::_init(const RNetworkMessage *pNetworkMessage)
{
    if (pNetworkMessage)
    {
        this->owner = pNetworkMessage->owner;
        this->authToken = pNetworkMessage->authToken;
        this->handlerId = pNetworkMessage->handlerId;
        this->messageId = pNetworkMessage->messageId;
        this->correlationId = pNetworkMessage->correlationId;
        this->from = pNetworkMessage->from;
        this->to = pNetworkMessage->to;
        this->replyTo = pNetworkMessage->replyTo;
        this->properties = pNetworkMessage->properties;
        this->body = pNetworkMessage->body;
        this->errorType = pNetworkMessage->errorType;
    }
}

RNetworkMessage::RNetworkMessage()
    : messageId(QUuid::createUuid())
    , errorType(RError::None)
{
    this->_init();

}

RNetworkMessage::RNetworkMessage(const RNetworkMessage &networkMessage)
{
    this->_init(&networkMessage);
}

RNetworkMessage::RNetworkMessage(const RCloudAction &action)
    : messageId(QUuid::createUuid())
{
    this->properties.clear();
    if (!action.getAction().isEmpty())
    {
        this->properties.insert(RCloudAction::Action::key,action.getAction());
    }
    if (!action.getExecutor().isEmpty())
    {
        this->owner = action.getExecutor();
    }
    if (!action.getAuthToken().isEmpty())
    {
        this->authToken = action.getAuthToken();
    }
    if (!action.getResourceName().isEmpty())
    {
        this->properties.insert(RCloudAction::Resource::Name::key,action.getResourceName());
    }
    if (!action.getResourceId().isNull())
    {
        this->properties.insert(RCloudAction::Resource::Id::key,action.getResourceId().toString(QUuid::WithoutBraces));
    }
    this->body = action.getData();
    this->errorType = action.getErrorType();
}

RNetworkMessage::~RNetworkMessage()
{

}

RNetworkMessage &RNetworkMessage::operator =(const RNetworkMessage &networkMessage)
{
    this->_init(&networkMessage);
    return (*this);
}

RNetworkMessage RNetworkMessage::toReply(const RCloudAction &action) const
{
    RNetworkMessage replyMessage(*this);

    replyMessage.to = this->replyTo;
    replyMessage.replyTo = QString();
    QString actionString = this->getProperties().value(RCloudAction::Action::key,action.getAction());
    replyMessage.properties.clear();
    replyMessage.properties.insert(RCloudAction::Action::key,actionString);

    if (!action.getResourceName().isEmpty())
    {
        replyMessage.properties.insert(RCloudAction::Resource::Name::key,action.getResourceName());
    }
    if (!action.getResourceId().isNull())
    {
        replyMessage.properties.insert(RCloudAction::Resource::Id::key,action.getResourceId().toString(QUuid::WithoutBraces));
    }
    replyMessage.body = action.getData();
    replyMessage.errorType = action.getErrorType();

    return replyMessage;
}

RCloudAction RNetworkMessage::toAction() const
{
    if (!this->properties.contains(RCloudAction::Action::key))
    {
        RLogger::warning("Message is missing propperty \"%s\"\n",RCloudAction::Action::key.toUtf8().constData());
    }
    // if (!this->properties.contains(RCloudAction::Name::key))
    // {
    //     RLogger::warning("Message is missing propperty \"%s\"\n",RCloudAction::Name::key.toUtf8().constData());
    // }
    // if (!this->properties.contains(RCloudAction::Id::key))
    // {
    //     RLogger::warning("Message is missing propperty \"%s\"\n",RCloudAction::Id::key.toUtf8().constData());
    // }

    RCloudAction action(QUuid::createUuid(),
                   this->owner,
                   this->authToken,
                   this->properties.value(RCloudAction::Action::key,QString()),
                   this->properties.value(RCloudAction::Resource::Name::key,QString()),
                   QUuid(this->properties.value(RCloudAction::Resource::Id::key,QString())),
                   this->body);
    action.setErrorType(this->errorType);

    return action;
}

const QString &RNetworkMessage::getOwner() const
{
    return this->owner;
}

void RNetworkMessage::setOwner(const QString &owner)
{
    this->owner = owner;
}

const QString &RNetworkMessage::getAuthToken() const
{
    return this->authToken;
}

void RNetworkMessage::setAuthToken(const QString &authToken)
{
    this->authToken = authToken;
}

const QUuid &RNetworkMessage::getHandlerId() const
{
    return this->handlerId;
}

void RNetworkMessage::setHandlerId(const QUuid &handlerId)
{
    this->handlerId = handlerId;
}

const QUuid &RNetworkMessage::getMessageId() const
{
    return this->messageId;
}

void RNetworkMessage::setMessageId(const QUuid &messageId)
{
    this->messageId = messageId;
}

const QUuid &RNetworkMessage::getCorrelationId() const
{
    return this->correlationId;
}

void RNetworkMessage::setCorrelationId(const QUuid &correlationId)
{
    this->correlationId = correlationId;
}

const QString &RNetworkMessage::getFrom() const
{
    return this->from;
}

void RNetworkMessage::setFrom(const QString &from)
{
    this->from = from;
}

const QString &RNetworkMessage::getTo() const
{
    return this->to;
}

void RNetworkMessage::setTo(const QString &to)
{
    this->to = to;
}

const QString &RNetworkMessage::getReplyTo() const
{
    return this->replyTo;
}

void RNetworkMessage::setReplyTo(const QString &replyTo)
{
    this->replyTo = replyTo;
}

const QMap<QString, QString> &RNetworkMessage::getProperties() const
{
    return this->properties;
}

void RNetworkMessage::setProperties(const QMap<QString, QString> &properties)
{
    this->properties = properties;
}

const QByteArray &RNetworkMessage::getBody() const
{
    return this->body;
}

void RNetworkMessage::setBody(const QByteArray &body)
{
    this->body = body;
}

RError::Type RNetworkMessage::getErrorType() const
{
    return this->errorType;
}

void RNetworkMessage::setErrorType(RError::Type errorType)
{
    this->errorType = errorType;
}

void RNetworkMessage::print(bool printBody) const
{
    RLogger::indent();
    RLogger::info("owner: \"%s\"\n",this->owner.toUtf8().constData());
    RLogger::info("auth-token: \"%s\"\n",this->authToken.toUtf8().constData());
    RLogger::info("handler-id: \"%s\"\n",this->handlerId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("message-id: \"%s\"\n",this->messageId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("correlation-id: \"%s\"\n",this->correlationId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("from: \"%s\"\n",this->from.toUtf8().constData());
    RLogger::info("to: \"%s\"\n",this->to.toUtf8().constData());
    RLogger::info("reply-to: \"%s\"\n",this->replyTo.toUtf8().constData());
    RLogger::info("properties:\n");

    for (auto iter = this->properties.cbegin(); iter != this->properties.cend(); ++iter)
    {
        RLogger::info("  \"%s\": \"%s\"\n", iter.key().toUtf8().constData(), this->properties.value(iter.key()).toUtf8().constData());
    }

    if (printBody)
    {
        RLogger::info("body: \"%s\"\n",this->body.constData());
    }
    else
    {
        RLogger::info("body: <%ld bytes>\n",this->body.size());
    }
    RLogger::info("error-type: \"%s\"\n",RError::getTypeMessage(this->errorType).toUtf8().constData());
    RLogger::unindent(false);
}

