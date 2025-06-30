#ifndef RCL_CLOUD_SESSION_MANAGER_H
#define RCL_CLOUD_SESSION_MANAGER_H

#include <QObject>
#include <QString>

#include "rcl_cloud_session_info.h"

class RCloudSessionManager : public QObject
{

    Q_OBJECT

    public:

        struct DefaultRangeSession
        {
            static const QString name;
            static const QString host;
            static const uint publicPort;
            static const uint privatePort;
        };
        struct DefaultCloudSession
        {
            static const QString name;
            static const QString host;
            static const uint publicPort;
            static const uint privatePort;
        };

    protected:

        static const QString sessionNameTemplate;

        QString activeSessionName;
        QList<RCloudSessionInfo> sessions;

    public:

        //! Constructor.
        explicit RCloudSessionManager(QObject *parent = nullptr);

        //! Read from file.
        void read(const QString &fileName);

        //! Write to file.
        void write(const QString &fileName) const;

        //! Set active session name.
        void setActiveSessionName(const QString &sessionName);

        //! Find active session name. If no valid session name is stored then first from the list is selected. In case the list is empty, empty string is returned.
        QString findActiveSessionName() const;

        //! Return list of session names.
        QStringList getSessionNames() const;

        //! Find existing session.
        RCloudSessionInfo findSession(const QString &sessionName) const;

        //! Rename session.
        void renameSession(const QString &oldSessionName, const QString &newSessionName);

        //! Remove session. Return number of items that were removed.
        qsizetype removeSession(const QString &sessionName);

        //! Remove all sessions.
        void removeAllSessions();

        //! Guess new session name.
        QString guessNewSessionName() const;

        //! Return default range-software session.
        static RCloudSessionInfo getDefaultRangeSession();

        //! Return default cloud session.
        static RCloudSessionInfo getDefaultCloudSession();

    public slots:

        //! Insert session.
        //! If session with same session name already exist, it will be replaced.
        void insertSession(const RCloudSessionInfo &sessionInfo);

    private:

        //! Create session object from Json.
        void fromJson(const QJsonObject &json);

        //! Create Json from session object.
        QJsonObject toJson() const;

    signals:

        //! Active session changed.
        void activeSessionChanged(RCloudSessionInfo sessionInfo);

        //! RCloudSessionManager inserted.
        void sessionInserted(const QString &sessionName);

        //! RCloudSessionManager renamed.
        void sessionRenamed(const QString &oldSessionName, const QString &newSessionName);

        //! CSessionManager removed.
        void sessionRemoved(const QString &sessionName);

};

#endif /* RCL_CLOUD_SESSION_MANAGER_H */
