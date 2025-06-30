#ifndef RCL_ACCESS_RIGHTS_H
#define RCL_ACCESS_RIGHTS_H

#include <QJsonObject>
#include <QString>

#include "rcl_access_mode.h"
#include "rcl_access_owner.h"
#include "rcl_user_info.h"

class RAccessRights
{

    protected:

        RAccessOwner owner;
        RAccessMode mode;

    private:

        //! Internal initialization function.
        void _init(const RAccessRights *pAccessRights = nullptr);

    public:

        //! Constructor.
        RAccessRights();

        //! Copy constructor.
        RAccessRights(const RAccessRights &accessRights);

        //! Destructor.
        ~RAccessRights() {};

        //! Assignment operator.
        RAccessRights &operator =(const RAccessRights &accessRights);

        //! Return true if values are valid.
        bool isValid() const;

        //! Return owner.
        const RAccessOwner &getOwner() const;

        //! Set new owner.
        void setOwner(const RAccessOwner &owner);

        //! Return mode.
        const RAccessMode &getMode() const;

        //! Set new mode.
        void setMode(const RAccessMode &owner);

        //! Check if given user is authorized.
        bool isUserAuthorized(const RUserInfo &userInfo, RAccessMode::Mode mode) const;

        //! To string.
        QString toString(const char delimiter = ',') const;

        //! From string.
        static RAccessRights fromString(const QString &line, const char delimiter = ',');

        //! Create access rights object from Json.
        static RAccessRights fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;

};

#endif // RCL_ACCESS_RIGHTS_H
