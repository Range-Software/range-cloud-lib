#ifndef RCL_CLOUD_PROCESS_INFO_H
#define RCL_CLOUD_PROCESS_INFO_H

#include <QJsonObject>
#include <QList>
#include <QString>

#include "rcl_access_rights.h"

class RCloudProcessInfo
{

    protected:

        //! Name.
        QString name;
        //! Path to executable..
        QString executable;
        //! List of arguments.
        QStringList arguments;
        //! Access rights.
        RAccessRights accessRights;

    private:

        //! Internal initialization function.
        void _init(const RCloudProcessInfo *pProcessInfo = nullptr);

    public:

        //! Constructor.
        RCloudProcessInfo();

        //! Copy constructor.
        RCloudProcessInfo(const RCloudProcessInfo &processInfo);

        //! Destructor.
        ~RCloudProcessInfo();

        //! Assignment operator.
        RCloudProcessInfo &operator =(const RCloudProcessInfo &processInfo);

        //! Return const reference to name.
        const QString &getName() const;

        //! Set new name.
        void setName(const QString &name);

        //! Return const reference to executable.
        const QString &getExecutable() const;

        //! Return reference to executable.
        QString &getExecutable();

        //! Set new executable.
        void setExecutable(const QString &executable);

        //! Return const reference to list of arguments.
        const QStringList &getArguments() const;

        //! Set new list of arguments.
        void setArguments(const QStringList &arguments);

        //! Return const reference to access rights.
        const RAccessRights &getAccessRights() const;

        //! Set new access rights.
        void setAccessRights(const RAccessRights &accessRights);

        //! Build command from executable and arguments.
        QString buildCommand() const;

        //! Create user object from Json.
        static RCloudProcessInfo fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;
};

#endif // RCL_CLOUD_PROCESS_INFO_H
