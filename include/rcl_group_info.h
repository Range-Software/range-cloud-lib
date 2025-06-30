#ifndef RCL_GROUP_INFO_H
#define RCL_GROUP_INFO_H

#include <QJsonObject>
#include <QString>

class RGroupInfo
{

    protected:

        //! Group name.
        QString name;

    private:

        //! Internal initialization function.
        void _init(const RGroupInfo *pGroupInfo = nullptr);

    public:

        //! Constructor.
        RGroupInfo();

        //! Copy constructor.
        RGroupInfo(const RGroupInfo &groupInfo);

        //! Destructor.
        ~RGroupInfo();

        //! Assignment operator.
        RGroupInfo &operator =(const RGroupInfo &groupInfo);

        //! Return const reference to name.
        const QString &getName() const;

        //! Set new name.
        void setName(const QString &name);

        //! Return true if group was not correctly initialized.
        bool isNull() const;

        //! Create group object from Json.
        static RGroupInfo fromJson(const QJsonObject &json);

        //! Create Json from froup object.
        QJsonObject toJson() const;

        //! Create group name from Json.
        static QString groupNameFromJson(const QJsonObject &json);

        //! Create Json from group name.
        static QJsonObject groupNameToJson(const QString &groupName);

        //! Validate name.
        static bool isNameValid(const QString &name);

};

#endif // RCL_GROUP_INFO_H

