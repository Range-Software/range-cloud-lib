#ifndef RCL_CLOUD_ACTION_INFO_H
#define RCL_CLOUD_ACTION_INFO_H

#include <QJsonObject>
#include <QString>

#include "rcl_access_rights.h"

class RCloudActionInfo
{

    protected:

        //! Action name.
        QString name;
        //! Access rights.
        RAccessRights accessRights;

    private:

        //! Internal initialization function.
        void _init(const RCloudActionInfo *pActionInfo = nullptr);

    public:

        //! Constructor.
        RCloudActionInfo();

        //! Copy constructor.
        RCloudActionInfo(const RCloudActionInfo &actionInfo);

        //! Destructor.
        ~RCloudActionInfo();

        //! Assignment operator.
        RCloudActionInfo &operator =(const RCloudActionInfo &actionInfo);

        //! Return const reference to name.
        const QString &getName() const;

        //! Set new name.
        void setName(const QString &name);

        //! Return const reference to access rights.
        const RAccessRights &getAccessRights() const;

        //! Set new access rights.
        void setAccessRights(const RAccessRights &accessRights);

        //! Return true if action was not correctly initialized.
        bool isNull() const;

        //! Create action object from Json.
        static RCloudActionInfo fromJson(const QJsonObject &json);

        //! Create Json from froup object.
        QJsonObject toJson() const;

};

#endif // RCL_CLOUD_ACTION_INFO_H
