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

    public:

        //! Constructor.
        explicit RFileManagerCache(QObject *parent = nullptr);

        //! Read from file.
        void read(const QString &fileName);

        //! Write to file.
        void write(const QString &fileName) const;

        //! Return last local update time.
        const qint64 &getLocalUpdateDateTime() const;

        //! Return last remote update time.
        const qint64 &getRemoteUpdateDateTime() const;

        //! Set current date time.
        void resetCurrentDateTime();

    private:

        //! Retrieve cache content from Json object.
        void fromJson(const QJsonObject &json);

        //! Export cache content to Json object.
        QJsonObject toJson() const;

};

#endif // RCL_FILE_MANAGER_CACHE_H
