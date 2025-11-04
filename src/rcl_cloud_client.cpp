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
    , type{type}
    , httpClientSettings{httpClientSettings}
    , blocking{true}
{
    R_LOG_TRACE_IN;
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
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestTest(new RHttpClient(this->type,this->httpClientSettings,this),responseMessage,authUser,authToken)));
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
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcess(new RHttpClient(this->type,this->httpClientSettings,this),processRequest,authUser,authToken)));
}

RToolTask *RCloudClient::requestListFiles(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListFiles(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpload(const QString &filePath, const QString &name, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpload(new RHttpClient(this->type,this->httpClientSettings,this),filePath,name,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileReplace(const QString &filePath, const QString &name, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileReplace(new RHttpClient(this->type,this->httpClientSettings,this),filePath,name,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdate(const QString &filePath, const QString &name, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdate(new RHttpClient(this->type,this->httpClientSettings,this),filePath,name,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateAccessOwner(const RAccessOwner &accessOwner, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateAccessOwner(new RHttpClient(this->type,this->httpClientSettings,this),accessOwner,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateAccessMode(const RAccessMode &accessMode, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateAccessMode(new RHttpClient(this->type,this->httpClientSettings,this),accessMode,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateVersion(const RVersion &version, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateVersion(new RHttpClient(this->type,this->httpClientSettings,this),version,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileUpdateTags(const QStringList &tags, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileUpdateTags(new RHttpClient(this->type,this->httpClientSettings,this),tags,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileDownload(const QString &filePath, const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileDownload(new RHttpClient(this->type,this->httpClientSettings,this),filePath,id,authUser,authToken)));
}

RToolTask *RCloudClient::requestFileRemove(const QUuid &fileId, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestFileRemove(new RHttpClient(this->type,this->httpClientSettings,this),fileId,authUser,authToken)));
}

RToolTask *RCloudClient::requestListUsers(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListUsers(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestUserAdd(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserAdd(new RHttpClient(this->type,this->httpClientSettings,this),userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserUpdate(const QString &userName, const RUserInfo &userInfo, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserUpdate(new RHttpClient(this->type,this->httpClientSettings,this),userName,userInfo,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserRemove(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserRemove(new RHttpClient(this->type,this->httpClientSettings,this),userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserRegister(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserRegister(new RHttpClient(this->type,this->httpClientSettings,this),userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestListUserTokens(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListUserTokens(new RHttpClient(this->type,this->httpClientSettings,this),userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserTokenGenerate(const QString &userName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserTokenGenerate(new RHttpClient(this->type,this->httpClientSettings,this),userName,authUser,authToken)));
}

RToolTask *RCloudClient::requestUserTokenRemove(const QUuid &id, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestUserTokenRemove(new RHttpClient(this->type,this->httpClientSettings,this),id,authUser,authToken)));
}

RToolTask *RCloudClient::requestListGroups(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListGroups(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestGroupAdd(const QString &groupName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestGroupAdd(new RHttpClient(this->type,this->httpClientSettings,this),groupName,authUser,authToken)));
}

RToolTask *RCloudClient::requestGroupRemove(const QString &groupName, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestGroupRemove(new RHttpClient(this->type,this->httpClientSettings,this),groupName,authUser,authToken)));
}

RToolTask *RCloudClient::requestListActions(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListActions(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestActionUpdateAccessOwner(const QString &actionName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestActionUpdateAccessOwner(new RHttpClient(this->type,this->httpClientSettings,this),actionName,accessOwner,authUser,authToken)));
}

RToolTask *RCloudClient::requestActionUpdateAccessMode(const QString &actionName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestActionUpdateAccessMode(new RHttpClient(this->type,this->httpClientSettings,this),actionName,accessMode,authUser,authToken)));
}

RToolTask *RCloudClient::requestStatistics(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestStatistics(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestListProcesses(const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestListProcesses(new RHttpClient(this->type,this->httpClientSettings,this),authUser,authToken)));
}

RToolTask *RCloudClient::requestProcessUpdateAccessOwner(const QString &processName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcessUpdateAccessOwner(new RHttpClient(this->type,this->httpClientSettings,this),processName,accessOwner,authUser,authToken)));
}

RToolTask *RCloudClient::requestProcessUpdateAccessMode(const QString &processName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestProcessUpdateAccessMode(new RHttpClient(this->type,this->httpClientSettings,this),processName,accessMode,authUser,authToken)));
}

RToolTask *RCloudClient::requestSubmitReport(const RReportRecord &reportRecord, const QString &authUser, const QString &authToken)
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->submitAction(RCloudToolAction::requestSubmitReport(new RHttpClient(this->type,this->httpClientSettings,this),reportRecord,authUser,authToken)));
}

RToolTask *RCloudClient::submitAction(const QSharedPointer<RCloudToolAction> &toolAction)
{
    R_LOG_TRACE_IN;
    RToolInput toolInput;
    toolInput.addAction(toolAction);
    RToolTask *toolTask = new RToolTask(toolInput);
    toolTask->setBlocking(this->blocking);
    toolTask->setParallel(false);

    toolAction.data()->getHttpClient()->setParent(toolTask);

    QObject::connect(toolAction.data()->getHttpClient(),&RHttpClient::uploadProgress,toolTask,&RJob::setProgress);
    QObject::connect(toolAction.data()->getHttpClient(),&RHttpClient::downloadProgress,toolTask,&RJob::setProgress);

    QObject::connect(toolAction.data()->getHttpClient(),&RHttpClient::uploadProgress,this,&RCloudClient::onUploadProgress);
    QObject::connect(toolAction.data()->getHttpClient(),&RHttpClient::downloadProgress,this,&RCloudClient::onDownloadProgress);

    QObject::connect(toolTask, &RToolTask::canceled, toolAction.data()->getHttpClient(), &RHttpClient::abort);
    QObject::connect(toolTask, &RToolTask::actionFinished, this, &RCloudClient::onActionFinished);
    QObject::connect(toolTask, &RToolTask::actionFailed, this, &RCloudClient::onActionFailed, Qt::DirectConnection);
    QObject::connect(toolTask, &RToolTask::finished, this, &RCloudClient::onTaskFinished);
    QObject::connect(toolTask, &RToolTask::failed, this, &RCloudClient::onTaskFailed);

    RJobManager::getInstance().submit(toolTask);
    emit this->submitted();
    R_LOG_TRACE_RETURN(toolTask);
}

void RCloudClient::setHttpClientSettings(const RHttpClientSettings &httpClientSettings)
{
    R_LOG_TRACE_IN;
    this->httpClientSettings = httpClientSettings;
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
        case RCloudToolAction::FileReplace:
        {
            emit this->fileReplaced(RCloudToolAction::processFileReplaceResponse(responseMessage.getBody()));
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
        case RCloudToolAction::UserRegister:
        {
            emit this->userRegistered(RCloudToolAction::processUserRegisterResponse(responseMessage.getBody()));
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
