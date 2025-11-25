#ifndef RCL_FILE_QUOTA_H
#define RCL_FILE_QUOTA_H

#include <QJsonObject>

class RFileQuota
{

    public:

        //! Default store size.
        static const qint64 defaultStoreSize;
        //! Default file size.
        static const qint64 defaultFileSize;
        //! Default file count.
        static const qint64 defaultFileCount;

    protected:

        //! Store size.
        qint64 storeSize;
        //! File size.
        qint64 fileSize;
        //! File count.
        qint64 fileCount;

    private:

        //! Internal initialization function.
        void _init(const RFileQuota *pFileQuota = nullptr);

    public:

        //! Constructor.
        RFileQuota();

        //! Constructor.
        RFileQuota(qint64 storeSize, qint64 fileSize, qint64 fileCount);

        //! Copy constructor.
        RFileQuota(const RFileQuota &fileQuota);

        //! Destructor.
        ~RFileQuota();

        //! Assignment operator.
        RFileQuota &operator =(const RFileQuota &fileQuota);

        //! Return store size;
        qint64 getStoreSize() const;

        //! Set new store size;
        void setStoreSize(qint64 storeSize);

        //! Return file size;
        qint64 getFileSize() const;

        //! Set new file size;
        void setFileSize(qint64 fileSize);

        //! Return file count;
        qint64 getFileCount() const;

        //! Set new file count;
        void setFileCount(qint64 fileCount);

        //! Create file quota object from Json.
        static RFileQuota fromJson(const QJsonObject &json);

        //! Create Json from file quota object.
        QJsonObject toJson() const;

        //! Return true if quota has exceeded (negative value means unlimited).
        bool quotaExceeded(const RFileQuota &fileQuota) const;

};

#endif // RCL_FILE_QUOTA_H
