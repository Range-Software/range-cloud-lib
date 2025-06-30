#include "rcl_cloud_action.h"

const QString RCloudAction::Auth::User::key = "auth-user";
const QString RCloudAction::Auth::User::description = "Authentication user";

const QString RCloudAction::Auth::Token::key = "auth-token";
const QString RCloudAction::Auth::Token::description = "Authentication token";

const QString RCloudAction::Resource::Id::key = "resource-id";
const QString RCloudAction::Resource::Id::description = "Resource id";

const QString RCloudAction::Resource::Name::key = "resource-name";
const QString RCloudAction::Resource::Name::description = "Resource name";

const QString RCloudAction::Resource::Path::key = "resource-path";
const QString RCloudAction::Resource::Path::description = "Resource path";

const QString RCloudAction::Action::key = "action";

const QString RCloudAction::Action::Test::key = "test-request";
const QString RCloudAction::Action::Test::description = "Send a test request (ping) to the cloud server";

const QString RCloudAction::Action::ListFiles::key = "list-files";
const QString RCloudAction::Action::ListFiles::description = "List files on the cloud server";

const QString RCloudAction::Action::FileInfo::key = "file-info";
const QString RCloudAction::Action::FileInfo::description = "Get file information";

const QString RCloudAction::Action::FileUpload::key = "file-upload";
const QString RCloudAction::Action::FileUpload::description = "Upload file to the cloud server";

const QString RCloudAction::Action::FileUpdate::key = "file-update";
const QString RCloudAction::Action::FileUpdate::description = "Update file on the cloud server";

const QString RCloudAction::Action::FileUpdateAccessOwner::key = "file-update-access-owner";
const QString RCloudAction::Action::FileUpdateAccessOwner::description = "Update file access owner on the cloud server";

const QString RCloudAction::Action::FileUpdateAccessMode::key = "file-update-access-mode";
const QString RCloudAction::Action::FileUpdateAccessMode::description = "Update file access mode on the cloud server";

const QString RCloudAction::Action::FileUpdateVersion::key = "file-update-version";
const QString RCloudAction::Action::FileUpdateVersion::description = "Update file version on the cloud server";

const QString RCloudAction::Action::FileUpdateTags::key = "file-update-tags";
const QString RCloudAction::Action::FileUpdateTags::description = "Update file tags on the cloud server";

const QString RCloudAction::Action::FileDownload::key = "file-download";
const QString RCloudAction::Action::FileDownload::description = "Download file from the cloud server";

const QString RCloudAction::Action::FileRemove::key = "file-remove";
const QString RCloudAction::Action::FileRemove::description = "Remove file from the cloud server";

const QString RCloudAction::Action::Stop::key = "stop";
const QString RCloudAction::Action::Stop::description = "Stop the cloud server";

const QString RCloudAction::Action::Statistics::key = "statistics";
const QString RCloudAction::Action::Statistics::description = "Cloud server statistics";

const QString RCloudAction::Action::Process::key = "process";
const QString RCloudAction::Action::Process::description = "Start process";

const QString RCloudAction::Action::ListUsers::key = "list-users";
const QString RCloudAction::Action::ListUsers::description = "List users on the cloud server";

const QString RCloudAction::Action::UserInfo::key = "user-info";
const QString RCloudAction::Action::UserInfo::description = "Get user information";

const QString RCloudAction::Action::UserAdd::key = "user-add";
const QString RCloudAction::Action::UserAdd::description = "Add new user";

const QString RCloudAction::Action::UserUpdate::key = "user-update";
const QString RCloudAction::Action::UserUpdate::description = "Update existing user";

const QString RCloudAction::Action::UserRemove::key = "user-remove";
const QString RCloudAction::Action::UserRemove::description = "Remove existing user";

const QString RCloudAction::Action::ListUserTokens::key = "list-user-tokens";
const QString RCloudAction::Action::ListUserTokens::description = "List user authentication tokens";

const QString RCloudAction::Action::UserTokenGenerate::key = "user-token-generate";
const QString RCloudAction::Action::UserTokenGenerate::description = "Generate user authentication token";

const QString RCloudAction::Action::UserTokenRemove::key = "user-token-remove";
const QString RCloudAction::Action::UserTokenRemove::description = "Remove user authentication token";

const QString RCloudAction::Action::ListGroups::key = "list-groups";
const QString RCloudAction::Action::ListGroups::description = "List groups on the cloud server";

const QString RCloudAction::Action::GroupInfo::key = "group-info";
const QString RCloudAction::Action::GroupInfo::description = "Get group information";

const QString RCloudAction::Action::GroupAdd::key = "group-add";
const QString RCloudAction::Action::GroupAdd::description = "Add new group";

const QString RCloudAction::Action::GroupRemove::key = "group-remove";
const QString RCloudAction::Action::GroupRemove::description = "Remove existing group";

const QString RCloudAction::Action::ListActions::key = "list-actions";
const QString RCloudAction::Action::ListActions::description = "List actions on the cloud server";

const QString RCloudAction::Action::ActionUpdateAccessOwner::key = "action-update-access-owner";
const QString RCloudAction::Action::ActionUpdateAccessOwner::description = "Update action access owner on the cloud server";

const QString RCloudAction::Action::ActionUpdateAccessMode::key = "action-update-access-mode";
const QString RCloudAction::Action::ActionUpdateAccessMode::description = "Update action access mode on the cloud server";

const QString RCloudAction::Action::ListProcesses::key = "list-processes";
const QString RCloudAction::Action::ListProcesses::description = "List processes on the cloud server";

const QString RCloudAction::Action::ProcessUpdateAccessOwner::key = "process-update-access-owner";
const QString RCloudAction::Action::ProcessUpdateAccessOwner::description = "Update process access owner on the cloud server";

const QString RCloudAction::Action::ProcessUpdateAccessMode::key = "process-update-access-mode";
const QString RCloudAction::Action::ProcessUpdateAccessMode::description = "Update process access mode on the cloud server";

const QString RCloudAction::Action::SubmitReport::key = "submit-report";
const QString RCloudAction::Action::SubmitReport::description = "Submit report to the cloud server";

void RCloudAction::_init(const RCloudAction *pCAction)
{
    if (pCAction)
    {
        this->id = pCAction->id;
        this->executor = pCAction->executor;
        this->authToken = pCAction->authToken;
        this->action = pCAction->action;
        this->resourceName = pCAction->resourceName;
        this->resourceId = pCAction->resourceId;
        this->data = pCAction->data;
        this->errorType = pCAction->errorType;
    }
}

RCloudAction::RCloudAction()
    : id(QUuid::createUuid())
{

}

RCloudAction::RCloudAction(const QUuid id,
                 const QString &executor,
                 const QString &authToken,
                 const QString &action,
                 const QString &resourceName,
                 const QUuid &resourceId,
                 const QByteArray &data)
    : id(id)
    , executor{executor}
    , authToken{authToken}
    , action{action}
    , resourceName{resourceName}
    , resourceId{resourceId}
    , data{data}
    , errorType{RError::None}
{
    this->_init();
}

RCloudAction::RCloudAction(const RCloudAction &action)
{
    this->_init(&action);
}

RCloudAction::~RCloudAction()
{
}

RCloudAction &RCloudAction::operator =(const RCloudAction &action)
{
    this->_init(&action);
    return (*this);
}

const QUuid &RCloudAction::getId() const
{
    return this->id;
}

const QString &RCloudAction::getExecutor() const
{
    return this->executor;
}

const QString &RCloudAction::getAuthToken() const
{
    return this->authToken;
}

const QString &RCloudAction::getAction() const
{
    return this->action;
}

const QString &RCloudAction::getResourceName() const
{
    return this->resourceName;
}

const QUuid &RCloudAction::getResourceId() const
{
    return this->resourceId;
}

const QByteArray &RCloudAction::getData() const
{
    return this->data;
}

void RCloudAction::setData(const QByteArray &data)
{
    this->data = data;
}

RError::Type RCloudAction::getErrorType() const
{
    return this->errorType;
}

void RCloudAction::setErrorType(RError::Type errorType)
{
    this->errorType = errorType;
}

QMap<QString, QString> RCloudAction::getActionMap()
{
    QMap<QString,QString> actionMap;

    actionMap.insert(Action::Test::key,Action::Test::description);
    actionMap.insert(Action::ListFiles::key,Action::ListFiles::description);
    actionMap.insert(Action::FileInfo::key,Action::FileInfo::description);
    actionMap.insert(Action::FileUpload::key,Action::FileUpload::description);
    actionMap.insert(Action::FileUpdate::key,Action::FileUpdate::description);
    actionMap.insert(Action::FileUpdateAccessOwner::key,Action::FileUpdateAccessOwner::description);
    actionMap.insert(Action::FileUpdateAccessMode::key,Action::FileUpdateAccessMode::description);
    actionMap.insert(Action::FileUpdateVersion::key,Action::FileUpdateVersion::description);
    actionMap.insert(Action::FileUpdateTags::key,Action::FileUpdateTags::description);
    actionMap.insert(Action::FileDownload::key,Action::FileDownload::description);
    actionMap.insert(Action::FileRemove::key,Action::FileRemove::description);
    actionMap.insert(Action::Stop::key,Action::Stop::description);
    actionMap.insert(Action::Statistics::key,Action::Statistics::description);
    actionMap.insert(Action::Process::key,Action::Process::description);
    actionMap.insert(Action::ListUsers::key,Action::ListUsers::description);
    actionMap.insert(Action::UserInfo::key,Action::UserInfo::description);
    actionMap.insert(Action::UserAdd::key,Action::UserAdd::description);
    actionMap.insert(Action::UserUpdate::key,Action::UserUpdate::description);
    actionMap.insert(Action::UserRemove::key,Action::UserRemove::description);
    actionMap.insert(Action::ListUserTokens::key,Action::ListUserTokens::description);
    actionMap.insert(Action::UserTokenGenerate::key,Action::UserTokenGenerate::description);
    actionMap.insert(Action::UserTokenRemove::key,Action::UserTokenRemove::description);
    actionMap.insert(Action::ListGroups::key,Action::ListGroups::description);
    actionMap.insert(Action::GroupInfo::key,Action::GroupInfo::description);
    actionMap.insert(Action::GroupAdd::key,Action::GroupAdd::description);
    actionMap.insert(Action::GroupRemove::key,Action::GroupRemove::description);
    actionMap.insert(Action::ListActions::key,Action::ListActions::description);
    actionMap.insert(Action::ActionUpdateAccessOwner::key,Action::ActionUpdateAccessOwner::description);
    actionMap.insert(Action::ActionUpdateAccessMode::key,Action::ActionUpdateAccessMode::description);
    actionMap.insert(Action::ListProcesses::key,Action::ListProcesses::description);
    actionMap.insert(Action::ProcessUpdateAccessOwner::key,Action::ProcessUpdateAccessOwner::description);
    actionMap.insert(Action::ProcessUpdateAccessMode::key,Action::ProcessUpdateAccessMode::description);
    actionMap.insert(Action::SubmitReport::key,Action::SubmitReport::description);

    return actionMap;
}
