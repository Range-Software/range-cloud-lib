#ifndef RCL_FILE_MANAGER_SETTINGS_H
#define RCL_FILE_MANAGER_SETTINGS_H

#include <QStringList>

class RFileManagerSettings
{

    protected:

        //! File tags.
        QStringList fileTags;
        //! Local directory.
        QString localDirectory;
        //! Cache file.
        QString cacheFile;

    private:

        //! Internal initialization function.
        void _init(const RFileManagerSettings *pSettings = nullptr);

    public:

        //! Constructor.
        RFileManagerSettings();

        //! Copy constructor.
        RFileManagerSettings(const RFileManagerSettings &settings);

        //! Destructor.
        ~RFileManagerSettings();

        //! Assignment operator.
        RFileManagerSettings &operator =(const RFileManagerSettings &settings);

        //! Returns const reference to file tag list.
        const QStringList & getFileTags() const;

        //! Sets new file tags.
        void setFileTags(const QStringList &fileTags);

        //! Returns const reference to local directory path.
        const QString & getLocalDirectory() const;

        //! Sets new local directory path.
        void setLocalDirectory(const QString &localDirectory);

        //! Returns const reference to cache file path.
        const QString & getCacheFile() const;

        //! Sets new path to cache file.
        void setCacheFile(const QString &cacheFile);

};

#endif // RCL_FILE_MANAGER_SETTINGS_H
