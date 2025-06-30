#ifndef RCL_AUTH_TOKEN_H
#define RCL_AUTH_TOKEN_H

#include <QUuid>
#include <QString>
#include <QJsonObject>

class RAuthToken
{

    protected:

        //! Token ID.
        QUuid id;
        //! Token content.
        QString content;
        //! Resource name.
        QString resourceName;
        //! Validity date.
        qint64 validityDate;

    private:

        //! Internal initialization function.
        void _init(const RAuthToken *pAuthInfo = nullptr);

    public:

        //! Constructor.
        RAuthToken();

        //! Copy constructor.
        RAuthToken(const RAuthToken &authInfo);

        //! Destructor.
        ~RAuthToken();

        //! Assignment operator.
        RAuthToken &operator =(const RAuthToken &authInfo);

        //! Return const reference to token id.
        const QUuid &getId() const;

        //! Return const reference to token content.
        const QString &getContent() const;

        //! Set new token content.
        void setContent(const QString &content);

        //! Return const reference to resource name.
        const QString &getResourceName() const;

        //! Set new resource name.
        void setResourceName(const QString &resourceName);

        //! Return validity date.
        qint64 getValidityDate() const;

        //! Set new validity date.
        void setValidityDate(qint64 validityDate);

        //! Return true if token was not fully initialized.
        bool isNull() const;

        //! Create authentication token object from Json.
        static RAuthToken fromJson(const QJsonObject &json);

        //! Create Json from authentication token object.
        QJsonObject toJson() const;

        //! Generate random and unique token content.
        static QString generateTokenContent();

        //! Return seconds since epoch + nMonths from now.
        static quint64 validityMonthsFromNow(uint months);

};

#endif // RCL_AUTH_TOKEN_H
