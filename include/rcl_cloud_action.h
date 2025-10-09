#ifndef RCL_CLOUD_ACTION_H
#define RCL_CLOUD_ACTION_H

#include <QMap>
#include <QString>
#include <QUuid>

#include <rbl_error.h>

class RCloudAction
{

    public:

        struct Auth
        {
            struct User
            {
                static const QString key;
                static const QString description;
            };

            struct Token
            {
                static const QString key;
                static const QString description;
            };
        };

        struct Resource
        {
            struct Id
            {
                static const QString key;
                static const QString description;
            };

            struct Name
            {
                static const QString key;
                static const QString description;
            };

            struct Path
            {
                static const QString key;
                static const QString description;
            };
        };

        struct Action
        {
            static const QString key;
            struct Test
            {
                static const QString key;
                static const QString description;
            };
            struct ListFiles
            {
                static const QString key;
                static const QString description;
            };
            struct FileInfo
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpload
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpdate
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpdateAccessOwner
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpdateAccessMode
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpdateVersion
            {
                static const QString key;
                static const QString description;
            };
            struct FileUpdateTags
            {
                static const QString key;
                static const QString description;
            };
            struct FileDownload
            {
                static const QString key;
                static const QString description;
            };
            struct FileRemove
            {
                static const QString key;
                static const QString description;
            };
            struct Stop
            {
                static const QString key;
                static const QString description;
            };
            struct Statistics
            {
                static const QString key;
                static const QString description;
            };
            struct Process
            {
                static const QString key;
                static const QString description;
            };
            struct ListUsers
            {
                static const QString key;
                static const QString description;
            };
            struct UserInfo
            {
                static const QString key;
                static const QString description;
            };
            struct UserAdd
            {
                static const QString key;
                static const QString description;
            };
            struct UserUpdate
            {
                static const QString key;
                static const QString description;
            };
            struct UserRemove
            {
                static const QString key;
                static const QString description;
            };
            struct UserSignUp
            {
                static const QString key;
                static const QString description;
            };
            struct ListUserTokens
            {
                static const QString key;
                static const QString description;
            };
            struct UserTokenGenerate
            {
                static const QString key;
                static const QString description;
            };
            struct UserTokenRemove
            {
                static const QString key;
                static const QString description;
            };
            struct ListGroups
            {
                static const QString key;
                static const QString description;
            };
            struct GroupInfo
            {
                static const QString key;
                static const QString description;
            };
            struct GroupAdd
            {
                static const QString key;
                static const QString description;
            };
            struct GroupRemove
            {
                static const QString key;
                static const QString description;
            };
            struct ListActions
            {
                static const QString key;
                static const QString description;
            };
            struct ActionUpdateAccessOwner
            {
                static const QString key;
                static const QString description;
            };
            struct ActionUpdateAccessMode
            {
                static const QString key;
                static const QString description;
            };
            struct ListProcesses
            {
                static const QString key;
                static const QString description;
            };
            struct ProcessUpdateAccessOwner
            {
                static const QString key;
                static const QString description;
            };
            struct ProcessUpdateAccessMode
            {
                static const QString key;
                static const QString description;
            };
            struct SubmitReport
            {
                static const QString key;
                static const QString description;
            };
        };

    protected:

        QUuid id;
        QString executor;
        QString authToken;
        QString action;
        QString resourceName;
        QUuid resourceId;
        QByteArray data;
        RError::Type errorType;

    private:

        //! Internal initialization function.
        void _init(const RCloudAction *pCAction = nullptr);

    public:

        //! Copy constructor.
        RCloudAction();

        //! Constructor.
        RCloudAction(const QUuid id, const QString &executor, const QString &authToken, const QString &action, const QString &resourceName, const QUuid &resourceId, const QByteArray &data);

        //! Copy constructor.
        RCloudAction(const RCloudAction &action);

        //! Destructor.
        ~RCloudAction();

        //! Assignment operator.
        RCloudAction & operator =(const RCloudAction &action);

        //! Get id.
        const QUuid &getId() const;

        //! Get executor.
        const QString &getExecutor() const;

        //! Get authentication token.
        const QString &getAuthToken() const;

        //! Get action.
        const QString &getAction() const;

        //! Get resource name.
        const QString &getResourceName() const;

        //! Get resource id.
        const QUuid &getResourceId() const;

        //! Get data.
        const QByteArray &getData() const;

        //! Set data.
        void setData(const QByteArray &data);

        //! Get error type.
        RError::Type getErrorType() const;

        //! Set error type.
        void setErrorType(RError::Type errorType);

        //! Return list of actions.
        static QMap<QString,QString> getActionMap();

};

#endif // RCL_CLOUD_ACTION_H
