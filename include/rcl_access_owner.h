#ifndef RCL_ACCESS_OWNER_H
#define RCL_ACCESS_OWNER_H

#include <QJsonObject>
#include <QString>

class RAccessOwner
{

    protected:

        QString user;
        QString group;

    private:

        //! Internal initialization function.
        void _init(const RAccessOwner *pAccessOwner = nullptr);

    public:

        //! Constructor.
        RAccessOwner();

        //! Copy constructor.
        RAccessOwner(const RAccessOwner &accessOwner);

        //! Destructor.
        ~RAccessOwner() {};

        //! Assignment operator.
        RAccessOwner &operator =(const RAccessOwner &accessOwner);

        //! Return true if values are valid.
        bool isValid() const;

        //! Return user owner.
        const QString &getUser() const;

        //! Set user owner.
        void setUser(const QString &user);

        //! Return group owner.
        const QString &getGroup() const;

        //! Set group owner.
        void setGroup(const QString &group);

        //! From string line.
        static RAccessOwner fromString(const QString &line, const char delimiter = ',');

        //! To string.
        QString toString(const char delimiter = ',') const;

        //! Create user object from Json.
        static RAccessOwner fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;

        //! Validate user.
        static bool isUserValid(const QString &user);

        //! Validate group.
        static bool isGroupValid(const QString &group);

};

#endif // RCL_ACCESS_OWNER_H
