#include <QFile>
#include <QJsonDocument>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rcl_cloud_tool_action.h"

#include "rcl_cloud_action.h"
#include "rcl_cloud_tool_action.h"
#include "rcl_file_tools.h"

Q_DECLARE_METATYPE(RCloudAction);

void RCloudToolAction::_init(const RCloudToolAction *pRCloudToolAction)
{
    R_LOG_TRACE_IN;
    if (pRCloudToolAction)
    {
        this->type = pRCloudToolAction->type;
        this->input = pRCloudToolAction->input;
        this->httpClient = pRCloudToolAction->httpClient;
        this->requestMessage = pRCloudToolAction->requestMessage;
        this->responseMessage = pRCloudToolAction->responseMessage;
    }
    R_LOG_TRACE_OUT;
}

RCloudToolAction::RCloudToolAction(Type type, RHttpClient *httpClient)
    : type(type)
    , httpClient(httpClient)
{
    R_LOG_TRACE_IN;
    this->_init();
    R_LOG_TRACE_OUT;
}

RCloudToolAction::RCloudToolAction(const RCloudToolAction &toolAction)
    : RToolAction(toolAction)
{
    R_LOG_TRACE_IN;
    this->_init(&toolAction);
    R_LOG_TRACE_OUT;
}

RCloudToolAction::~RCloudToolAction()
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_OUT;
}

RCloudToolAction &RCloudToolAction::operator =(const RCloudToolAction &toolAction)
{
    R_LOG_TRACE_IN;
    this->_init(&toolAction);
    R_LOG_TRACE_RETURN(*this);
}

const RHttpClient *RCloudToolAction::getHttpClient() const
{
    return this->httpClient;
}

RHttpClient *RCloudToolAction::getHttpClient()
{
    return this->httpClient;
}

RCloudToolAction::Type RCloudToolAction::getType() const
{
    return this->type;
}

const RHttpMessage &RCloudToolAction::getRequestMessage() const
{
    return this->requestMessage;
}

const RHttpMessage &RCloudToolAction::getResponseMessage() const
{
    return this->responseMessage;
}

void RCloudToolAction::perform()
{
    R_LOG_TRACE_IN;

    if (!this->httpClient)
    {
        RLogger::error("HTTP client service is not running.\n");
        R_LOG_TRACE_OUT;
        return;
    }

    switch (this->type)
    {
        case Test:
        case ListFiles:
        case FileInfo:
        case FileUpload:
        case FileUpdate:
        case FileUpdateAccessOwner:
        case FileUpdateAccessMode:
        case FileUpdateVersion:
        case FileUpdateTags:
        case FileDownload:
        case FileRemove:
        case Stop:
        case Statistics:
        case Process:
        case ListUsers:
        case UserInfo:
        case UserAdd:
        case UserUpdate:
        case UserRemove:
        case UserRegister:
        case ListUserTokens:
        case UserTokenGenerate:
        case UserTokenRemove:
        case ListGroups:
        case GroupInfo:
        case GroupAdd:
        case GroupRemove:
        case ListActions:
        case ActionUpdateAccessOwner:
        case ActionUpdateAccessMode:
        case ListProcesses:
        case ProcessUpdateAccessOwner:
        case ProcessUpdateAccessMode:
        case SubmitReport:
        {
            if (this->httpClient)
            {
                RLogger::debug("HTTP request\n");
                try
                {
                    this->requestMessage = qvariant_cast<RCloudAction>(this->input);
                    this->requestMessage.setCorrelationId(QUuid::createUuid());

                    if (RLogger::getInstance().getLevel() & RLogLevel::Debug)
                    {
                        this->requestMessage.print(this->type != FileUpload && this->type != FileUpdate);
                    }

                    this->httpClient->sendRequest(this->requestMessage,this->responseMessage);

                    if (this->responseMessage.getErrorType() == RError::None)
                    {

                        RLogger::debug("HTTP reply received\n");
                        if (RLogger::getInstance().getLevel() & RLogLevel::Debug)
                        {
                            this->responseMessage.print(this->type != FileDownload || this->responseMessage.getErrorType() != RError::None);
                        }
                    }
                    else
                    {
                        throw RError(RError::Type::Connection,R_ERROR_REF,"Cloud tool action: HTTP Request has failed. %s",
                                     RError::getTypeMessage(this->responseMessage.getErrorType()).toUtf8().constData());
                    }
                }
                catch (const RError &error)
                {
                    throw error;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
    R_LOG_TRACE_OUT;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestTest(RHttpClient *httpClient, const QString &message, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(Test,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::Test::key,QString(),QUuid(),message.toUtf8()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QString RCloudToolAction::processTestResponse(const QByteArray &data)
{
    return QString(data);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListFiles(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListFiles,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListFiles::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RFileInfo> RCloudToolAction::processListFilesResponse(const QByteArray &data)
{
    QList<RFileInfo> fileInfoList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["files"]; v.isArray())
    {
        const QJsonArray &fileInfosJson = v.toArray();
        for (const QJsonValue &fileInfoJson : fileInfosJson)
        {
            if (fileInfoJson.isObject())
            {
                fileInfoList.append(RFileInfo::fromJson(fileInfoJson.toObject()));
            }
        }
    }

    return fileInfoList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileInfo(RHttpClient *httpClient, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileInfo,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileInfo::key,QString(),id,QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpload(RHttpClient *httpClient, const QString &filePath, const QString &name, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpload,httpClient);

    QByteArray fileContent;
    if (!RFileTools::readBinaryFile(filePath,fileContent))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,
                     "Failed to read file \"%s\" for upload.",
                     filePath.toUtf8().constData());
    }

    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpload::key,name,QUuid(),fileContent));

    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUploadResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpdate(RHttpClient *httpClient, const QString &filePath, const QString &name, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpdate,httpClient);

    QByteArray fileContent;
    if (!RFileTools::readBinaryFile(filePath,fileContent))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,
                     "Failed to read file \"%s\" for upload.",
                     filePath.toUtf8().constData());
    }

    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpdate::key,name,id,fileContent));

    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUpdateResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpdateAccessOwner(RHttpClient *httpClient, const RAccessOwner &accessOwner, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpdateAccessOwner,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpdateAccessOwner::key,QString(),id,QJsonDocument(accessOwner.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUpdateAccessOwnerResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpdateAccessMode(RHttpClient *httpClient, const RAccessMode &accessMode, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpdateAccessMode,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpdateAccessMode::key,QString(),id,QJsonDocument(accessMode.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUpdateAccessModeResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpdateVersion(RHttpClient *httpClient, const RVersion &version, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpdateVersion,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpdateVersion::key,QString(),id,version.toString().toUtf8()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUpdateVersionResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileUpdateTags(RHttpClient *httpClient, const QStringList &tags, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileUpdateTags,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileUpdateTags::key,QString(),id,tags.join(',').toUtf8()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileUpdateTagsResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileDownload(RHttpClient *httpClient, const QString &filePath, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileDownload,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileDownload::key,filePath,id,QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestFileRemove(RHttpClient *httpClient, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(FileRemove,httpClient);

    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::FileRemove::key,QString(),id,QByteArray()));

    return QSharedPointer<RCloudToolAction>(toolAction);
}

RFileInfo RCloudToolAction::processFileRemoveResponse(const QByteArray &data)
{
    return RFileInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestStop(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(Stop,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::Stop::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestStatistics(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(Statistics,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::Statistics::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QString RCloudToolAction::processStatisticsResponse(const QByteArray &data)
{
    return QString(data);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestProcess(RHttpClient *httpClient, const RCloudProcessRequest &processRequest, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(Process,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::Process::key,QString(),QUuid(),QJsonDocument(processRequest.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListUsers(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListUsers,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListUsers::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RUserInfo> RCloudToolAction::processListUsersResponse(const QByteArray &data)
{
    QList<RUserInfo> userInfoList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["users"]; v.isArray())
    {
        const QJsonArray &userInfosJson = v.toArray();
        for (const QJsonValue &userInfoJson : userInfosJson)
        {
            if (userInfoJson.isObject())
            {
                userInfoList.append(RUserInfo::fromJson(userInfoJson.toObject()));
            }
        }
    }

    return userInfoList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserInfo(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserInfo,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserInfo::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserAdd(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserAdd,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserAdd::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RUserInfo RCloudToolAction::processUserAddResponse(const QByteArray &data)
{
    return RUserInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserUpdate(RHttpClient *httpClient, const QString &userName, const RUserInfo &userInfo, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserUpdate,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserUpdate::key,userName,QUuid(),QJsonDocument(userInfo.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RUserInfo RCloudToolAction::processUserUpdateResponse(const QByteArray &data)
{
    return RUserInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserRemove(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserRemove,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserRemove::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QString RCloudToolAction::processUserRemoveResponse(const QByteArray &data)
{
    return data;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserRegister(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserRegister,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserRegister::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RUserInfo RCloudToolAction::processUserRegisterResponse(const QByteArray &data)
{
    return RUserInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListUserTokens(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListUserTokens,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListUserTokens::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RAuthToken> RCloudToolAction::processListUserTokensResponse(const QByteArray &data)
{
    QList<RAuthToken> authTokenList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["tokens"]; v.isArray())
    {
        const QJsonArray &authTokensJson = v.toArray();
        for (const QJsonValue &authTokenJson : authTokensJson)
        {
            if (authTokenJson.isObject())
            {
                authTokenList.append(RAuthToken::fromJson(authTokenJson.toObject()));
            }
        }
    }

    return authTokenList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserTokenGenerate(RHttpClient *httpClient, const QString &userName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserTokenGenerate,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserTokenGenerate::key,userName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RAuthToken RCloudToolAction::processUserTokenGenerateResponse(const QByteArray &data)
{
    return RAuthToken::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestUserTokenRemove(RHttpClient *httpClient, const QUuid &id, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(UserTokenRemove,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::UserTokenRemove::key,QString(),id,QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QUuid RCloudToolAction::processUserTokenRemoveResponse(const QByteArray &data)
{
    return QUuid(data);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListGroups(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListGroups,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListGroups::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RGroupInfo> RCloudToolAction::processListGroupsResponse(const QByteArray &data)
{
    QList<RGroupInfo> groupInfoList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["groups"]; v.isArray())
    {
        const QJsonArray &groupInfosJson = v.toArray();
        for (const QJsonValue &groupInfoJson : groupInfosJson)
        {
            if (groupInfoJson.isObject())
            {
                groupInfoList.append(RGroupInfo::fromJson(groupInfoJson.toObject()));
            }
        }
    }

    return groupInfoList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestGroupInfo(RHttpClient *httpClient, const QString &groupName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(GroupInfo,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::GroupInfo::key,groupName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestGroupAdd(RHttpClient *httpClient, const QString &groupName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(GroupAdd,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::GroupAdd::key,groupName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RGroupInfo RCloudToolAction::processGroupAddResponse(const QByteArray &data)
{
    return RGroupInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestGroupRemove(RHttpClient *httpClient, const QString &groupName, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(GroupRemove,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::GroupRemove::key,groupName,QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QString RCloudToolAction::processGroupRemoveResponse(const QByteArray &data)
{
    return data;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListActions(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListActions,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListActions::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RCloudActionInfo> RCloudToolAction::processListActionsResponse(const QByteArray &data)
{
    QList<RCloudActionInfo> actionInfoList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["actions"]; v.isArray())
    {
        const QJsonArray &actionInfosJson = v.toArray();
        for (const QJsonValue &actionInfoJson : actionInfosJson)
        {
            if (actionInfoJson.isObject())
            {
                actionInfoList.append(RCloudActionInfo::fromJson(actionInfoJson.toObject()));
            }
        }
    }

    return actionInfoList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestActionUpdateAccessOwner(RHttpClient *httpClient, const QString &actionName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ActionUpdateAccessOwner,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ActionUpdateAccessOwner::key,actionName,QUuid(),QJsonDocument(accessOwner.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RCloudActionInfo RCloudToolAction::processActionUpdateAccessOwnerResponse(const QByteArray &data)
{
    return RCloudActionInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestActionUpdateAccessMode(RHttpClient *httpClient, const QString &actionName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ActionUpdateAccessMode,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ActionUpdateAccessMode::key,actionName,QUuid(),QJsonDocument(accessMode.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RCloudActionInfo RCloudToolAction::processActionUpdateAccessModeResponse(const QByteArray &data)
{
    return RCloudActionInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestListProcesses(RHttpClient *httpClient, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ListProcesses,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ListProcesses::key,QString(),QUuid(),QByteArray()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QList<RCloudProcessInfo> RCloudToolAction::processListProcessesResponse(const QByteArray &data)
{
    QList<RCloudProcessInfo> processInfoList;

    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();

    if (const QJsonValue &v = jsonObject["processes"]; v.isArray())
    {
        const QJsonArray &processInfosJson = v.toArray();
        for (const QJsonValue &processInfoJson : processInfosJson)
        {
            if (processInfoJson.isObject())
            {
                processInfoList.append(RCloudProcessInfo::fromJson(processInfoJson.toObject()));
            }
        }
    }

    return processInfoList;
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestProcessUpdateAccessOwner(
    RHttpClient *httpClient, const QString &processName, const RAccessOwner &accessOwner, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ProcessUpdateAccessOwner,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ProcessUpdateAccessOwner::key,processName,QUuid(),QJsonDocument(accessOwner.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RCloudProcessInfo RCloudToolAction::processProcessUpdateAccessOwnerResponse(const QByteArray &data)
{
    return RCloudProcessInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestProcessUpdateAccessMode(
    RHttpClient *httpClient, const QString &processName, const RAccessMode &accessMode, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(ProcessUpdateAccessMode,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::ProcessUpdateAccessMode::key,processName,QUuid(),QJsonDocument(accessMode.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

RCloudProcessInfo RCloudToolAction::processProcessUpdateAccessModeResponse(const QByteArray &data)
{
    return RCloudProcessInfo::fromJson(QJsonDocument::fromJson(data).object());
}

QSharedPointer<RCloudToolAction> RCloudToolAction::requestSubmitReport(RHttpClient *httpClient, const RReportRecord &reportRecord, const QString &authUser, const QString &authToken)
{
    RCloudToolAction *toolAction = new RCloudToolAction(SubmitReport,httpClient);
    toolAction->input.setValue<RCloudAction>(RCloudAction(QUuid::createUuid(),authUser,authToken,RCloudAction::Action::SubmitReport::key,QString(),QUuid(),QJsonDocument(reportRecord.toJson()).toJson()));
    return QSharedPointer<RCloudToolAction>(toolAction);
}

QString RCloudToolAction::processSubmitReportResponse(const QByteArray &data)
{
    return data;
}
