#ifndef RCL_FILE_MANAGER_H
#define RCL_FILE_MANAGER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QMutex>

#include "rcl_cloud_client.h"
#include "rcl_file_manager_cache.h"
#include "rcl_file_manager_settings.h"

class RFileManager : public QObject
{

    Q_OBJECT

    protected:

        //! File manager settings.
        RFileManagerSettings fileManagerSettings;
        //! File manager cache.
        RFileManagerCache *fileManagerCache;
        //! Cloud client.
        RCloudClient *cloudClient;
        //! Local file system watcher.
        QFileSystemWatcher *localFileSystemWatcher;
        //! Remote refresh timer.
        QTimer *remoteRefreshTimer;
        //! Local change timer.
        //! Single change (e.g. saving a document) produces a burst of directory change notifications.
        //! Changes are collected and processed together once the directory has settled.
        QTimer *localChangeTimer;
        //! List of remote files.
        QList<RFileInfo> remoteFiles;
        //! List of local files.
        QFileInfoList localFiles;

        //! Number of currently running actions.
        uint nRunningActions;

        //! File manager is running.
        bool isRunning;

        //! Remote file list must be refreshed as soon as running actions are finished.
        bool remoteRefreshPending;

        struct
        {
            QMutex mutex;
            //! List of local files to be uploaded.
            QList<RFileInfo> localUpload;
            //! List of local files to be updaed.
            QList<RFileInfo> localUpdate;
            //! List of local files to be removed.
            QList<RFileInfo> localRemove;
            //! List of remote files to be downloaded.
            QList<RFileInfo> remoteDownload;
            //! List of remote files to be removed.
            QList<RFileInfo> remoteRemove;
        } filesToSync;

        static const QString logPrefix;

    public:

        //! Constructor.
        explicit RFileManager(const RFileManagerSettings &fileManagerSettings, RCloudClient *cloudClient, QObject *parent = nullptr);

        //! Get const reference to file manager settings.
        const RFileManagerSettings &getFileManagerSettings() const;

        //! Set new file manager settings.
        void setFileManagerSettings(const RFileManagerSettings &fileManagerSettings);

        //! Return true if file manager is running.
        bool isActive() const;

        //! Find remote file matching given local file name.
        //! Empty file info is returned if no such file is being synced.
        RFileInfo findRemoteFile(const QString &fileName) const;

        //! Start file manager.
        void start(uint remoteRefreshTimeout);

        //! Stop file manager.
        void stop();

        //! Clear cache.
        void clearCache();

    private:

        //! Initialize cache file.
        void initializeCacheFile();

        //! Request fresh list of remote files.
        //! Local and remote files are compared only against freshly received remote file list.
        //! If other actions are still running the request is postponed until they are finished.
        void requestRemoteRefresh();

        //! Return name of the user owning synchronized files.
        QString findUserName() const;

        //! Store remote file info (replace existing entry with the same id or append new one).
        void storeRemoteFile(const RFileInfo &fileInfo);

        //! Remove remote file info with given id.
        void removeRemoteFile(const QUuid &id);

        //! Finish one running action.
        void finishRunningAction();

        //! Sync files.
        void syncFiles();

        //! Compare files in local and remote file lists.
        void compareFileLists();

        //! Return incrementeded version.
        static RVersion incrementVersion(const RVersion &in);

    protected slots:

        //! File list is available.
        void onFileListAvailable(QList<RFileInfo> fileInfoList);

        //! List with files to sync are avalable.
        void onFilesToSyncAvailable();

        //! File has been removed.
        void onFileRemoved(RFileInfo fileInfo);

        //! File has been downloaded.
        void onFileDownloaded(QString fileName);

        //! File has been updated.
        void onFileUpdated(RFileInfo fileInfo);

        //! File has been uploaded.
        void onFileUploaded(RFileInfo fileInfo);

        //! File was replaced.
        void onFileReplaced(std::tuple<RFileInfo,QList<RFileInfo>> fileInfoList);

        //! File version has been updated.
        void onFileVersionUpdated(RFileInfo fileInfo);

        //! File tags has been updated.
        void onFileTagsUpdated(RFileInfo fileInfo);

        //! Cloud action has finished.
        void onCloudActionFinished();

        //! Cloud action has failed.
        void onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

        //! Local directory has changed.
        void onLocalDirectoryChanged(const QString &path);

        //! Local directory has settled after one or more changes.
        void onLocalChangeTimeout();

        //! Refresh timeout.
        void onRemoteRefreshTimeout();

    signals:

        //! Lists of files to be synced are available.
        void filesToSyncAvailable();

        //! Local file has been removed.
        void fileRemoved(const QString &fileName);

        //! Remote file has been removed.
        void fileRemoved(const RFileInfo &fileInfo);

        //! File has been downloaded.
        void fileDownloaded(const QString &fileName);

        //! File has been updated.
        void fileUpdated(const RFileInfo &fileInfo);

        //! File has been uploaded.
        void fileUploaded(const RFileInfo &fileInfo);

        //! File vesrion has been updated.
        void fileVersionUpdated(const RFileInfo &fileInfo);

        //! File tags have been updated.
        void fileTagsUpdated(const RFileInfo &fileInfo);

        //! Synchronization of files has been completed.
        void syncFilesCompleted();

        //! Cloud error has occurred.
        void cloudError(RError::Type errorType, const QString &errorMessage, const QString &message);

};

#endif // RCL_FILE_MANAGER_H
