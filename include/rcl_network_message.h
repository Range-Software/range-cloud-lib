#ifndef RCL_NETWORK_MESSAGE
#define RCL_NETWORK_MESSAGE

#include <QMap>
#include <QString>
#include <QUuid>

#include <rbl_error.h>

class RCloudAction;

class RNetworkMessage
{

    protected:

        //! Internal initialization function.
        void _init(const RNetworkMessage *pNetworkMessage = nullptr);

    protected:

        //! Message owner.
        QString owner;
        //! Message authentication token.
        QString authToken;
        //! Handler ID.
        QUuid handlerId;
        //! Message ID.
        QUuid messageId;
        //! Correlation ID.
        QUuid correlationId;
        //! From address.
        QString from;
        //! To address.
        QString to;
        //! Reply to address.
        QString replyTo;
        //! Properties.
        QMap<QString,QString> properties;
        //! Message body.
        QByteArray body;
        //! Error type.
        RError::Type errorType;

    public:

        //! Constructor.
        RNetworkMessage();

        //! Copy constructor.
        RNetworkMessage(const RNetworkMessage &networkMessage);

        //! Copy constructor.
        RNetworkMessage(const RCloudAction &action);

        //! Destructor.
        virtual ~RNetworkMessage();

        //! Assignment operator (message).
        RNetworkMessage &operator =(const RNetworkMessage &networkMessage);

        //! Convert message to reply.
        RNetworkMessage toReply(const RCloudAction &action) const;

        //! Convert to Action.
        RCloudAction toAction() const;

        //! Get const reference to owner.
        const QString &getOwner() const;

        //! Set new owner.
        void setOwner(const QString &owner);

        //! Get const reference to authentication token.
        const QString &getAuthToken() const;

        //! Set new authentication token.
        void setAuthToken(const QString &authToken);

        //! Get const reference to handler ID.
        const QUuid &getHandlerId() const;

        //! Set new handler ID.
        void setHandlerId(const QUuid &handlerId);

        //! Get const reference to message ID.
        const QUuid &getMessageId() const;

        //! Set new message ID.
        void setMessageId(const QUuid &messageId);

        //! Get const reference to correlation ID.
        const QUuid &getCorrelationId() const;

        //! Set new correlation ID.
        void setCorrelationId(const QUuid &correlationId);

        //! Get const reference to from address.
        const QString &getFrom() const;

        //! Set new from address.
        void setFrom(const QString &from);

        //! Get const reference to to address.
        const QString &getTo() const;

        //! Set new to address.
        void setTo(const QString &to);

        //! Get const reference to reply-to address.
        const QString &getReplyTo() const;

        //! Set new reply-to address.
        void setReplyTo(const QString &replyTo);

        //! Get const reference to properties.
        const QMap<QString, QString> &getProperties() const;

        //! Set new properties.
        void setProperties(const QMap<QString,QString> &properties);

        //! Get const reference to body.
        const QByteArray &getBody() const;

        //! Set new body.
        void setBody(const QByteArray &body);

        //! Get error type.
        RError::Type getErrorType() const;

        //! Set error type.
        void setErrorType(RError::Type errorType);

        //! Print message to standard output.
        virtual void print(bool printBody = false) const;

};

#endif // RCL_NETWORK_MESSAGE
