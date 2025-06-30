#include "rcl_cloud_action.h"
#include "rcl_http_message.h"
#include <rbl_logger.h>

void RHttpMessage::_init(const RHttpMessage *pHttpMessage)
{
    if (pHttpMessage)
    {
        this->method = pHttpMessage->method;
        this->urlQuery = pHttpMessage->urlQuery;
        this->responseHeaders = pHttpMessage->responseHeaders;
    }
}

RHttpMessage::RHttpMessage()
    : method(QHttpServerRequest::Method::Get)
{
    this->_init();
}

RHttpMessage::RHttpMessage(const RHttpMessage &httpMessage)
    : RNetworkMessage(httpMessage)
{
    this->_init(&httpMessage);
}

RHttpMessage::RHttpMessage(const RNetworkMessage &networkMessage)
    : RNetworkMessage(networkMessage)
{
    this->_init();

    if (this->properties.value(RCloudAction::Action::key) == RCloudAction::Action::FileDownload::key)
    {
        QString filePath = this->properties.value(RCloudAction::Resource::Name::key);
        this->responseHeaders.append(QHttpHeaders::WellKnownHeader::ContentDisposition,
                                     QString("attachment; filename=\"%1\"").arg(filePath));
    }
}

RHttpMessage::RHttpMessage(const RCloudAction &action)
    : RNetworkMessage(action)
{
    this->method = RHttpMessage::findMethodForAction(action.getAction());
    this->to = QString("%1/").arg(action.getAction());

    this->urlQuery.clear();

    for (QMap<QString, QString>::const_iterator iter=this->properties.cbegin();iter!=this->properties.cend();++iter)
    {
        if (!iter.value().isEmpty())
        {
            this->urlQuery.addQueryItem(iter.key(),iter.value());
        }
    }

    if (!this->owner.isEmpty())
    {
        this->urlQuery.addQueryItem(RCloudAction::Auth::User::key,this->owner);
    }

    if (!this->authToken.isEmpty())
    {
        this->urlQuery.addQueryItem(RCloudAction::Auth::Token::key,this->authToken);
    }
}

RHttpMessage::~RHttpMessage()
{

}

RHttpMessage &RHttpMessage::operator =(const RHttpMessage &httpMessage)
{
    this->RNetworkMessage::_init(&httpMessage);
    this->_init(&httpMessage);
    return (*this);
}

QHttpServerRequest::Method RHttpMessage::getMethod() const
{
    return this->method;
}

void RHttpMessage::setMethod(QHttpServerRequest::Method method)
{
    this->method = method;
}

const QUrlQuery &RHttpMessage::getUrlQuery() const
{
    return this->urlQuery;
}

void RHttpMessage::setUrlQuery(const QUrlQuery &urlQuery)
{
    this->urlQuery = urlQuery;
}

const QHttpHeaders &RHttpMessage::getResponseHeaders() const
{
    return this->responseHeaders;
}

void RHttpMessage::setResponseHeaders(const QHttpHeaders &responseHeaders)
{
    this->responseHeaders = responseHeaders;
}

void RHttpMessage::print(bool printBody) const
{
    RLogger::indent();
    RLogger::info("owner: \"%s\"\n",this->owner.toUtf8().constData());
    RLogger::info("handler-id: \"%s\"\n",this->handlerId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("message-id: \"%s\"\n",this->messageId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("correlation-id: \"%s\"\n",this->correlationId.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("to: \"%s\"\n",this->to.toUtf8().constData());
    RLogger::info("reply-to: \"%s\"\n",this->replyTo.toUtf8().constData());
    RLogger::info("http-method: \"%s\"\n",RHttpMessage::httpMethodToString(this->method).toUtf8().constData());
    RLogger::info("url-query: \"%s\"\n",this->urlQuery.toString().toUtf8().constData());
    RLogger::info("properties:\n");
    for (QMap<QString, QString>::const_iterator iter=this->properties.cbegin();iter!=this->properties.cend();++iter)
    {
        RLogger::info("  \"%s\": \"%s\"\n", iter.key().toUtf8().constData(), iter.value().toUtf8().constData());
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

QHttpServerRequest::Method RHttpMessage::findMethodForAction(const QString &actionKey)
{
    if (actionKey == RCloudAction::Action::FileUpload::key)
    {
        return QHttpServerRequest::Method::Put;
    }
    else if (actionKey == RCloudAction::Action::Test::key ||
             actionKey == RCloudAction::Action::FileUpdate::key ||
             actionKey == RCloudAction::Action::FileUpdateAccessOwner::key ||
             actionKey == RCloudAction::Action::FileUpdateAccessMode::key ||
             actionKey == RCloudAction::Action::FileUpdateVersion::key ||
             actionKey == RCloudAction::Action::FileUpdateTags::key ||
             actionKey == RCloudAction::Action::Process::key ||
             actionKey == RCloudAction::Action::UserUpdate::key ||
             actionKey == RCloudAction::Action::ActionUpdateAccessOwner::key ||
             actionKey == RCloudAction::Action::ActionUpdateAccessMode::key ||
             actionKey == RCloudAction::Action::ProcessUpdateAccessOwner::key ||
             actionKey == RCloudAction::Action::ProcessUpdateAccessMode::key ||
             actionKey == RCloudAction::Action::SubmitReport::key)
    {
        return QHttpServerRequest::Method::Post;
    }
    else
    {
        return QHttpServerRequest::Method::Get;
    }
}

QString RHttpMessage::httpMethodToString(QHttpServerRequest::Method method)
{
    if (method == QHttpServerRequest::Method::Unknown) return QString("An unknown method.");
    else if (method == QHttpServerRequest::Method::Get) return QString("HTTP GET method.");
    else if (method == QHttpServerRequest::Method::Put) return QString("HTTP PUT method.");
    else if (method == QHttpServerRequest::Method::Delete) return QString("HTTP DELETE method.");
    else if (method == QHttpServerRequest::Method::Post) return QString("HTTP POST method.");
    else if (method == QHttpServerRequest::Method::Head) return QString("HTTP HEAD method.");
    else if (method == QHttpServerRequest::Method::Options) return QString("HTTP OPTIONS method.");
    else if (method == QHttpServerRequest::Method::Patch) return QString("HTTP PATCH method (RFC 5789).");
    else if (method == QHttpServerRequest::Method::Connect) return QString("HTTP CONNECT method.");
    else if (method == QHttpServerRequest::Method::Trace) return QString("HTTP TRACE method.");
    else if (method == QHttpServerRequest::Method::AnyKnown) return QString("Combination of all known methods.");
    else  return QString("An unknown method.");
}

QHttpServerResponse::StatusCode RHttpMessage::errorTypeToStatusCode(RError::Type errorType)
{
    switch (errorType)
    {
        case RError::None:
            return QHttpServerResponse::StatusCode::Ok;
        case RError::InvalidInput:
            return QHttpServerResponse::StatusCode::BadRequest;
        case RError::Unauthorized:
            return QHttpServerResponse::StatusCode::Unauthorized;
        case RError::OpenFile:
        case RError::ReadFile:
        case RError::WriteFile:
        case RError::RemoveFile:
        case RError::RenameFile:
        case RError::InvalidFileName:
        case RError::InvalidFileFormat:
        case RError::OpenDir:
        case RError::ReadDir:
        case RError::Application:
        case RError::ChildProcess:
        case RError::Unknown:
        default:
            return QHttpServerResponse::StatusCode::InternalServerError;
    }
}

RError::Type RHttpMessage::statusCodeToErrorType(QHttpServerResponse::StatusCode statusCode)
{
    switch (statusCode)
    {
        case QHttpServerResponse::StatusCode::Ok:
            return RError::None;
        case QHttpServerResponse::StatusCode::BadRequest:
            return RError::InvalidInput;
        case QHttpServerResponse::StatusCode::Unauthorized:
            return RError::Unauthorized;
        default:
            return RError::Unknown;
    }
}
