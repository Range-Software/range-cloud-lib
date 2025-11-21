#include <rbl_logger.h>
#include <rbl_utils.h>

#include <QDir>

#include "rcl_file_manager.h"
#include "rcl_file_tools.h"

RFileManager::RFileManager(const RFileManagerSettings &fileManagerSettings, RCloudClient *cloudClient, QObject *parent)
    : QObject{parent}
    , fileManagerSettings{fileManagerSettings}
    , cloudClient{cloudClient}
{
    this->localFiles = RFileTools::listFiles(this->fileManagerSettings.getLocalDirectory());

    this->localFileSystemWatcher = new QFileSystemWatcher;
    if (!this->localFileSystemWatcher->addPath(this->fileManagerSettings.getLocalDirectory()))
    {
        RLogger::warning("Could not add path \'%s\' to file system watcher\n",this->fileManagerSettings.getLocalDirectory().toUtf8().constData());
    }

    QObject::connect(this->localFileSystemWatcher,&QFileSystemWatcher::directoryChanged,this,&RFileManager::onLocalDirectoryChanged);

    QObject::connect(this->cloudClient,&RCloudClient::fileListAvailable,this,&RFileManager::onFileListAvailable);
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

void RFileManager::requestSyncFiles()
{
    try
    {
        this->cloudClient->requestListFiles();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of cloud files. %s\n",rError.getMessage().toUtf8().constData());
    }
}

void RFileManager::syncFiles()
{
    this->filesToSync.mutex.lock();

    QDir localDirectory(this->fileManagerSettings.getLocalDirectory());

    try
    {
        // Download
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.download))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File download: \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileDownload(filePath,fileInfo.getId());
        }
        // Update
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.update))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File update: \"%s\" (id: %s)\n",
                          fileInfo.getPath().toUtf8().constData(),
                          fileInfo.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            this->cloudClient->requestFileUpdate(filePath,fileInfo.getPath(),fileInfo.getId());
        }
        // Upload
        for (const RFileInfo &fileInfo : std::as_const(this->filesToSync.upload))
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(fileInfo.getPath()).fileName());
            RLogger::info("File upload: \"%s\"\n",
                          fileInfo.getPath().toUtf8().constData());
            this->cloudClient->requestFileUpload(filePath,fileInfo.getPath());
        }
    }
    catch (const RError &rError)
    {
        RLogger::error("Sync has failed. %s\n",rError.getMessage().toUtf8().constData());
    }

    this->filesToSync.mutex.unlock();
}

RToolTask *RFileManager::downloadFile(const QString &path, const QUuid &id)
{
    return this->cloudClient->requestFileDownload(path,id);
}

void RFileManager::compareFileLists()
{
    QString userName = RTlsTrustStore::findCN(this->cloudClient->getHttpClientSettings().getTlsKeyStore().getCertificateFile());

    // Compare filename and modification time of both local and remote file lists.

    this->filesToSync.mutex.lock();

    this->filesToSync.upload.clear();
    this->filesToSync.update.clear();
    this->filesToSync.download.clear();

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
                        this->filesToSync.download.append(remoteFileInfo);
                    }
                    else
                    {
                        this->filesToSync.update.append(remoteFileInfo);
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

            this->filesToSync.upload.append(localFileInfo);
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

void RFileManager::onFileDownloaded(QString fileName)
{
    emit this->fileDownloaded(fileName);
}

void RFileManager::onFileUpdated(RFileInfo fileInfo)
{
    emit this->fileUpdated(fileInfo);
}

void RFileManager::onFileUploaded(RFileInfo fileInfo)
{
    this->cloudClient->requestFileUpdateTags(this->fileManagerSettings.getFileTags(),fileInfo.getId());
}

void RFileManager::onFileTagsUpdated(RFileInfo fileInfo)
{
    emit this->fileUploaded(fileInfo);
}

void RFileManager::onCloudActionFinished()
{

}

void RFileManager::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    emit this->cloudError(errorType,errorMessage,message);
}

void RFileManager::onLocalDirectoryChanged(const QString &path)
{
    RLogger::info("One or more files in local directory have changed.\n");
    this->localFiles = RFileTools::listFiles(path);
    this->compareFileLists();
}
