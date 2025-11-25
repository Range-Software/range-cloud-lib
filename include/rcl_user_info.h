#ifndef RCL_USER_INFO_H
#define RCL_USER_INFO_H

#include <QJsonObject>
#include <QList>
#include <QString>

#include "rcl_file_quota.h"

class RUserInfo
{

    public:

        static const QString rootUser;
        static const QString rootGroup;
        static const QString guestUser;
        static const QString guestGroup;
        static const QString userGroup;

    protected:

        //! User name.
        QString name;
        //! Group IDs.
        QList<QString> groupNames;
        //! File quota.
        RFileQuota fileQuota;

    private:

        //! Internal initialization function.
        void _init(const RUserInfo *pUserInfo = nullptr);

    public:

        //! Constructor.
        RUserInfo();

        //! Copy constructor.
        RUserInfo(const RUserInfo &userInfo);

        //! Destructor.
        ~RUserInfo();

        //! Assignment operator.
        RUserInfo &operator =(const RUserInfo &userInfo);

        //! Return const reference to name.
        const QString &getName() const;

        //! Set new name.
        void setName(const QString &name);

        //! Return const reference to list of group names.
        const QList<QString> &getGroupNames() const;

        //! Return reference to list of group names.
        QList<QString> &getGroupNames();

        //! Set new list of group IDs.
        void setGroupNames(const QList<QString> &groupNames);

        //! Return const reference to file quota.
        const RFileQuota &getFileQuota() const;

        //! Set new file quota.
        void setFileQuota(const RFileQuota &fileQuota);

        //! Return true if user was not initialized.
        bool isNull() const;

        //! Return true if given name matches user name.
        bool isUser(const QString &name) const;

        //! Return true if given group is among user groups.
        bool hasGroup(const QString &groupName) const;

        //! Create user object from Json.
        static RUserInfo fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;

        //! Create user name from Json.
        static QString userNameFromJson(const QJsonObject &json);

        //! Create Json from user name.
        static QJsonObject userNameToJson(const QString &userName);

        //! Validate name.
        static bool isNameValid(const QString &name);

};

#endif // RCL_USER_INFO_H
