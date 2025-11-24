#include "rcl_file_manager_settings.h"

void RFileManagerSettings::_init(const RFileManagerSettings *pSettings)
{
    if (pSettings)
    {
        this->fileTags = pSettings->fileTags;
        this->localDirectory = pSettings->localDirectory;
        this->cacheFile = pSettings->cacheFile;
    }
}

RFileManagerSettings::RFileManagerSettings()
{
    this->_init();
}

RFileManagerSettings::RFileManagerSettings(const RFileManagerSettings &settings)
{
    this->_init(&settings);
}

RFileManagerSettings::~RFileManagerSettings()
{

}

RFileManagerSettings &RFileManagerSettings::operator =(const RFileManagerSettings &settings)
{
    this->_init(&settings);
    return (*this);
}

const QStringList &RFileManagerSettings::getFileTags() const
{
    return this->fileTags;
}

void RFileManagerSettings::setFileTags(const QStringList &fileTags)
{
    this->fileTags = fileTags;
}

const QString &RFileManagerSettings::getLocalDirectory() const
{
    return this->localDirectory;
}

void RFileManagerSettings::setLocalDirectory(const QString &localDirectory)
{
    this->localDirectory = localDirectory;
}

const QString &RFileManagerSettings::getCacheFile() const
{
    return this->cacheFile;
}

void RFileManagerSettings::setCacheFile(const QString &cacheFile)
{
    this->cacheFile = cacheFile;
}
