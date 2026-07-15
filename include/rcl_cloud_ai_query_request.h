#ifndef RCL_CLOUD_AI_QUERY_REQUEST_H
#define RCL_CLOUD_AI_QUERY_REQUEST_H

#include <QJsonObject>
#include <QString>

#include "rcl_user_info.h"

class RCloudAIQueryRequest
{

    protected:

        //! Executor.
        RUserInfo executor;
        //! Application identifier (selects the server-side context and guardrails).
        QString application;
        //! Model name (optional, server default is used when empty).
        QString model;
        //! Query (prompt) text.
        QString query;

    private:

        //! Internal initialization function.
        void _init(const RCloudAIQueryRequest *pAIQueryRequest = nullptr);

    public:

        //! Constructor.
        RCloudAIQueryRequest();

        //! Copy constructor.
        RCloudAIQueryRequest(const RCloudAIQueryRequest &aiQueryRequest);

        //! Destructor.
        ~RCloudAIQueryRequest();

        //! Assignment operator.
        RCloudAIQueryRequest &operator =(const RCloudAIQueryRequest &aiQueryRequest);

        //! Return const reference to query executor.
        const RUserInfo &getExecutor() const;

        //! Set new query executor.
        void setExecutor(const RUserInfo &executor);

        //! Return const reference to application identifier.
        const QString &getApplication() const;

        //! Set new application identifier.
        void setApplication(const QString &application);

        //! Return const reference to model name.
        const QString &getModel() const;

        //! Set new model name.
        void setModel(const QString &model);

        //! Return const reference to query text.
        const QString &getQuery() const;

        //! Set new query text.
        void setQuery(const QString &query);

        //! Create request object from Json.
        static RCloudAIQueryRequest fromJson(const QJsonObject &json);

        //! Create Json from request object.
        QJsonObject toJson() const;

};

#endif // RCL_CLOUD_AI_QUERY_REQUEST_H
