#include "rcl_software_manager_settings.h"

void RSoftwareManagerSettings::_init(const RSoftwareManagerSettings *pSettings)
{
    if (pSettings)
    {
        this->httpClientSettings = pSettings->httpClientSettings;
        this->fileTags = pSettings->fileTags;
    }
}

RSoftwareManagerSettings::RSoftwareManagerSettings()
{
    this->_init();
}

RSoftwareManagerSettings::RSoftwareManagerSettings(const RSoftwareManagerSettings &settings)
{
    this->_init(&settings);
}

RSoftwareManagerSettings::~RSoftwareManagerSettings()
{

}

RSoftwareManagerSettings &RSoftwareManagerSettings::operator =(const RSoftwareManagerSettings &settings)
{
    this->_init(&settings);
    return (*this);
}

const RHttpClientSettings &RSoftwareManagerSettings::getHttpClientSettings() const
{
    return this->httpClientSettings;
}

void RSoftwareManagerSettings::setHttpClientSettings(const RHttpClientSettings &httpClientSettings)
{
    this->httpClientSettings = httpClientSettings;
}

const QStringList &RSoftwareManagerSettings::getFileTags() const
{
    return this->fileTags;
}

void RSoftwareManagerSettings::setFileTags(const QStringList &fileTags)
{
    this->fileTags = fileTags;
}
