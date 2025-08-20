#include <QJsonDocument>

#include <rbl_logger.h>
#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include "rcl_cloud_client.h"
#include "rcl_cloud_action.h"
#include "rcl_file_tools.h"
#include "rcl_cloud_process_response.h"
#include "rcl_cloud_tool_action.h"
#include "rcl_http_client.h"

RCloudClient::RCloudClient(RHttpClient::Type type, const RHttpClientSettings &httpClientSettings, QObject *parent)
    : QObject{parent}
    , blocking(true)
{
    R_LOG_TRACE_IN;
    this->httpClient = new RHttpClient(type,httpClientSettings,this);

    QObject::connect(this->httpClient,&RHttpClient::uploadProgress,this,&RCloudClient::onUploadProgress);
    QObject::connect(this->httpClient,&RHttpClient::downloadProgress,this,&RCloudClient::onDownloadProgress);
    R_LOG_TRACE_OUT;
}

void RCloudClient::setBlocking(bool blocking)
{
    R_LOG_TRACE_IN;
    this->blocking = blocking;
    R_LOG_TRACE_OUT;
}

RToolTask *RCloudClient::requestTest(const QString &responseMessage, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestTest(this->httpClient,responseMessage,authUser,authToken)));
}

RToolTask *RCloudClient::requestCsrProcess(const QByteArray &csrBase64, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    QMap<QString,QString> argumentValues;
    argumentValues.insert("csr-content-base64",csrBase64);

    RCloudProcessRequest processRequest;
    processRequest.setName("process-csr");
    processRequest.setArgumentValues(argumentValues);

    R_LOG_TRACE_RETURN(this->requestProcess(processRequest,authUser,authToken));
}

RToolTask *RCloudClient::requestReportProcess(const QByteArray &reportBase64, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    QMap<QString,QString> argumentValues;
    argumentValues.insert("report-content-base64",reportBase64);

    RCloudProcessRequest processRequest;
    processRequest.setName("process-report");
    processRequest.setArgumentValues(argumentValues);

    R_LOG_TRACE_RETURN(this->requestProcess(processRequest,authUser,authToken));
}

RToolTask *RCloudClient::requestProcess(const RCloudProcessRequest &processRequest, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcess(this->httpClient,processRequest,authUser,authToken)));
}

RToolTask *RCloudClient::requestListFiles(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListFiles(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpload(const QString &filePath, const QString &name, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpload(this->httpClient,filePath,name,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdate(const QString &filePath, const QString &name, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdate(this->httpClient,filePath,name,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateAccessOwner(const RAccessOwner &accessOwner, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateAccessOwner(this->httpClient,accessOwner,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateAccessMode(const RAccessMode &accessMode, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateAccessMode(this->httpClient,accessMode,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateVersion(const RVersion &version, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateVersion(this->httpClient,version,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateTags(const QStringList &tags, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateTags(this->httpClient,tags,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileDownload(const QString &filePath, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileDownload(this->httpClient,filePath,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileRemove(const QUuid &fileId, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileRemove(this->httpClient,fileId,authUser,authToken)));
}

RToolTask *RCloudClient::requestListUsers(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListUsers(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserAdd(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserAdd(this->httpClient,userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserUpdate(const QString &userName, const RUserInfo &userInfo, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserUpdate(this->httpClient,userName,userInfo,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserRemove(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserRemove(this->httpClient,userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestListUserTokens(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListUserTokens(this->httpClient,userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserTokenGenerate(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserTokenGenerate(this->httpClient,userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserTokenRemove(const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserTokenRemove(this->httpClient,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestListGroups(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListGroups(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestGroupAdd(const QString &groupName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestGroupAdd(this->httpClient,groupName,authUser,authToken)));
}

RToolTask *RCloudClient::requestGroupRemove(const QString &groupName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestGroupRemove(this->httpClient,groupName,authUser,authToken)));
}

RToolTask *RCloudClient::requestListActions(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListActions(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestActionUpdateAccessOwner(const QString &actionName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestActionUpdateAccessOwner(this->httpClient,actionName,accessOwner,authUser,authToken)));
}

RToolTask *RCloudClient::requestActionUpdateAccessMode(const QString &actionName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestActionUpdateAccessMode(this->httpClient,actionName,accessMode,authUser,authToken)));
}

RToolTask *RCloudClient::requestStatistics(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestStatistics(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestListProcesses(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListProcesses(this->httpClient,authUser,authToken)));
}

RToolTask *RCloudClient::requestProcessUpdateAccessOwner(const QString &processName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcessUpdateAccessOwner(this->httpClient,processName,accessOwner,authUser,authToken)));
}

RToolTask *RCloudClient::requestProcessUpdateAccessMode(const QString &processName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcessUpdateAccessMode(this->httpClient,processName,accessMode,authUser,authToken)));
}

RToolTask *RCloudClient::requestSubmitReport(const RReportRecord &reportRecord, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestSubmitReport(this->httpClient,reportRecord,authUser,authToken)));
}

RToolTask *RCloudClient::submitAction(const QSharedPointer<RCloudToolAction> &toolAction)
{
    R_LOG_TRACE_IN;
    RToolInput toolInput;
    toolInput.addAction(toolAction);
    RToolTask *toolTask = new RToolTask(toolInput);
    toolTask->setBlocking(this->blocking);
    toolTask->setParallel(true);

    QObject::connect(toolTask, &RToolTask::actionFinished, this, &RCloudClient::onActionFinished);
    QObject::connect(toolTask, &RToolTask::actionFailed, this, &RCloudClient::onActionFailed, Qt::DirectConnection);
    QObject::connect(toolTask, &RToolTask::finished, this, &RCloudClient::onTaskFinished);
    QObject::connect(toolTask, &RToolTask::failed, this, &RCloudClient::onTaskFailed);

    RJobManager::getInstance().submit(toolTask);
    emit this->submitted();
    R_LOG_TRACE_RETURN(toolTask);
}

void RCloudClient::cancelTask()
{
    R_LOG_TRACE_IN;
    this->httpClient->abort();
    R_LOG_TRACE_OUT;
}

void RCloudClient::setHttpClientSettings(const RHttpClientSettings &httpClientSettings)
{
    R_LOG_TRACE_IN;
    this->httpClient->setHttpClientSettings(httpClientSettings);
    emit this->configurationChanged();
    R_LOG_TRACE_OUT;
}

void RCloudClient::onActionFinished(const QSharedPointer<RToolAction> &action)
{
    R_LOG_TRACE_IN;
    RLogger::debug("Requested cloud action has finished.\n");
    RHttpMessage responseMessage = action.staticCast<RCloudToolAction>().data()->getResponseMessage();

    RCloudToolAction::Type actionType = action.staticCast<RCloudToolAction>().data()->getType();

    switch (actionType)
    {
        case RCloudToolAction::Test:
        {
            emit this->connectionTestFinished(RCloudToolAction::processTestResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ListFiles:
        {
            emit this->fileListAvailable(RCloudToolAction::processListFilesResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpload:
        {
            emit this->fileUploaded(RCloudToolAction::processFileUploadResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpdate:
        {
            emit this->fileUpdated(RCloudToolAction::processFileUpdateResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpdateAccessMode:
        {
            emit this->fileAccessModeUpdated(RCloudToolAction::processFileUpdateAccessModeResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpdateAccessOwner:
        {
            emit this->fileAccessModeUpdated(RCloudToolAction::processFileUpdateAccessOwnerResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpdateVersion:
        {
            emit this->fileVersionUpdated(RCloudToolAction::processFileUpdateVersionResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileUpdateTags:
        {
            emit this->fileTagsUpdated(RCloudToolAction::processFileUpdateTagsResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::FileDownload:
        {
            QString filePath = responseMessage.getProperties()[RCloudAction::Resource::Name::key];
            if (RFileTools::writeBinaryFile(filePath,responseMessage.getBody()))
            {
                emit this->fileDownloaded(filePath);
            }
            else
            {
                RLogger::error("Failed to write downloaded file \"%s\".\n", filePath.toUtf8().constData());
            }
            break;
        }
        case RCloudToolAction::FileRemove:
        {
            emit this->fileRemoved(RCloudToolAction::processFileRemoveResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ListUsers:
        {
            emit this->userListAvailable(RCloudToolAction::processListUsersResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::UserAdd:
        {
            emit this->userAdded(RCloudToolAction::processUserAddResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::UserUpdate:
        {
            emit this->userUpdated(RCloudToolAction::processUserUpdateResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::UserRemove:
        {
            emit this->userRemoved(RCloudToolAction::processUserRemoveResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ListUserTokens:
        {
            emit this->userTokenListAvailable(RCloudToolAction::processListUserTokensResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::UserTokenGenerate:
        {
            emit this->userTokenGenerated(RCloudToolAction::processUserTokenGenerateResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::UserTokenRemove:
        {
            emit this->userTokenRemoved(RCloudToolAction::processUserTokenRemoveResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ListGroups:
        {
            emit this->groupListAvailable(RCloudToolAction::processListGroupsResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::GroupAdd:
        {
            emit this->groupAdded(RCloudToolAction::processGroupAddResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::GroupRemove:
        {
            emit this->groupRemoved(RCloudToolAction::processGroupRemoveResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ListActions:
        {
            emit this->actionListAvailable(RCloudToolAction::processListActionsResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ActionUpdateAccessOwner:
        {
            emit this->actionAccessOwnerUpdated(RCloudToolAction::processActionUpdateAccessOwnerResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ActionUpdateAccessMode:
        {
            emit this->actionAccessModeUpdated(RCloudToolAction::processActionUpdateAccessModeResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::Process:
        {
            RCloudProcessResponse processResponse = RCloudProcessResponse::fromJson(QJsonDocument::fromJson(responseMessage.getBody()).object());
            if (processResponse.getProcessRequest().getName() == "process-csr")
            {
                QByteArray certificateContent = QByteArray::fromBase64(processResponse.getResponseMessage().toUtf8());
                emit this->signedCertificateAvailable(QSslCertificate(certificateContent));
            }
            // else if (processResponse.getProcessRequest().getName() == "process-report")
            // {
            //     QByteArray certificateContent = QByteArray::fromBase64(processResponse.getResponseMessage().toUtf8());
            //     emit this->signedCertificateAvailable(QSslCertificate(certificateContent));
            // }
            emit this->processFinished(processResponse.getResponseMessage());
            break;
        }
        case RCloudToolAction::ListProcesses:
        {
            emit this->processListAvailable(RCloudToolAction::processListProcessesResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ProcessUpdateAccessOwner:
        {
            emit this->processAccessOwnerUpdated(RCloudToolAction::processProcessUpdateAccessOwnerResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::ProcessUpdateAccessMode:
        {
            emit this->processAccessModeUpdated(RCloudToolAction::processProcessUpdateAccessModeResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::SubmitReport:
        {
            emit this->reportSubmitted(RCloudToolAction::processSubmitReportResponse(responseMessage.getBody()));
            break;
        }
        case RCloudToolAction::Statistics:
        {
            emit this->statisticsAvailable(RCloudToolAction::processStatisticsResponse(responseMessage.getBody()));
            break;
        }
        default:
        {
            RLogger::warning("Unhandled action type \'%d\'\n",actionType);
            RLogger::info("Received response:\n");
            RLogger::info("%s\n",responseMessage.getBody().constData());
            break;
        }
    }

    emit this->actionFinished();

    R_LOG_TRACE_OUT;
}

void RCloudClient::onActionFailed(const QSharedPointer<RToolAction> &action)
{
    R_LOG_TRACE_IN;
    RLogger::error("Requested cloud action has failed.\n");
    RHttpMessage responseMessage = action.staticCast<RCloudToolAction>().data()->getResponseMessage();
    emit this->actionFailed(action->getErrorType(), action->getErrorMessage(), responseMessage.getBody());
    R_LOG_TRACE_OUT;
}

void RCloudClient::onTaskFinished()
{
    R_LOG_TRACE_IN;
    RLogger::debug("Requested cloud task has finished.\n");
    emit this->finished();
    R_LOG_TRACE_OUT;
}

void RCloudClient::onTaskFailed()
{
    R_LOG_TRACE_IN;
    RLogger::error("Requested cloud task has failed.\n");
    emit this->failed();
    R_LOG_TRACE_OUT;
}

void RCloudClient::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    R_LOG_TRACE_IN;
    emit this->uploadProgress(bytesSent,bytesTotal);
    R_LOG_TRACE_OUT;
}

void RCloudClient::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    R_LOG_TRACE_IN;
    emit this->downloadProgress(bytesReceived,bytesTotal);
    R_LOG_TRACE_OUT;
}
