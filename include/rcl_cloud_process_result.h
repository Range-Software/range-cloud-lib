#ifndef RCL_CLOUD_PROCESS_RESULT_H
#define RCL_CLOUD_PROCESS_RESULT_H

#include <QByteArray>

#include <rbl_error.h>

#include "rcl_cloud_process_request.h"

class RCloudProcessResult
{

    protected:

        //! Error code.
        RError::Type errorCode;
        //! Output buffer.
        QByteArray outputBuffer;
        //! Error buffer.
        QByteArray errorBuffer;
        //! Process request.
        RCloudProcessRequest processRequest;

    private:

        //! Internal initialization function.
        void _init(const RCloudProcessResult *pProcessResult = nullptr);

    public:

        //! Constructor.
        RCloudProcessResult();

        //! Copy constructor.
        RCloudProcessResult(const RCloudProcessResult &processResult);

        //! Destructor.
        ~RCloudProcessResult();

        //! Assignment operator.
        RCloudProcessResult &operator =(const RCloudProcessResult &processResult);

        //! Return const reference to error.
        const RError::Type &getErrorType() const;

        //! Set new error.
        void setErrorType(const RError::Type &errorCode);

        //! Return const reference to output buffer.
        const QByteArray &getOutputBuffer() const;

        //! Return reference to output buffer.
        QByteArray &getOutputBuffer();

        //! Return const reference to error buffer.
        const QByteArray &getErrorBuffer() const;

        //! Return reference to error buffer.
        QByteArray &getErrorBuffer();

        //! Return const reference to request.
        const RCloudProcessRequest &getProcessRequest() const;

        //! Set new process request.
        void setProcessRequest(const RCloudProcessRequest &processRequest);

};

#endif // RCL_CLOUD_PROCESS_RESULT_H
