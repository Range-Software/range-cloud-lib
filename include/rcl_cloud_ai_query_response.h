#ifndef RCL_CLOUD_AI_QUERY_RESPONSE_H
#define RCL_CLOUD_AI_QUERY_RESPONSE_H

#include <QJsonObject>
#include <QString>
#include <QUuid>

#include "rcl_cloud_ai_query_request.h"

class RCloudAIQueryResponse
{

    public:

        //! Query processing status.
        enum Status
        {
            Unknown = 0,
            Pending,
            Completed
        };

    protected:

        //! Query request id (used to fetch the result of a pending query).
        QUuid id;
        //! Query processing status.
        Status status;
        //! AI query response message.
        QString responseMessage;
        //! Original AI query request.
        RCloudAIQueryRequest aiQueryRequest;

    private:

        //! Internal initialization function.
        void _init(const RCloudAIQueryResponse *pAIQueryResponse = nullptr);

    public:

        //! Constructor.
        RCloudAIQueryResponse();

        //! Copy constructor.
        RCloudAIQueryResponse(const RCloudAIQueryResponse &aiQueryResponse);

        //! Destructor.
        ~RCloudAIQueryResponse();

        //! Assignment operator.
        RCloudAIQueryResponse &operator =(const RCloudAIQueryResponse &aiQueryResponse);

        //! Return const reference to query request id.
        const QUuid &getId() const;

        //! Set new query request id.
        void setId(const QUuid &id);

        //! Return query processing status.
        Status getStatus() const;

        //! Set new query processing status.
        void setStatus(Status status);

        //! Return const reference to AI query response message.
        const QString &getResponseMessage() const;

        //! Set new AI query response message.
        void setResponseMessage(const QString &responseMessage);

        //! Return const reference to AI query request.
        const RCloudAIQueryRequest &getAIQueryRequest() const;

        //! Set new AI query request.
        void setAIQueryRequest(const RCloudAIQueryRequest &aiQueryRequest);

        //! Create response object from Json.
        static RCloudAIQueryResponse fromJson(const QJsonObject &json);

        //! Create Json from response object.
        QJsonObject toJson() const;

        //! Convert status to string.
        static QString statusToString(Status status);

        //! Convert string to status.
        static Status statusFromString(const QString &statusString);

};

#endif // RCL_CLOUD_AI_QUERY_RESPONSE_H
