#ifndef RCL_HTTP_MESSAGE_H
#define RCL_HTTP_MESSAGE_H

#include <QUrlQuery>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

#include "rcl_network_message.h"

class RHttpMessage : public RNetworkMessage
{

    public:

    protected:

        //! Internal initialization function.
        void _init(const RHttpMessage *pHttpMessage = nullptr);

    protected:

        //! Http method.
        QHttpServerRequest::Method method;
        //! Http url query.
        QUrlQuery urlQuery;
        //! Response header.
        QHttpHeaders responseHeaders;

    public:

        //! Constructor.
        RHttpMessage();

        //! Copy constructor.
        RHttpMessage(const RHttpMessage &httpMessage);

        //! Copy constructor.
        RHttpMessage(const RNetworkMessage &networkMessage);

        //! Copy constructor.
        RHttpMessage(const RCloudAction &action);

        //! Destructor.
        ~RHttpMessage();

        //! Assignment operator (message).
        RHttpMessage &operator =(const RHttpMessage &httpMessage);

        //! Return HTTP method.
        QHttpServerRequest::Method getMethod() const;

        //! Set HTTP method.
        void setMethod(QHttpServerRequest::Method method);

        //! Return HTTP url.
        const QUrlQuery &getUrlQuery() const;

        //! Set HTTP url.
        void setUrlQuery(const QUrlQuery &urlQuery);

        //! Return response headers.
        const QHttpHeaders &getResponseHeaders() const;

        //! Set response headers.
        void setResponseHeaders(const QHttpHeaders &responseHeaders);

        //! Print message to standard output.
        void print(bool printBody = false) const override;

        //! Find what HTTP method to use for given action.
        static QHttpServerRequest::Method findMethodForAction(const QString &actionKey);

        //! Convert HTTP method to string.
        static QString httpMethodToString(QHttpServerRequest::Method method);

        //! Convert Error type to HTTP status code.
        static QHttpServerResponse::StatusCode errorTypeToStatusCode(RError::Type errorType);

        //! Convert HTTP status code to Error type.
        static RError::Type statusCodeToErrorType(QHttpServerResponse::StatusCode statusCode);

};

#endif // RCL_HTTP_MESSAGE_H
