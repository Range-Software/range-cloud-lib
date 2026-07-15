#ifndef RCL_CLOUD_AI_QUERY_RESPONSE_H
#define RCL_CLOUD_AI_QUERY_RESPONSE_H

#include <QJsonObject>
#include <QString>

#include "rcl_cloud_ai_query_request.h"

class RCloudAIQueryResponse
{

    protected:

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

};

#endif // RCL_CLOUD_AI_QUERY_RESPONSE_H
