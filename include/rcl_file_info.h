#ifndef RCL_FILE_INFO_H
#define RCL_FILE_INFO_H

#include <QJsonObject>
#include <QString>
#include <QUuid>

#include <rbl_version.h>

#include "rcl_access_rights.h"

class RFileInfo
{

    public:

        //! Maximum number of tags.
        static const uint MaxNumTags;

    protected:

        //! Internal initialization function.
        void _init(const RFileInfo *pFileInfo = nullptr);

    protected:

        static const char delimiter;
        static const char accessRightsDelimiter;
        static const char versionDelimiter;
        static const char tagsDelimiter;

        //! ID.
        QUuid id;

        //! File size in bytes.
        qint64 size;
        //! Creation datetime.
        qint64 creationDateTime;
        //! Update datetime.
        qint64 updateDateTime;
        //! File name (including path).
        QString path;
        //! Access rights.
        RAccessRights accessRights;
        //! File version.
        RVersion version;
        //! Tags
        QStringList tags;
        //! Checksum
        QByteArray md5Checksum;

    public:

        //! Constructor.
        RFileInfo();

        //! Copy constructor.
        RFileInfo(const RFileInfo &fileInfo);

        //! Destructor.
        ~RFileInfo();

        //! Assignment operator.
        RFileInfo &operator =(const RFileInfo &fileInfo);

        //! Get const reference to file ID.
        const QUuid &getId() const;

        //! Set new file ID.
        void setId(const QUuid &id);

        //! Get size.
        qint64 getSize() const;

        //! Set new size.
        void setSize(qint64 size);

        //! Get creation date-time seconds since epoch.
        qint64 getCreationDateTime() const;

        //! Set new creation date-time seconds since epoch.
        void setCreationDateTime(qint64 creationDateTime);

        //! Get update date-time seconds since epoch.
        qint64 getUpdateDateTime() const;

        //! Set new update date-time seconds since epoch.
        void setUpdateDateTime(qint64 updateDateTime);

        //! Get const reference to file path.
        const QString &getPath() const;

        //! Set new file path.
        void setPath(const QString &remotePath);

        //! Get access rights.
        const RAccessRights &getAccessRights() const;

        //! Set new access rights.
        void setAccessRights(const RAccessRights &accessRights);

        //! Get version.
        const RVersion &getVersion() const;

        //! Set new version.
        void setVersion(const RVersion &version);

        //! Get tags.
        const QStringList &getTags() const;

        //! Set new tags.
        void setTags(const QStringList &tags);

        //! Get md5 checksum.
        const QByteArray &getMd5Checksum() const;

        //! Set new md5 checksum.
        void setMd5Checksum(const QByteArray &md5Checksum);

        //! To string.
        QString toString() const;

        //! From string.
        static RFileInfo fromString(const QString &line);

        //! Create file info object from Json.
        static RFileInfo fromJson(const QJsonObject &json);

        //! To Json.
        QJsonObject toJson() const;

        //! Return true if file info contains given tag.
        bool hasTag(const QString &tag) const;

        //! Return true if file info contains all given tags.
        //! When input list is empty true is returned.
        bool hasTags(const QStringList &tags) const;

        //! Validate file path.
        static bool isPathValid(const QString &path);

        //! Validate file tag.
        static bool isTagValid(const QString &tag);

        //! Get file md5 checksum.
        static QByteArray findMd5Checksum(const QString &fileName);

};

#endif // RCL_FILE_INFO_H
