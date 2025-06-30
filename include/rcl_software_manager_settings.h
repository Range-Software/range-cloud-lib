#ifndef RCL_SOFTWARE_MANAGER_SETTINGS_H
#define RCL_SOFTWARE_MANAGER_SETTINGS_H

#include <QStringList>

#include "rcl_http_client_settings.h"

class RSoftwareManagerSettings
{

    protected:

        //! Http client settings.
        RHttpClientSettings httpClientSettings;
        //! File tags.
        QStringList fileTags;

    private:

        //! Internal initialization function.
        void _init(const RSoftwareManagerSettings *pSettings = nullptr);

    public:

        //! Constructor.
        RSoftwareManagerSettings();

        //! Copy constructor.
        RSoftwareManagerSettings(const RSoftwareManagerSettings &settings);

        //! Destructor.
        ~RSoftwareManagerSettings();

        //! Assignment operator.
        RSoftwareManagerSettings &operator =(const RSoftwareManagerSettings &settings);

        //! Returns const reference to session info.
        const RHttpClientSettings & getHttpClientSettings() const;

        //! Sets new session info.
        void setHttpClientSettings(const RHttpClientSettings &httpClientSettings);

        //! Returns const reference to file tag list.
        const QStringList & getFileTags() const;

        //! Sets new file tags.
        void setFileTags(const QStringList &fileTags);

};

#endif // RCL_SOFTWARE_MANAGER_SETTINGS_H
