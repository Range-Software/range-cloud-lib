#include <rbl_logger.h>
#include <rbl_utils.h>

#include <QDir>

#include "rcl_file_manager.h"
#include "rcl_file_tools.h"

const QString RFileManager::logPrefix = "[FileManager]";

RFileManager::RFileManager(const RFileManagerSettings &fileManagerSettings, RCloudClient *cloudClient, QObject *parent)
    : QObject{parent}
    , fileManagerSettings{fileManagerSettings}
    , cloudClient{cloudClient}
    , nRunningActions{0}
{
    this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());

    this->fileManagerCache = new RFileManagerCache(this);

    this->initializeCacheFile();

    this->localFileSystemWatcher = new QFileSystemWatcher;
    if (!this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
    {
        RLogger::warning("Could not add path \'%s\' to file system watcher\n",this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
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
}

const RFileManagerSettings &RFileManager::getFileManagerSettings() const
{
    return this->fileManagerSettings;
}

void RFileManager::setFileManagerSettings(const RFileManagerSettings &fileManagerSettings)
{
    // Set new sync directory
    QString oldLocalDirectory = this->fileManagerSettings.getLocalDirectory();
    this->fileManagerSettings = fileManagerSettings;
    if (oldLocalDirectory != this->fileManagerSettings.getLocalDirectory())
    {
        this->localFileSystemWatcher->removePath(oldLocalDirectory);
        if (this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
        {
            RLogger::warning("Could not add path \'%s\' to local file system watcher\n",this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
        }
    }
}

RCloudClient *RFileManager::getCloudClient()
{
    return this->cloudClient;
}

uint RFileManager::getNRunningActions() const
{
    return this->nRunningActions;
}

bool RFileManager::requestSyncFiles()
{
    if (this->nRunningActions > 0)
    {
        RLogger::error("Cannot sync files because there are %u running actions (previous sync is still running).\n",this->nRunningActions);
        return false;
    }

    try
    {
        this->cloudClient->requestListFiles();
        this->nRunningActions++;
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of cloud files. %s\n",rError.getMessage().toUtf8().constData());
        return false;
    }
    return true;
}

void RFileManager::initializeCacheFile()
{
    if (QFile::exists(this->fileManagerSettings.getCacheFile()))
    {
        this->fileManagerCache->read(this->fileManagerSettings.getCacheFile());
    }
}

void RFileManager::syncFiles()
{
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
            RLogger::info("File remove (local): \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            QFile::remove(filePath);
        }
        // Remove remote
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteRemove))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File download: \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileRemove(fileInfo.getId());
            this->nRunningActions++;
        }
        // Download
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.remoteDownload))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File download: \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileDownload(filePath,fileInfo.getId());
            this->nRunningActions++;
        }
        // Update
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpdate))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File update: \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileUpdate(filePath,fileInfo.getPath(),fileInfo.getId());
            this->nRunningActions++;
        }
        // Upload
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.localUpload))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File upload: \"%s\"\n",
                          fileInfo.getPath().toUtf8().constData());
            this->cloudClient->requestFileUpload(filePath,fileInfo.getPath());
            this->nRunningActions++;
        }

        this->fileManagerCache->resetCurrentDateTime();
        this->fileManagerCache->write(this->fileManagerSettings.getCacheFile());
    }
    catch (const RError &rError)
    {
        RLogger::error("Sync has failed. %s\n",rError.getMessage().toUtf8().constData());
    }

    this->localFileSystemWatcher->blockSignals(localFileSystemWatcherSignalsBlocked);

    this->filesToSync.mutex.unlock();
}

void RFileManager::compareFileLists()
{
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
}

void RFileManager::onFileListAvailable(QList<RFileInfo> fileInfoList)
{
    RLogger::info("Cloud file list is available.\n");

    this->remoteFiles.clear();
    for (const RFileInfo &fileInfo : fileInfoList)
    {
        if (fileInfo.hasTags(this->fileManagerSettings.getFileTags()))
        {
            this->remoteFiles.append(fileInfo);
        }
    }
    this->compareFileLists();
}

void RFileManager::onFilesToSyncAvailable()
{
    this->syncFiles();
}

void RFileManager::onFileRemoved(RFileInfo fileInfo)
{
    RLogger::info("File \"%s\" (%s) has been removed.\n",
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    emit this->fileUpdated(fileInfo);
}

void RFileManager::onFileDownloaded(QString fileName)
{
    RLogger::info("File \"%s\" has been downloaded.\n",fileName.toUtf8().constData());
    emit this->fileDownloaded(fileName);
}

void RFileManager::onFileUpdated(RFileInfo fileInfo)
{
    RLogger::info("File \"%s\" (%s) has been updated.\n",
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    emit this->fileUpdated(fileInfo);
}

void RFileManager::onFileUploaded(RFileInfo fileInfo)
{
    RLogger::info("File \"%s\" (%s) has been uploaded.\n",
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->cloudClient->requestFileUpdateTags(this->fileManagerSettings.getFileTags(),fileInfo.getId());
    this->nRunningActions++;
}

void RFileManager::onFileTagsUpdated(RFileInfo fileInfo)
{
    RLogger::info("File tags \"%s\" (%s) has been uploaded.\n",
                  fileInfo.getPath().toUtf8().constData(),
                  fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    emit this->fileUploaded(fileInfo);
}

void RFileManager::onCloudActionFinished()
{
    this->nRunningActions--;
    if (this->nRunningActions == 0)
    {
        emit this->syncFilesCompleted();
    }
}

void RFileManager::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    this->nRunningActions--;
    emit this->cloudError(errorType,errorMessage,message);
    if (this->nRunningActions == 0)
    {
        emit this->syncFilesCompleted();
    }
}

void RFileManager::onLocalDirectoryChanged(const QString &path)
{
    RLogger::info("One or more files in local directory have changed.\n");
    this->localFiles = RFileTools::listFiles(path);
    this->compareFileLists();
}
