#include <rbl_logger.h>
#include <rbl_utils.h>

#include <QDir>

#include "rcl_file_manager.h"
#include "rcl_file_tools.h"

const QString RFileManager::logPrefix = "LocalFileManager";

RFileManager::RFileManager(const RFileManagerSettings &fileManagerSettings, RCloudClient *cloudClient, QObject *parent)
    : QObject{parent}
    , fileManagerSettings{fileManagerSettings}
    , cloudClient{cloudClient}
    , nRunningActions{0}
{
    R_LOG_TRACE_IN;
    this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());

    this->fileManagerCache = new RFileManagerCache(this);

    this->initializeCacheFile();

    this->localFileSystemWatcher = new QFileSystemWatcher;
    if (!this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
    {
        RLogger::warning("[%s] Could not add path \'%s\' to file system watcher\n",
                         RFileManager::logPrefix.toUtf8().constData(),
                         this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
    }

    QObject::connect(this->localFileSystemWatcher,&QFileSystemWatcher::directoryChanged,this,&RFileManager::onLocalDirectoryChanged);

    QObject::connect(this->cloudClient,&RCloudClient::fileListAvailable,this,&RFileManager::onFileListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::fileRemoved,this,&RFileManager::onFileRemoved);
    QObject::connect(this->cloudClient,&RCloudClient::fileDownloaded,this,&RFileManager::onFileDownloaded);
    QObject::connect(this->cloudClient,&RCloudClient::fileUpdated,this,&RFileManager::onFileUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileUploaded,this,&RFileManager::onFileUploaded);
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
        if (this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
        {
            RLogger::warning("[%s] Could not add path \'%s\' to local file system watcher\n",
                             RFileManager::logPrefix.toUtf8().constData(),
                             this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
        }
    }
    R_LOG_TRACE_OUT;
}

RCloudClient *RFileManager::getCloudClient()
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->cloudClient);
}

uint RFileManager::getNRunningActions() const
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->nRunningActions);
}

bool RFileManager::requestSyncFiles()
{
    R_LOG_TRACE_IN;
    if (this->nRunningActions > 0)
    {
        RLogger::error("[%s] Cannot sync files because there are %u running actions (previous sync is still running).\n",
                       RFileManager::logPrefix.toUtf8().constData(),this->nRunningActions);
        R_LOG_TRACE_RETURN(false);
    }

    try
    {
        this->cloudClient->requestListFiles();
        this->nRunningActions++;
    }
    catch (const RError &rError)
    {
        RLogger::error("[%s] Failed to request list of cloud files. %s\n",
                       RFileManager::logPrefix.toUtf8().constData(),
                       rError.getMessage().toUtf8().constData());
        R_LOG_TRACE_RETURN(false);
    }
    R_LOG_TRACE_RETURN(true);
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

void RFileManager::syncFiles()
{
    R_LOG_TRACE_IN;
    this->filesToSync.mutex.lock();

    bool localFileSystemWatcherSignalsBlocked = this->localFileSystemWatcher->signalsBlocked();
    this->localFileSystemWatcher->blockSignals(true);

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
            QFile::remove(filePath);
        }
        // Remove remote
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteRemove))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("[%s] File download: \"%s\" (id: %s)\n",
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
            this->cloudClient->requestFileUpload(filePath,fileInfo.getPath());
            this->nRunningActions++;
        }

        this->fileManagerCache->resetCurrentDateTime();
        this->fileManagerCache->write(this->fileManagerSettings.getCacheFile());
    }
    catch (const RError &rError)
    {
        RLogger::error("[%s] Sync has failed. %s\n",
                       RFileManager::logPrefix.toUtf8().constData(),
                       rError.getMessage().toUtf8().constData());
    }

    this->localFileSystemWatcher->blockSignals(localFileSystemWatcherSignalsBlocked);

    this->filesToSync.mutex.unlock();
    R_LOG_TRACE_OUT;
}

void RFileManager::compareFileLists()
{
    R_LOG_TRACE_IN;
    QString userName = RTlsTrustStore::findCN(this->cloudClient->getHttpClientSettings().getTlsKeyStore().getCertificateFile());

    // Compare filename and modification time of both local and remote file lists.

    this->filesToSync.mutex.lock();

    this->filesToSync.localUpload.clear();
    this->filesToSync.localUpdate.clear();
    this->filesToSync.localRemove.clear();
    this->filesToSync.remoteDownload.clear();
    this->filesToSync.remoteRemove.clear();

    for (const QFileInfo &fileInfo : std::as_const(this->localFiles))
    {
        QByteArray localCheckSum = RFileInfo::findMd5Checksum(fileInfo.absoluteFilePath());
        qint64 localLastModified = fileInfo.lastModified().toSecsSinceEpoch();

        bool remoteFileFound = false;
        for (const RFileInfo &remoteFileInfo : std::as_const(this->remoteFiles))
        {
            if (fileInfo.fileName() == remoteFileInfo.getPath())
            {
                if (remoteFileInfo.getAccessRights().getOwner().getUser() != userName)
                {
                    continue;
                }

                remoteFileFound = true;

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
        }
        if (!remoteFileFound)
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
                this->filesToSync.localRemove.append(localFileInfo);
            }
        }
    }

    for (const RFileInfo &remoteFileInfo : std::as_const(this->remoteFiles))
    {
        bool localFileFound = false;
        for (const QFileInfo &fileInfo : std::as_const(this->localFiles))
        {
            if (fileInfo.fileName() == remoteFileInfo.getPath() &&
                remoteFileInfo.getAccessRights().getOwner().getUser() == userName)
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
        }
    }

    this->filesToSync.mutex.unlock();

    emit this->filesToSyncAvailable();
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileListAvailable(QList<RFileInfo> fileInfoList)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] Cloud file list is available.\n",RFileManager::logPrefix.toUtf8().constData());

    this->remoteFiles.clear();
    for (const RFileInfo &fileInfo : fileInfoList)
    {
        if (fileInfo.hasTags(this->fileManagerSettings.getFileTags()))
        {
            this->remoteFiles.append(fileInfo);
        }
    }
    this->compareFileLists();
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
    RLogger::info("[%s] File \"%s\" (%s) has been removed.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    emit this->fileUpdated(fileInfo);
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
    emit this->fileUpdated(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileUploaded(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File \"%s\" (%s) has been uploaded.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->cloudClient->requestFileUpdateTags(this->fileManagerSettings.getFileTags(),fileInfo.getId());
    this->nRunningActions++;
    R_LOG_TRACE_OUT;
}

void RFileManager::onFileTagsUpdated(RFileInfo fileInfo)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] File tags \"%s\" (%s) has been uploaded.\n",
                  RFileManager::logPrefix.toUtf8().constData(),
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    emit this->fileUploaded(fileInfo);
    R_LOG_TRACE_OUT;
}

void RFileManager::onCloudActionFinished()
{
    R_LOG_TRACE_IN;
    this->nRunningActions--;
    if (this->nRunningActions == 0)
    {
        emit this->syncFilesCompleted();
    }
    R_LOG_TRACE_OUT;
}

void RFileManager::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    R_LOG_TRACE_IN;
    this->nRunningActions--;
    emit this->cloudError(errorType,errorMessage,message);
    if (this->nRunningActions == 0)
    {
        emit this->syncFilesCompleted();
    }
    R_LOG_TRACE_OUT;
}

void RFileManager::onLocalDirectoryChanged(const QString &path)
{
    R_LOG_TRACE_IN;
    RLogger::info("[%s] One or more files in local directory have changed.\n",
                  RFileManager::logPrefix.toUtf8().constData());
    this->localFiles = RFileTools::listFiles(path);
    this->compareFileLists();
    R_LOG_TRACE_OUT;
}
