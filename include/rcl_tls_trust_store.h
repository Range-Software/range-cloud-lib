#ifndef RCL_TLS_TRUST_STORE_H
#define RCL_TLS_TRUST_STORE_H

#include <QString>
#include <QSslCertificate>
#include <QJsonObject>

class RTlsTrustStore
{

    private:

        //! Internal initialization function.
        void _init(const RTlsTrustStore *pTlsTrustStore = nullptr);

    protected:

        //! Public certificate.
        QString certificateFile;

    public:

        //! Constructor.
        RTlsTrustStore();

        //! Copy constructor.
        RTlsTrustStore(const RTlsTrustStore &tlsTrustStore);

        //! Copy constructor.
        RTlsTrustStore(const QString &certificateFile);

        //! Destructor.
        ~RTlsTrustStore();

        //! Assignment operator (message).
        RTlsTrustStore &operator =(const RTlsTrustStore &tlsTrustStore);

        const QString &getCertificateFile() const;

        void setCertificateFile(const QString &certificateFile);

        //! Convert SSL Certificate to text.
        static QString toText(const QSslCertificate &sslCertificate);

        //! Create trust store object from Json.
        static RTlsTrustStore fromJson(const QJsonObject &json);

        //! Create Json from trust store object.
        QJsonObject toJson() const;

        //! Find certificate common name.
        static QString findCN(const QSslCertificate &sslCertificate);

        //! Find certificate common name.
        static QString findCN(const QString &certificateFile);

        //! Convert QSsl::SslProtocol to string.
        static QString sslProtocolToString(QSsl::SslProtocol protocol);

};

#endif // RCL_TLS_TRUST_STORE_H
