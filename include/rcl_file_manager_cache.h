#ifndef RCL_FILE_MANAGER_CACHE_H
#define RCL_FILE_MANAGER_CACHE_H

#include <QObject>

class RFileManagerCache : public QObject
{

    Q_OBJECT

    protected:

        //! Last local update time when chache was modified.
        qint64 localUpdateDateTime;
        //! Last remote update time when chache was modified.
        qint64 remoteUpdateDateTime;
        //! Last list file request time.
        qint64 requestListFilesDateTime;

    public:

        //! Constructor.
        explicit RFileManagerCache(QObject *parent = nullptr);

        //! Read from file.
        void read(const QString &fileName);

        //! Write to file.
        void write(const QString &fileName) const;

        //! Return last local update time.
        qint64 getLocalUpdateDateTime() const;

        //! Return last remote update time.
        qint64 getRemoteUpdateDateTime() const;

        //! Return last request list files time.
        qint64 getRequestListFilesDateTime() const;

        //! Clear cache.
        void clear();

        //! Set current date time.
        void resetCurrentDateTime();

        //! Set current local update time.
        void resetLocalUpdateDateTime();

        //! Set current remote update time.
        void resetRemoteUpdateDateTime();

        //! Set current request list files time.
        void resetRequestListFilesDateTime();

    private:

        //! Retrieve cache content from Json object.
        void fromJson(const QJsonObject &json);

        //! Export cache content to Json object.
        QJsonObject toJson() const;

};

#endif // RCL_FILE_MANAGER_CACHE_H
