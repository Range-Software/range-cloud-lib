#ifndef RCL_SOFTWARE_MANAGER_H
#define RCL_SOFTWARE_MANAGER_H

#include <QObject>

#include "rcl_cloud_client.h"
#include "rcl_software_manager_settings.h"

class RSoftwareManager : public QObject
{
    Q_OBJECT

    protected:

        //! Software manager settings.
        RSoftwareManagerSettings softwareManagerSettings;
        //! Cloud cloudClient.
        RCloudClient *cloudClient;

    public:

        //! Constructor.
        explicit RSoftwareManager(const RSoftwareManagerSettings &softwareManagerSettings, QObject *parent = nullptr);

        //! Get const reference to software manager settings.
        const RSoftwareManagerSettings &getSoftwareManagerSettings() const;

        //! Set new software manager settings.
        void setSoftwareManagerSettings(const RSoftwareManagerSettings &softwareManagerSettings);

        //! Return pointer to cloud client.
        RCloudClient *getCloudClient();

        //! Check for updates.
        void checkForUpdates();

        //! Download file.
        RToolTask *downloadFile(const QString &path, const QUuid &id);

        //! Get list of expected tags.
        static QStringList getExpectedTags();

    protected slots:

        //! File list is available.
        void onFileListAvailable(QList<RFileInfo> fileInfoList);

        //! File has been downloaded.
        void onFileDownloaded(QString fileName);

        //! Cloud action has finished.
        void onCloudActionFinished();

        //! Cloud action has failed.
        void onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

    signals:

        //! Http client settings changed.
        void httpClientSettingsChanged(const RHttpClientSettings &clientSettings);

        //! Software is available.
        void softwareAvailable(QList<RFileInfo> fileInfoList);

        //! Software is downloaded.
        void softwareDownloaded(const QString &fileName);

        //! Cloud error has occured.
        void cloudError(RError::Type errorType, const QString &errorMessage, const QString &message);

};

#endif // RCL_SOFTWARE_MANAGER_H
