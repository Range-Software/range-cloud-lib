#ifndef RCL_CLOUD_TOOL_ACTION_H
#define RCL_CLOUD_TOOL_ACTION_H

#include <QVariant>
#include <QSharedPointer>

#include <rbl_tool_action.h>

#include "rcl_access_mode.h"
#include "rcl_access_owner.h"
#include "rcl_auth_token.h"
#include "rcl_cloud_action_info.h"
#include "rcl_file_info.h"
#include "rcl_http_client.h"
#include "rcl_cloud_process_info.h"
#include "rcl_cloud_process_request.h"
#include "rcl_report_record.h"
#include "rcl_user_info.h"
#include "rcl_group_info.h"

class RCloudToolAction : public RToolAction
{

    public:

        enum Type
        {
            NoAction = 0,
            Test,
            ListFiles,
            FileInfo,
            FileUpload,
            FileUpdate,
            FileUpdateAccessOwner,
            FileUpdateAccessMode,
            FileUpdateVersion,
            FileUpdateTags,
            FileDownload,
            FileRemove,
            Stop,
            Statistics,
            Process,
            ListUsers,
            UserInfo,
            UserAdd,
            UserUpdate,
            UserRemove,
            UserSignUp,
            ListUserTokens,
            UserTokenGenerate,
            UserTokenRemove,
            ListGroups,
            GroupInfo,
            GroupAdd,
            GroupRemove,
            ListActions,
            ActionUpdateAccessOwner,
            ActionUpdateAccessMode,
            ListProcesses,
            ProcessUpdateAccessOwner,
            ProcessUpdateAccessMode,
            SubmitReport,
            NTypes
        };

    protected:

        //! Action type.
        Type type;
        //! HTTP client service.
        RHttpClient *httpClient;
        //! HTTP request.
        RHttpMessage requestMessage;
        //! HTTP response.
        RHttpMessage responseMessage;

    private:

        //! Internal initialization function.
        void _init(const RCloudToolAction *pCToolAction = nullptr);

    public:

        //! Constructor.
        RCloudToolAction(Type type, RHttpClient *httpClient);

        //! Copy constructor.
        RCloudToolAction(const RCloudToolAction &toolAction);

        //! Destructor.
        ~RCloudToolAction();

        //! Assignment operator.
        RCloudToolAction & operator =(const RCloudToolAction &toolAction);

        //! Return action type.
        Type getType() const;

        //! Return const pointer to http client.
        const RHttpClient *getHttpClient() const;

        //! Return pointer to http client.
        RHttpClient *getHttpClient();

        //! Return const rference to request HTTP message.
        const RHttpMessage &getRequestMessage() const;

        //! Return const rference to response HTTP message.
        const RHttpMessage &getResponseMessage() const;

        //! Perform action.
        void perform();

        //! Set action send test request.
        static QSharedPointer<RCloudToolAction> requestTest(RHttpClient *httpClient, const QString &message, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process test response.
        static QString processTestResponse(const QByteArray &data);

        //! Set action list files.
        static QSharedPointer<RCloudToolAction> requestListFiles(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list files response.
        static QList<RFileInfo> processListFilesResponse(const QByteArray &data);

        //! Set action file information.
        static QSharedPointer<RCloudToolAction> requestFileInfo(RHttpClient *httpClient, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action file upload.
        static QSharedPointer<RCloudToolAction> requestFileUpload(RHttpClient *httpClient, const QString &filePath, const QString &name, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file upload response.
        static RFileInfo processFileUploadResponse(const QByteArray &data);

        //! Set action file update.
        static QSharedPointer<RCloudToolAction> requestFileUpdate(RHttpClient *httpClient, const QString &filePath, const QString &name, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file update response.
        static RFileInfo processFileUpdateResponse(const QByteArray &data);

        //! Set action file update access owner.
        static QSharedPointer<RCloudToolAction> requestFileUpdateAccessOwner(RHttpClient *httpClient, const RAccessOwner &accessOwner, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file update access owner response.
        static RFileInfo processFileUpdateAccessOwnerResponse(const QByteArray &data);

        //! Set action file update access mode.
        static QSharedPointer<RCloudToolAction> requestFileUpdateAccessMode(RHttpClient *httpClient, const RAccessMode &accessMode, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file update access mode response.
        static RFileInfo processFileUpdateAccessModeResponse(const QByteArray &data);

        //! Set action file update version.
        static QSharedPointer<RCloudToolAction> requestFileUpdateVersion(RHttpClient *httpClient, const RVersion &version, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file update version response.
        static RFileInfo processFileUpdateVersionResponse(const QByteArray &data);

        //! Set action file update tags.
        static QSharedPointer<RCloudToolAction> requestFileUpdateTags(RHttpClient *httpClient, const QStringList &tags, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file update tags response.
        static RFileInfo processFileUpdateTagsResponse(const QByteArray &data);

        //! Set action file download.
        static QSharedPointer<RCloudToolAction> requestFileDownload(RHttpClient *httpClient, const QString &filePath, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action file remove.
        static QSharedPointer<RCloudToolAction> requestFileRemove(RHttpClient *httpClient, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process file remove response.
        static RFileInfo processFileRemoveResponse(const QByteArray &data);

        //! Set action requestStop.
        static QSharedPointer<RCloudToolAction> requestStop(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action send statistics request.
        static QSharedPointer<RCloudToolAction> requestStatistics(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process statistics response.
        static QString processStatisticsResponse(const QByteArray &data);

        //! Set action requestProcess.
        static QSharedPointer<RCloudToolAction> requestProcess(RHttpClient *httpClient, const RCloudProcessRequest &processRequest, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action list users.
        static QSharedPointer<RCloudToolAction> requestListUsers(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list users response.
        static QList<RUserInfo> processListUsersResponse(const QByteArray &data);

        //! Set action user information.
        static QSharedPointer<RCloudToolAction> requestUserInfo(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action add new user.
        static QSharedPointer<RCloudToolAction> requestUserAdd(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user add response.
        static RUserInfo processUserAddResponse(const QByteArray &data);

        //! Set action update existing user.
        static QSharedPointer<RCloudToolAction> requestUserUpdate(RHttpClient *httpClient, const QString &userName, const RUserInfo &userInfo, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user update response.
        static RUserInfo processUserUpdateResponse(const QByteArray &data);

        //! Set action remove existing user.
        static QSharedPointer<RCloudToolAction> requestUserRemove(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user remove response.
        static QString processUserRemoveResponse(const QByteArray &data);

        //! Set action to sign up suggested user name.
        static QSharedPointer<RCloudToolAction> requestUserSignUp(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user signup.
        static RUserInfo processUserSignUpResponse(const QByteArray &data);

        //! Set action list user authentication tokens.
        static QSharedPointer<RCloudToolAction> requestListUserTokens(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list user tokens response.
        static QList<RAuthToken> processListUserTokensResponse(const QByteArray &data);

        //! Set action generate user authentication token.
        static QSharedPointer<RCloudToolAction> requestUserTokenGenerate(RHttpClient *httpClient, const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user generate token response.
        static RAuthToken processUserTokenGenerateResponse(const QByteArray &data);

        //! Set action remove user authentication token.
        static QSharedPointer<RCloudToolAction> requestUserTokenRemove(RHttpClient *httpClient, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process user remove token response.
        static QUuid processUserTokenRemoveResponse(const QByteArray &data);

        //! Set action list groups.
        static QSharedPointer<RCloudToolAction> requestListGroups(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list groups response.
        static QList<RGroupInfo> processListGroupsResponse(const QByteArray &data);

        //! Set action group information.
        static QSharedPointer<RCloudToolAction> requestGroupInfo(RHttpClient *httpClient, const QString &groupName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Set action add new group.
        static QSharedPointer<RCloudToolAction> requestGroupAdd(RHttpClient *httpClient, const QString &groupName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process group add response.
        static RGroupInfo processGroupAddResponse(const QByteArray &data);

        //! Set action remove existing group.
        static QSharedPointer<RCloudToolAction> requestGroupRemove(RHttpClient *httpClient, const QString &groupName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process group remove response.
        static QString processGroupRemoveResponse(const QByteArray &data);

        //! Set action list actions.
        static QSharedPointer<RCloudToolAction> requestListActions(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list actions response.
        static QList<RCloudActionInfo> processListActionsResponse(const QByteArray &data);

        //! Set action update access owner action.
        static QSharedPointer<RCloudToolAction> requestActionUpdateAccessOwner(RHttpClient *httpClient, const QString &actionName, const RAccessOwner &accessOwner, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process action update access owner response.
        static RCloudActionInfo processActionUpdateAccessOwnerResponse(const QByteArray &data);

        //! Set action update access mode action.
        static QSharedPointer<RCloudToolAction> requestActionUpdateAccessMode(RHttpClient *httpClient, const QString &actionName, const RAccessMode &accessMode, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process action update access mode response.
        static RCloudActionInfo processActionUpdateAccessModeResponse(const QByteArray &data);

        //! Set action list processes.
        static QSharedPointer<RCloudToolAction> requestListProcesses(RHttpClient *httpClient, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process list processes response.
        static QList<RCloudProcessInfo> processListProcessesResponse(const QByteArray &data);

        //! Set action process update access owner.
        static QSharedPointer<RCloudToolAction> requestProcessUpdateAccessOwner(RHttpClient *httpClient, const QString &processName, const RAccessOwner &accessOwner, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process process update access owner response.
        static RCloudProcessInfo processProcessUpdateAccessOwnerResponse(const QByteArray &data);

        //! Set action process update access mode.
        static QSharedPointer<RCloudToolAction> requestProcessUpdateAccessMode(RHttpClient *httpClient, const QString &processName, const RAccessMode &accessMode, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process process update access mode response.
        static RCloudProcessInfo processProcessUpdateAccessModeResponse(const QByteArray &data);

        //! Set action submit report.
        static QSharedPointer<RCloudToolAction> requestSubmitReport(RHttpClient *httpClient, const RReportRecord &reportRecord, const QString &authUser = QString(), const QString &authToken = QString());

        //! Process submit report response.
        static QString processSubmitReportResponse(const QByteArray &data);

};

#endif // RCL_CLOUD_TOOL_ACTION_H
