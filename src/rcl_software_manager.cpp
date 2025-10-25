#include <rbl_utils.h>

#include "rcl_software_manager.h"

RSoftwareManager::RSoftwareManager(const RSoftwareManagerSettings &softwareManagerSettings, QObject *parent)
    : QObject{parent}
    , softwareManagerSettings{softwareManagerSettings}
{
    this->cloudClient = new RCloudClient(RHttpClient::Public,this->softwareManagerSettings.getHttpClientSettings(),parent);

    QObject::connect(this,&RSoftwareManager::httpClientSettingsChanged,this->cloudClient,&RCloudClient::setHttpClientSettings);
    QObject::connect(this->cloudClient,&RCloudClient::fileListAvailable,this,&RSoftwareManager::onFileListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::fileDownloaded,this,&RSoftwareManager::onFileDownloaded);
    QObject::connect(this->cloudClient,&RCloudClient::actionFinished,this,&RSoftwareManager::onCloudActionFinished);
    QObject::connect(this->cloudClient,&RCloudClient::actionFailed,this,&RSoftwareManager::onCloudActionFailed);
}

const RSoftwareManagerSettings &RSoftwareManager::getSoftwareManagerSettings() const
{
    return this->softwareManagerSettings;
}

void RSoftwareManager::setSoftwareManagerSettings(const RSoftwareManagerSettings &softwareManagerSettings)
{
    this->softwareManagerSettings = softwareManagerSettings;
    emit this->httpClientSettingsChanged(this->softwareManagerSettings.getHttpClientSettings());
}

RCloudClient *RSoftwareManager::getCloudClient()
{
    return this->cloudClient;
}

void RSoftwareManager::checkForUpdates()
{
    this->cloudClient->requestListFiles();
}

RToolTask *RSoftwareManager::downloadFile(const QString &path, const QUuid &id)
{
    return this->cloudClient->requestFileDownload(path,id);
}

QStringList RSoftwareManager::getExpectedTags()
{
    return QStringList() << QSysInfo::kernelType() << QSysInfo::currentCpuArchitecture() << RVendor::packageName();
}

void RSoftwareManager::onFileListAvailable(QList<RFileInfo> fileInfoList)
{
    QList<RFileInfo> availableFileInfoList;
    for (const RFileInfo &fileInfo : fileInfoList)
    {
        if (fileInfo.hasTags(this->softwareManagerSettings.getFileTags()))
        {
            availableFileInfoList.append(fileInfo);
        }
    }
    emit this->softwareAvailable(availableFileInfoList);
}

void RSoftwareManager::onFileDownloaded(QString fileName)
{
    emit this->softwareDownloaded(fileName);
}

void RSoftwareManager::onCloudActionFinished()
{

}

void RSoftwareManager::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    emit this->cloudError(errorType,errorMessage,message);
}
