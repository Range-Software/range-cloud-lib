#ifndef RCL_CLOUD_PROCESS_RESPONSE_H
#define RCL_CLOUD_PROCESS_RESPONSE_H

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

#include "rcl_cloud_process_request.h"

class RCloudProcessResponse
{

    protected:

        //! Process response message.
        QString responseMessage;
        //! Process processRequest.
        RCloudProcessRequest processRequest;

    private:

        //! Internal initialization function.
        void _init(const RCloudProcessResponse *pProcessResponse = nullptr);

    public:

        //! Constructor.
        RCloudProcessResponse();

        //! Copy constructor.
        RCloudProcessResponse(const RCloudProcessResponse &processResponse);

        //! Destructor.
        ~RCloudProcessResponse();

        //! Assignment operator.
        RCloudProcessResponse &operator =(const RCloudProcessResponse &processResponse);

        //! Return const reference to process response message.
        const QString &getResponseMessage() const;

        //! Set new process response message.
        void setResponseMessage(const QString &responseMessage);

        //! Return const reference to process request.
        const RCloudProcessRequest &getProcessRequest() const;

        //! Set new process request.
        void setProcessRequest(const RCloudProcessRequest &processRequest);

        //! Create response object from Json.
        static RCloudProcessResponse fromJson(const QJsonObject &json);

        //! Create Json from response object.
        QJsonObject toJson() const;

};

#endif // RCL_CLOUD_PROCESS_RESPONSE_H
