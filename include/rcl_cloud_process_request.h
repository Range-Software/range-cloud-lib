#ifndef RCL_CLOUD_PROCESS_REQUEST_H
#define RCL_CLOUD_PROCESS_REQUEST_H

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QString>

#include "rcl_user_info.h"

class RCloudProcessRequest
{

    protected:

        //! Executor.
        RUserInfo executor;
        //! Process name.
        QString name;
        //! Argument values.
        QMap<QString, QString> argumentValues;

    private:

        //! Internal initialization function.
        void _init(const RCloudProcessRequest *pProcessRequest = nullptr);

    public:

        //! Constructor.
        RCloudProcessRequest();

        //! Copy constructor.
        RCloudProcessRequest(const RCloudProcessRequest &processRequest);

        //! Destructor.
        ~RCloudProcessRequest();

        //! Assignment operator.
        RCloudProcessRequest &operator =(const RCloudProcessRequest &processRequest);

        //! Return const reference to process executor.
        const RUserInfo &getExecutor() const;

        //! Set new process executor.
        void setExecutor(const RUserInfo &executor);

        //! Return const reference to process name.
        const QString &getName() const;

        //! Set new process name.
        void setName(const QString &name);

        //! Return const reference to process argument values.
        const QMap<QString, QString> &getArgumentValues() const;

        //! Set new process argument values.
        void setArgumentValues(const QMap<QString, QString> &argumentValues);

        //! Create request object from Json.
        static RCloudProcessRequest fromJson(const QJsonObject &json);

        //! Create Json from request object.
        QJsonObject toJson() const;

    protected:

        //! Create QMap from Json array.
        static QMap<QString,QString> mapFromJson(const QJsonObject &json);

        //! Create Json array from QMap.
        static QJsonObject mapToJson(const QMap<QString,QString> &map);

};

#endif // RCL_CLOUD_PROCESS_REQUEST_H
