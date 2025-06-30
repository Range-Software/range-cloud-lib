#include "rcl_cloud_process_result.h"

void RCloudProcessResult::_init(const RCloudProcessResult *pProcessResult)
{
    if (pProcessResult)
    {
        this->errorCode = pProcessResult->errorCode;
        this->outputBuffer = pProcessResult->outputBuffer;
        this->errorBuffer = pProcessResult->errorBuffer;
        this->processRequest = pProcessResult->processRequest;
    }
}

RCloudProcessResult::RCloudProcessResult()
{
    this->_init();
}

RCloudProcessResult::RCloudProcessResult(const RCloudProcessResult &processResult)
{
    this->_init(&processResult);
}

RCloudProcessResult::~RCloudProcessResult()
{

}

RCloudProcessResult &RCloudProcessResult::operator=(const RCloudProcessResult &processResult)
{
    this->_init(&processResult);
    return (*this);
}

const RError::Type &RCloudProcessResult::getErrorType() const
{
    return this->errorCode;
}

void RCloudProcessResult::setErrorType(const RError::Type &errorCode)
{
    this->errorCode = errorCode;
}

const QByteArray &RCloudProcessResult::getOutputBuffer() const
{
    return this->outputBuffer;
}

QByteArray &RCloudProcessResult::getOutputBuffer()
{
    return this->outputBuffer;
}

const QByteArray &RCloudProcessResult::getErrorBuffer() const
{
    return this->errorBuffer;
}

QByteArray &RCloudProcessResult::getErrorBuffer()
{
    return this->errorBuffer;
}

const RCloudProcessRequest &RCloudProcessResult::getProcessRequest() const
{
    return this->processRequest;
}

void RCloudProcessResult::setProcessRequest(const RCloudProcessRequest &processRequest)
{
    this->processRequest = processRequest;
}
