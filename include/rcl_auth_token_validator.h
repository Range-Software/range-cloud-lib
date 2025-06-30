#ifndef RCL_AUTH_TOKEN_VALIDATOR_H
#define RCL_AUTH_TOKEN_VALIDATOR_H

#include <QObject>

class RAuthTokenValidator : public QObject
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RAuthTokenValidator(QObject *parent = nullptr) : QObject{parent} { }

        //! Validate.
        virtual bool validate(const QString &resourceName, const QString &token) = 0;

};

#endif // RCL_AUTH_TOKEN_VALIDATOR_H
