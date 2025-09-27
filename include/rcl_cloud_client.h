#ifndef RCL_CLOUD_CLIENT_H
#define RCL_CLOUD_CLIENT_H

#include <QObject>

#include "rbl_tool_task.h"

#include "rcl_auth_token.h"
#include "rcl_http_client.h"
#include "rcl_cloud_process_info.h"
#include "rcl_cloud_tool_action.h"

class RCloudClient : public QObject
{
    Q_OBJECT

    protected:

        //! Client type.
        RHttpClient::Type type;
        //! Http client settings.
        RHttpClientSettings httpClientSettings;
        //! Blocking task.
        bool blocking;

    public:

        //! Constructor.
        explicit RCloudClient(RHttpClient::Type type, const RHttpClientSettings &httpClientSettings, QObject *parent = nullptr);

        //! Set blocking.
        void setBlocking(bool blocking);

        //! Submit test request.
        RToolTask *requestTest(const QString &responseMessage, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit CSR request.
        RToolTask *requestCsrProcess(const QByteArray &csrBase64, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit report request.
        RToolTask *requestReportProcess(const QByteArray &reportBase64, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit process request.
        RToolTask *requestProcess(const RCloudProcessRequest &processRequest, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list files request.
        RToolTask *requestListFiles(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file upload request.
        RToolTask *requestFileUpload(const QString &filePath, const QString &name, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file update request.
        RToolTask *requestFileUpdate(const QString &filePath, const QString &name, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file access owner update request.
        RToolTask *requestFileUpdateAccessOwner(const RAccessOwner &accessOwner, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file access mode update request.
        RToolTask *requestFileUpdateAccessMode(const RAccessMode &accessMode, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file version update request.
        RToolTask *requestFileUpdateVersion(const RVersion &version, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file tags update request.
        RToolTask *requestFileUpdateTags(const QStringList &tags, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file download request.
        RToolTask *requestFileDownload(const QString &filePath, const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit file remove request.
        RToolTask *requestFileRemove(const QUuid &fileId, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list users request.
        RToolTask *requestListUsers(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit user add request.
        RToolTask *requestUserAdd(const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit user update request.
        RToolTask *requestUserUpdate(const QString &userName, const RUserInfo &userInfo, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit user remove request.
        RToolTask *requestUserRemove(const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list user authentication tokens request.
        RToolTask *requestListUserTokens(const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit generate user authentication token request.
        RToolTask *requestUserTokenGenerate(const QString &userName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit remove user authentication token request.
        RToolTask *requestUserTokenRemove(const QUuid &id, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list groups request.
        RToolTask *requestListGroups(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit group add request.
        RToolTask *requestGroupAdd(const QString &groupName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit group remove request.
        RToolTask *requestGroupRemove(const QString &groupName, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list actions request.
        RToolTask *requestListActions(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit action update access owner request.
        RToolTask *requestActionUpdateAccessOwner(const QString &actionName, const RAccessOwner &accessOwner, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit action update access mode request.
        RToolTask *requestActionUpdateAccessMode(const QString &actionName, const RAccessMode &accessMode, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit statistics request.
        RToolTask *requestStatistics(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit list processes request.
        RToolTask *requestListProcesses(const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit process update access owner request.
        RToolTask *requestProcessUpdateAccessOwner(const QString &processName, const RAccessOwner &accessOwner, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit process update access mode request.
        RToolTask *requestProcessUpdateAccessMode(const QString &processName, const RAccessMode &accessMode, const QString &authUser = QString(), const QString &authToken = QString());

        //! Submit submit report request.
        RToolTask *requestSubmitReport(const RReportRecord &reportRecord, const QString &authUser = QString(), const QString &authToken = QString());

    private:

        //! Submit task.
        RToolTask *submitAction(const QSharedPointer<RCloudToolAction> &toolAction);

    public slots:

        //! Set new http client settings.
        void setHttpClientSettings(const RHttpClientSettings &httpClientSettings);

    protected slots:

        //! Task has finished.
        void onActionFinished(const QSharedPointer<RToolAction> &action);

        //! Task has failed.
        void onActionFailed(const QSharedPointer<RToolAction> &action);

        //! Task has finished.
        void onTaskFinished();

        //! Task has failed.
        void onTaskFailed();

        //! Upload progress.
        void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

        //! Download progress.
        void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    signals:

        //! Client configuration has changed.
        void configurationChanged();

        //! Task has submitted.
        void submitted();

        //! Task has finished.
        void finished();

        //! Task has failed.
        void failed();

        //! Action has failed.
        void actionFinished();

        //! Action has failed.
        void actionFailed(RError::Type errorType, QString errorMessage, QString message);

        //! Signed certificate is available.
        void signedCertificateAvailable(QSslCertificate certificate);

        //! Connection test finished.
        void connectionTestFinished(QString message);

        //! Process is finished.
        void processFinished(QString message);

        //! File list is available.
        void fileListAvailable(QList<RFileInfo> fileInfoList);

        //! File was uploaded.
        void fileUploaded(RFileInfo fileInfo);

        //! File was updated.
        void fileUpdated(RFileInfo fileInfo);

        //! File access mode was updated.
        void fileAccessModeUpdated(RFileInfo fileInfo);

        //! File access owner was updated.
        void fileAccessOwnerUpdated(RFileInfo fileInfo);

        //! File version was updated.
        void fileVersionUpdated(RFileInfo fileInfo);

        //! File tags was updated.
        void fileTagsUpdated(RFileInfo fileInfo);

        //! File was downloaded.
        void fileDownloaded(QString fileName);

        //! File was removed.
        void fileRemoved(RFileInfo fileInfo);

        //! User list is available.
        void userListAvailable(QList<RUserInfo> userInfoList);

        //! User was added.
        void userAdded(RUserInfo userInfo);

        //! User was updated.
        void userUpdated(RUserInfo userInfo);

        //! User was removed.
        void userRemoved(QString userName);

        //! User token list is available.
        void userTokenListAvailable(QList<RAuthToken> authTokenList);

        //! User authentication token was generated.
        void userTokenGenerated(RAuthToken authToken);

        //! User authentication token was removed.
        void userTokenRemoved(QUuid id);

        //! Group list is available.
        void groupListAvailable(QList<RGroupInfo> groupInfoList);

        //! Group was added.
        void groupAdded(RGroupInfo groupInfo);

        //! Group was removed.
        void groupRemoved(QString groupName);

        //! Upload progress.
        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

        //! Download progress.
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        //! Action list is available.
        void actionListAvailable(QList<RCloudActionInfo> actionInfoList);

        //! Action access owner was updated.
        void actionAccessOwnerUpdated(RCloudActionInfo actionInfo);

        //! Action access mode was updated.
        void actionAccessModeUpdated(RCloudActionInfo actionInfo);

        //! Process list is available.
        void processListAvailable(QList<RCloudProcessInfo> processInfoList);

        //! Action access owner was updated.
        void processAccessOwnerUpdated(RCloudProcessInfo processInfo);

        //! Action access mode was updated.
        void processAccessModeUpdated(RCloudProcessInfo processInfo);

        //! Report was submitted.
        void reportSubmitted(QString response);

        //! File was downloaded.
        void statisticsAvailable(QString statistics);

};

#endif // RCL_CLOUD_CLIENT_H
