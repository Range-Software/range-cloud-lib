#include <rbl_file_tools.h>
#include <rbl_logger.h>

#include <QDir>
#include <QMap>

#include "rcl_file_manager.h"

const QString RFileManager::logPrefix = "LocalFileManager";

//! Time [ms] the local directory must stay unchanged before changes are processed.
static const int localChangeSettleTimeout = 2000;

RFileManager::RFileManager(const RFileManagerSettings &fileManagerSettings, RCloudClient *cloudClient, QObject *parent)
    : QObject{parent}
    , fileManagerSettings{fileManagerSettings}
    , cloudClient{cloudClient}
    , nRunningActions{0}
    , isRunning{false}
    , remoteRefreshPending{false}
{
    R_LOG_TRACE_IN;
    this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());

    this->fileManagerCache = new RFileManagerCache(this);

    this->initializeCacheFile();

    this->localFileSystemWatcher = new QFileSystemWatcher(this);
    if (!this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
    {
        RLogger::warning("[%s] Could not add path \'%s\' to file system watcher\n",
                         RFileManager::logPrefix.toUtf8().constData(),
                         this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
    }
    QObject::connect(this->localFileSystemWatcher,&QFileSystemWatcher::directoryChanged,this,&RFileManager::onLocalDirectoryChanged);

    this->remoteRefreshTimer = new QTimer(this);
    QObject::connect(this->remoteRefreshTimer,&QTimer::timeout,this,&RFileManager::onRemoteRefreshTimeout);

    this->localChangeTimer = new QTimer(this);
    this->localChangeTimer->setSingleShot(true);
    this->localChangeTimer->setInterval(localChangeSettleTimeout);
    QObject::connect(this->localChangeTimer,&QTimer::timeout,this,&RFileManager::onLocalChangeTimeout);

    QObject::connect(this->cloudClient,&RCloudClient::fileListAvailable,this,&RFileManager::onFileListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::fileRemoved,this,&RFileManager::onFileRemoved);
    QObject::connect(this->cloudClient,&RCloudClient::fileDownloaded,this,&RFileManager::onFileDownloaded);
    QObject::connect(this->cloudClient,&RCloudClient::fileUpdated,this,&RFileManager::onFileUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileUploaded,this,&RFileManager::onFileUploaded);
    QObject::connect(this->cloudClient,&RCloudClient::fileReplaced,this,&RFileManager::onFileReplaced);
    QObject::connect(this->cloudClient,&RCloudClient::fileVersionUpdated,this,&RFileManager::onFileVersionUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileTagsUpdated,this,&RFileManager::onFileTagsUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::actionFinished,this,&RFileManager::onCloudActionFinished);
    QObject::connect(this->cloudClient,&RCloudClient::actionFailed,this,&RFileManager::onCloudActionFailed);

    QObject::connect(this,&RFileManager::filesToSyncAvailable,this,&RFileManager::onFilesToSyncAvailable);
    R_LOG_TRACE_OUT;
}

const RFileManagerSettings &RFileManager::getFileManagerSettings() const
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->fileManagerSettings);
}

void RFileManager::setFileManagerSettings(const RFileManagerSettings &fileManagerSettings)
{
    R_LOG_TRACE_IN;
    // Set new sync directory
    QString oldLocalDirectory = this->fileManagerSettings.getLocalDirectory();
    this->fileManagerSettings = fileManagerSettings;
    if (oldLocalDirectory != this->fileManagerSettings.getLocalDirectory())
    {
        this->localFileSystemWatcher->removePath(oldLocalDirectory);
        if (!this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
        {
            RLogger::warning("[%s] Could not add path \'%s\' to local file system watcher\n",
                             RFileManager::logPrefix.toUtf8().constData(),
                             this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
        }
    }
    R_LOG_TRACE_OUT;
}

bool RFileManager::isActive() const
{
    return this->isRunning;
}

RFileInfo RFileManager::findRemoteFile(const QString &fileName) const
{
    R_LOG_TRACE_IN;
    // Remote files are matched the same way as in compareFileLists.
    // Should there be more files with the same name the most recently updated one is returned.
    QString userName = this->findUserName();

    // Default constructed file info has a random id and current update time, therefore it cannot be used
    // as "nothing found yet" - it would win every comparison and its random id would be returned.
    const RFileInfo *foundFileInfo = nullptr;
    for (const RFileInfo &remoteFileInfo : std::as_const(this->remoteFiles))
    {
        if (remoteFileInfo.getPath() == fileName && remoteFileInfo.getAccessRights().getOwner().getUser() == userName)
        {
            if (!foundFileInfo || remoteFileInfo.getUpdateDateTime() > foundFileInfo->getUpdateDateTime())
            {
                foundFileInfo = &remoteFileInfo;
            }
        }
    }
    if (foundFileInfo)
    {
        R_LOG_TRACE_RETURN(*foundFileInfo);
    }
    // Not found - return file info with null id as documented.
    RFileInfo emptyFileInfo;
    emptyFileInfo.setId(QUuid());
    R_LOG_TRACE_RETURN(emptyFileInfo);
}

void RFileManager::start(uint remoteRefreshTimeout)
{
    RLogger::info("[%s] Start remote refresh timer with timeout %lu [ms]\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  remoteRefreshTimeout);
    this->remoteRefreshTimer->start(remoteRefreshTimeout);

    this->isRunning = true;

    // Do not wait for the first timeout - local changes are not synchronized until remote file list is known.
    this->requestRemoteRefresh();
}

void RFileManager::stop()
{
    if (this->remoteRefreshTimer->isActive())
    {
        RLogger::info("[%s] Stop remote refresh timer\n",
                      RFileManager::logPrefix.toUtf8().constData());
        this->remoteRefreshTimer->stop();
    }
    this->localChangeTimer->stop();
    this->remoteRefreshPending = false;
    this->isRunning = false;
}

void RFileManager::clearCache()
{
    R_LOG_TRACE_IN;
    this->filesToSync.mutex.lock();
    this->fileManagerCache->clear();
    this->filesToSync.mutex.unlock();
    R_LOG_TRACE_OUT;
}

void RFileManager::initializeCacheFile()
{
    R_LOG_TRACE_IN;
    if (QFile::exists(this->fileManagerSettings.getCacheFile()))
    {
        this->fileManagerCache->read(this->fileManagerSettings.getCacheFile());
    }
    R_LOG_TRACE_OUT;
}

void RFileManager::requestRemoteRefresh()
{
    R_LOG_TRACE_IN;
    if (!this->isRunning)
    {
        R_LOG_TRACE_OUT;
        return;
    }
    if (this->nRunningActions > 0)
    {
        // Remote file list would not reflect actions which are still running.
        RLogger::info("[%s] Postponing remote refresh because there are %u running actions.\n",
                      RFileManager::logPrefix.toUtf8().constData(),this->nRunningActions);
        this->remoteRefreshPending = true;
        R_LOG_TRACE_OUT;
        return;
    }

    this->remoteRefreshPending = false;
    try
    {
        this->fileManagerCache->resetRequestListFilesDateTime();
        this->cloudClient->requestListFiles();
        this->nRunningActions++;
    }
    catch (const RError &rError)
    {
        RLogger::error("[%s] Failed to request list of cloud files. %s\n",
                       RFileManager::logPrefix.toUtf8().constData(),
                       rError.getMessage().toUtf8().constData());
    }
    R_LOG_TRACE_OUT;
}

QString RFileManager::findUserName() const
{
    return RTlsTrustStore::findCN(this->cloudClient->getHttpClientSettings().getTlsKeyStore().getCertificateFile());
}

void RFileManager::storeRemoteFile(const RFileInfo &fileInfo)
{
    if (fileInfo.getId().isNull())
    {
        return;
    }
    for (RFileInfo &remoteFileInfo : this->remoteFiles)
    {
        if (remoteFileInfo.getId() == fileInfo.getId())
        {
            remoteFileInfo = fileInfo;
            return;
        }
    }
    this->remoteFiles.append(fileInfo);
}

void RFileManager::removeRemoteFile(const QUuid &id)
{
    this->remoteFiles.removeIf([&id](const RFileInfo &fileInfo) { return fileInfo.getId() == id; });
}

void RFileManager::finishRunningAction()
{
    if (this->nRunningActions > 0)
    {
        this->nRunningActions--;
    }
    if (this->nRunningActions == 0)
    {
        emit this->syncFilesCompleted();
        if (this->remoteRefreshPending)
        {
            this->requestRemoteRefresh();
        }
    }
}

void RFileManager::syncFiles()
{
    R_LOG_TRACE_IN;
    this->filesToSync.mutex.lock();

    QDir localDirectory(this->fileManagerSettings.getLocalDirectory());

    try
    {
        // Remove local
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localRemove))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("[%s] File remove (local): \"%s\" (id: %s)\n",
                          RFileManager::logPrefix.toUtf8().constData(),
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            if (QFile::remove(filePath))
            {
                RLogger::info("[%s] Local file \"%s\" has been removed.\n",
                              RFileManager::logPrefix.toUtf8().constData(),
                              filePath.toUtf8().constData());
                emit this->fileRemoved(filePath);
            }
        }
        // Remove remote
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteRemove))
        {
            RLogger::info("[%s] File remove (remote): \"%s\" (id: %s)\n",
                          RFileManager::logPrefix.toUtf8().constData(),
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileRemove(fileInfo.getId());
            this->nRunningActions++;
        }
        // Download
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteDownload))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("[%s] File download: \"%s\" (id: %s)\n",
                          RFileManager::logPrefix.toUtf8().constData(),
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileDownload(filePath,fileInfo.getId());
            this->nRunningActions++;
        }
        // Update
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpdate))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("[%s] File update: \"%s\" (id: %s)\n",
                          RFileManager::logPrefix.toUtf8().constData(),
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileUpdate(filePath,fileInfo.getPath(),fileInfo.getId());
            this->nRunningActions++;
        }
        // Upload
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpload))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("[%s] File upload: \"%s\"\n",
                          RFileManager::logPrefix.toUtf8().constData(),
                          fileInfo.getPath().toUtf8().constData());
            // Replace instead of upload - cloud removes any other file of the same name owned by the user,
            // therefore a single file can never end up being stored more than once.
            this->cloudClient->requestFileReplace(filePath,fileInfo.getPath());
            this->nRunningActions++;
        }

        this->fileManagerCache->write(this->fileManagerSettings.getCacheFile());
    }
    catch (const RError &rError)
    {
        RLogger::error("[%s] Sync has failed. %s\n",
                       RFileManager::logPrefix.toUtf8().constData(),
                       rError.getMessage().toUtf8().constData());
    }

    this->filesToSync.mutex.unlock();
    R_LOG_TRACE_OUT;
}

void RFileManager::compareFileLists()
{
    R_LOG_TRACE_IN;
    QString userName = this->findUserName();

    // Compare filename and modification time of both local and remote file lists.

    this->filesToSync.mutex.lock();

    this->filesToSync.localUpload.clear();
    this->filesToSync.localUpdate.clear();
    this->filesToSync.localRemove.clear();
    this->filesToSync.remoteDownload.clear();
    this->filesToSync.remoteRemove.clear();

    // Remote files owned by the user, one per file name (the most recently updated one).
    // All other remote files with the same name are duplicates and are removed.
    QMap<QString,RFileInfo> userRemoteFiles;
    for (const RFileInfo &remoteFileInfo : std::as_const(this->remoteFiles))
    {
        if (remoteFileInfo.getAccessRights().getOwner().getUser() != userName)
        {
            continue;
        }

        auto it = userRemoteFiles.find(remoteFileInfo.getPath());
        if (it == userRemoteFiles.end())
        {
            userRemoteFiles.insert(remoteFileInfo.getPath(),remoteFileInfo);
        }
        else if (remoteFileInfo.getUpdateDateTime() > it.value().getUpdateDateTime())
        {
            this->filesToSync.remoteRemove.append(it.value());
            it.value() = remoteFileInfo;
        }
        else
        {
            this->filesToSync.remoteRemove.append(remoteFileInfo);
        }
    }

    for (const QFileInfo &fileInfo : std::as_const(this->localFiles))
    {
        auto it = userRemoteFiles.constFind(fileInfo.fileName());
        if (it != userRemoteFiles.constEnd())
        {
            const RFileInfo &remoteFileInfo = it.value();

            QByteArray localCheckSum = RFileInfo::findMd5Checksum(fileInfo.absoluteFilePath());
            qint64 localLastModified = fileInfo.lastModified().toSecsSinceEpoch();

            if (localCheckSum != remoteFileInfo.getMd5Checksum())
            {
                if (localLastModified < remoteFileInfo.getUpdateDateTime())
                {
                    this->filesToSync.remoteDownload.append(remoteFileInfo);
                }
                else
                {
                    this->filesToSync.localUpdate.append(remoteFileInfo);
                }
            }
        }
        else
        {
            RFileInfo localFileInfo = RFileInfo::fromQFileInfo(fileInfo,false);
            localFileInfo.setTags(this->fileManagerSettings.getFileTags());
            RAccessOwner accessOwner;
            accessOwner.setUser(userName);
            accessOwner.setGroup(userName);
            RAccessRights accessRights = localFileInfo.getAccessRights();
            accessRights.setOwner(accessOwner);
            localFileInfo.setAccessRights(accessRights);

            if (localFileInfo.getUpdateDateTime() > this->fileManagerCache->getRemoteUpdateDateTime())
            {
                this->filesToSync.localUpload.append(localFileInfo);
            }
            else
            {
                if (localFileInfo.getUpdateDateTime() < this->fileManagerCache->getRequestListFilesDateTime())
                {
                    this->filesToSync.localRemove.append(localFileInfo);
                }
            }
        }
    }

    for (const RFileInfo &remoteFileInfo : std::as_const(userRemoteFiles))
    {
        bool localFileFound = false;
        for (const QFileInfo &fileInfo : std::as_const(this->localFiles))
        {
            if (fileInfo.fileName() == remoteFileInfo.getPath())
            {
                localFileFound = true;
                break;
            }
        }
        if (!localFileFound)
        {
            if (this->fileManagerCache->getLocalUpdateDateTime() > remoteFileInfo.getUpdateDateTime())
            {
                this->filesToSync.remoteRemove.append(remoteFileInfo);
            }
            else
            {
                this->filesToSync.remoteDownload.append(remoteFileInfo);
            }
        }
    }

    RLogger::info("Expected cloud file changes:\n");
    RLogger::indent();
    for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpload))
    {
        RLogger::info("lu: %s (id: %s, md5-checksum: %s)\n",
                      fileInfo.getPath().toUtf8().constData(),
                      fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      fileInfo.getMd5Checksum().constData());
    }
    for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpdate))
    {
        RLogger::info("lU: %s (id: %s, md5-checksum: %s)\n",
                      fileInfo.getPath().toUtf8().constData(),
                      fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      fileInfo.getMd5Checksum().constData());
    }
    for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localRemove))
    {
        RLogger::info("lr: %s (id: %s, md5-checksum: %s)\n",
                      fileInfo.getPath().toUtf8().constData(),
                      fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      fileInfo.getMd5Checksum().constData());
    }
    for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteDownload))
    {
        RLogger::info("rd: %s (id: %s, md5-checksum: %s)\n",
                      fileInfo.getPath().toUtf8().constData(),
                      fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      fileInfo.getMd5Checksum().constData());
    }
    for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteRemove))
    {
        RLogger::info("rr: %s (id: %s, md5-checksum: %s)\n",
                      fileInfo.getPath().toUtf8().constData(),
                      fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      fileInfo.getMd5Checksum().constData());
    }
    RLogger::unindent(false);

    this->filesToSync.mutex.unlock();

    emit this->filesToSyncAvailable();
    R_LOG_TRACE_OUT;
}

RVersion RFileManager::incrementVersion(const RVersion &in)
{
    R_LOG_TRACE_IN;
    RVersion out(in.getMajor(),in.getMinor(),in.getRelease() + 1);
    R_LOG_TRACE_RETURN(out);
}

void RFileManager::onFileListAvailable(QList<RFileInfo> fileInfoList)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] Remote file list is available.\n",RFileManager::logPrefix.toUtf8().constData());

    this->remoteFiles.clear();
    for (const RFileInfo &fileInfo : fileInfoList)
    {
        if (fileInfo.hasTags(this->fileManagerSettings.getFileTags()))
        {
            this->remoteFiles.append(fileInfo);
        }
    }
    if (!this->isRunning)
    {
        R_LOG_TRACE_OUT;
        return;
    }
    // Local directory listing is refreshed together with remote one so both describe the same moment.
    this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());
    this->compareFileLists();
    this->fileManagerCache->resetRemoteUpdateDateTime();
    R_LOG_TRACE_OUT;
}

void RFileManager::onFilesToSyncAvailable()
{
    R_LOG_TRACE_IN;
    this->syncFiles();
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileRemoved(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] Remote file \"%s\" (%s) has been removed.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->removeRemoteFile(fileInfo.getId());
    emit this->fileRemoved(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileDownloaded(QString fileName)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File \"%s\" has been downloaded.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileName.toUtf8().constData());
    emit this->fileDownloaded(fileName);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileUpdated(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File \"%s\" (%s) has been updated.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->storeRemoteFile(fileInfo);
    // Request update version.
    this->cloudClient->requestFileUpdateVersion(RFileManager::incrementVersion(fileInfo.getVersion()),fileInfo.getId());
    this->nRunningActions++;
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileUploaded(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File \"%s\" (%s) has been uploaded.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->storeRemoteFile(fileInfo);
    // Request update version.
    this->cloudClient->requestFileUpdateVersion(RFileManager::incrementVersion(RVersion()),fileInfo.getId());
    this->nRunningActions++;
    // Request update tags.
    this->cloudClient->requestFileUpdateTags(this->fileManagerSettings.getFileTags(),fileInfo.getId());
    this->nRunningActions++;
    emit this->fileUploaded(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileReplaced(std::tuple<RFileInfo, QList<RFileInfo> > fileInfoList)
{
    R_LOG_TRACE_IN;
    const RFileInfo &fileInfo = std::get<0>(fileInfoList);
    const QList<RFileInfo> &replacedFileInfoList = std::get<1>(fileInfoList);

    RLogger::info("[%s] File \"%s\" (%s) has been replaced.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());

    // Replaced file continues the version line of files it replaced.
    RVersion version;
    for (const RFileInfo &replacedFileInfo : replacedFileInfoList)
    {
        RLogger::info("[%s] Replaced file \"%s\" (%s) has been removed.\n",
                      RFileManager::logPrefix.toUtf8().constData(),
                      replacedFileInfo.getPath().toUtf8().constData(),
                      replacedFileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
        if (replacedFileInfo.getVersion() > version)
        {
            version = replacedFileInfo.getVersion();
        }
        this->removeRemoteFile(replacedFileInfo.getId());
    }
    this->storeRemoteFile(fileInfo);

    // Request update version.
    this->cloudClient->requestFileUpdateVersion(RFileManager::incrementVersion(version),fileInfo.getId());
    this->nRunningActions++;
    // Request update tags.
    this->cloudClient->requestFileUpdateTags(this->fileManagerSettings.getFileTags(),fileInfo.getId());
    this->nRunningActions++;
    emit this->fileUploaded(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileVersionUpdated(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File version \"%s\" (%s) has been updated.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->storeRemoteFile(fileInfo);
    emit this->fileVersionUpdated(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileTagsUpdated(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File tags \"%s\" (%s) have been updated.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->storeRemoteFile(fileInfo);
    emit this->fileTagsUpdated(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onCloudActionFinished()
{
    R_LOG_TRACE_IN;
    this->finishRunningAction();
    R_LOG_TRACE_OUT;
}

void RFileManager::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    R_LOG_TRACE_IN;
    emit this->cloudError(errorType,errorMessage,message);
    this->finishRunningAction();
    R_LOG_TRACE_OUT;
}

void RFileManager::onLocalDirectoryChanged(const QString &path)
{
    R_LOG_TRACE_IN;
    Q_UNUSED(path);
    if (this->isRunning)
    {
        // Wait until the directory settles - one save produces many notifications.
        this->localChangeTimer->start();
    }
    R_LOG_TRACE_OUT;
}

void RFileManager::onLocalChangeTimeout()
{
    R_LOG_TRACE_IN;
    if (this->isRunning)
    {
        RLogger::info("[%s] One or more files in local directory have changed.\n",
                      RFileManager::logPrefix.toUtf8().constData());
        this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());
        this->fileManagerCache->resetLocalUpdateDateTime();
        // Local files are compared against fresh remote file list once it arrives.
        this->requestRemoteRefresh();
    }
    R_LOG_TRACE_OUT;
}

void RFileManager::onRemoteRefreshTimeout()
{
    R_LOG_TRACE_IN;
    if (this->nRunningActions > 0)
    {
        RLogger::info("[%s] Skipping sync files because there are %u running actions (previous sync is still running).\n",
                       RFileManager::logPrefix.toUtf8().constData(),this->nRunningActions);
    }
    else
    {
        this->requestRemoteRefresh();
    }
    R_LOG_TRACE_OUT;
}
