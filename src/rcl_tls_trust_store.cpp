#include <QSslKey>
#include <QSslCertificateExtension>

#include "rcl_tls_trust_store.h"
#include "rcl_open_ssl_tool.h"

void RTlsTrustStore::_init(const RTlsTrustStore *pTlsTrustStore)
{
    if (pTlsTrustStore)
    {
        this->certificateFile = pTlsTrustStore->certificateFile;
    }
}

RTlsTrustStore::RTlsTrustStore()
{
    this->_init();

}

RTlsTrustStore::RTlsTrustStore(const RTlsTrustStore &tlsTrustStore)
{
    this->_init(&tlsTrustStore);
}

RTlsTrustStore::RTlsTrustStore(const QString &certificateFile)
    : certificateFile(certificateFile)
{
    this->_init();
}

RTlsTrustStore::~RTlsTrustStore()
{

}

RTlsTrustStore &RTlsTrustStore::operator =(const RTlsTrustStore &tlsTrustStore)
{
    this->_init(&tlsTrustStore);
    return (*this);
}

bool RTlsTrustStore::isValid() const
{
    if (this->certificateFile.isEmpty())
    {
        return false;
    }
    return true;
}

const QString &RTlsTrustStore::getCertificateFile() const
{
    return this->certificateFile;
}

void RTlsTrustStore::setCertificateFile(const QString &certificateFile)
{
    this->certificateFile = certificateFile;
}

QString RTlsTrustStore::toText(const QSslCertificate &sslCertificate)
{
    if (sslCertificate.isNull())
    {
        return QString();
    }

#ifdef __APPLE__
    QString text;

    text.append("Certificate:\n");
    text.append("    Data:\n");
    text.append(QString("        Version: %1\n").arg(sslCertificate.version().constData()));
    QString serialHex = QString(sslCertificate.serialNumber()).remove(':');
    bool ok;
    int serialDec = QString(serialHex).toInt(&ok,16);
    text.append(QString("        Serial Number: %1 (0x%2)\n").arg(QString::number(serialDec),serialHex));

    text.append("        Issuer:");
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::CountryName))
    {
        text.append(QString(" C = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::StateOrProvinceName))
    {
        text.append(QString(" ST = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::LocalityName))
    {
        text.append(QString(" L = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::Organization))
    {
        text.append(QString(" O = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::OrganizationalUnitName))
    {
        text.append(QString(" OU = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.issuerInfo(QSslCertificate::CommonName))
    {
        text.append(QString(" CN = %1").arg(n));
    }
    text.append('\n');
    text.append("        Validity\n");
    text.append(QString("            Not Before: %1\n").arg(sslCertificate.effectiveDate().toString()));
    text.append(QString("            Not After : %1\n").arg(sslCertificate.expiryDate().toString()));
    text.append('\n');
    text.append("        Subject:");
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::CountryName))
    {
        text.append(QString(" C = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::StateOrProvinceName))
    {
        text.append(QString(" ST = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::LocalityName))
    {
        text.append(QString(" L = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::Organization))
    {
        text.append(QString(" O = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::OrganizationalUnitName))
    {
        text.append(QString(" OU = %1,").arg(n));
    }
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::CommonName))
    {
        text.append(QString(" CN = %1").arg(n));
    }
    text.append('\n');
    text.append("        Email Address:");
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::EmailAddress))
    {
        text.append(QString(" %1").arg(n));
    }
    text.append('\n');
    text.append("        Serial Number:");
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::SerialNumber))
    {
        text.append(QString(" %1").arg(n));
    }
    text.append('\n');
    text.append("        Distinguished Name:");
    for (const QString &n : sslCertificate.subjectInfo(QSslCertificate::DistinguishedNameQualifier))
    {
        text.append(QString(" %1").arg(n));
    }
    text.append('\n');
    text.append("        Subject Public Key Info:\n");
    text.append("            Public Key Algorithm: ");
    switch (sslCertificate.publicKey().algorithm())
    {
        case QSsl::Rsa:
        {
            text.append("RSA");
            break;
        }
        case QSsl::Dsa:
        {
            text.append("DSA");
            break;
        }
        case QSsl::Ec:
        {
            text.append("Elliptic Curve");
            break;
        }
        case QSsl::Dh:
        {
            text.append("Diffie-Hellman");
            break;
        }
        case QSsl::Opaque:
        default:
        {
            text.append("Opaque");
            break;
        }
    }
    text.append('\n');
    text.append("        X509 extensions:\n");
    for (const QSslCertificateExtension &ex : sslCertificate.extensions())
    {
        text.append(QString("            %1 (OID: %2)\n").arg(ex.name(),ex.oid()));
    }
    text.append("\n\n");

    return text;
#else
    return sslCertificate.toText();
#endif
}

RTlsTrustStore RTlsTrustStore::fromJson(const QJsonObject &json)
{
    RTlsTrustStore trustStore;

    if (const QJsonValue &v = json["certificateFile"]; v.isString())
    {
        trustStore.certificateFile = v.toString();
    }

    return trustStore;
}

QJsonObject RTlsTrustStore::toJson() const
{
    QJsonObject json;

    json["certificateFile"] = this->certificateFile;

    return json;
}

QString RTlsTrustStore::findCN(const QSslCertificate &sslCertificate)
{
    QStringList values = sslCertificate.subjectInfo(QSslCertificate::CommonName);
    if (values.size() == 0)
    {
        return QString();
    }
    return values.at(0);
}

QString RTlsTrustStore::findCN(const QString &certificateFile)
{
    QList<QSslCertificate> sslCertificates = QSslCertificate::fromPath(certificateFile,QSsl::EncodingFormat::Pem);
    if (sslCertificates.size() == 0)
    {
        return QString();
    }
    return RTlsTrustStore::findCN(sslCertificates.at(0));
}

QPair<QDateTime, QDateTime> RTlsTrustStore::findValidity(const QString &certificateFile)
{
    QList<QSslCertificate> sslCertificates = QSslCertificate::fromPath(certificateFile,QSsl::EncodingFormat::Pem);
    if (sslCertificates.size() == 0)
    {
        return QPair<QDateTime, QDateTime>();
    }

    QPair<QDateTime, QDateTime> validity;

    validity.first = sslCertificates.at(0).effectiveDate();
    validity.second = sslCertificates.at(0).expiryDate();

    return validity;
}

QMap<QString, QString> RTlsTrustStore::findSubjectFields(const QString &certificateFile)
{
    QMap<QString, QString> subjectMap;

    const QList<QSslCertificate> clientCertificates = QSslCertificate::fromPath(certificateFile,QSsl::EncodingFormat::Pem);
    if (clientCertificates.size() > 0)
    {
        QMap<QSslCertificate::SubjectInfo,QString> subjectInfoMap;
        subjectInfoMap.insert(QSslCertificate::CountryName,ROpenSslTool::CertificateSubject::Country::key);
        subjectInfoMap.insert(QSslCertificate::StateOrProvinceName,ROpenSslTool::CertificateSubject::State::key);
        subjectInfoMap.insert(QSslCertificate::LocalityName,ROpenSslTool::CertificateSubject::Location::key);
        subjectInfoMap.insert(QSslCertificate::Organization,ROpenSslTool::CertificateSubject::Organization::key);
        subjectInfoMap.insert(QSslCertificate::OrganizationalUnitName,ROpenSslTool::CertificateSubject::OrganizationUnit::key);
        subjectInfoMap.insert(QSslCertificate::CommonName,ROpenSslTool::CertificateSubject::CommonName::key);

        for (auto it=subjectInfoMap.cbegin();it!=subjectInfoMap.cend();++it)
        {
            QStringList values = clientCertificates.at(0).subjectInfo(it.key());
            if (values.size() > 0)
            {
                subjectMap.insert(it.value(),values.at(0));
            }
        }
    }

    return subjectMap;
}

QString RTlsTrustStore::sslProtocolToString(QSsl::SslProtocol protocol)
{
    switch (protocol) {
    case QSsl::UnknownProtocol:
        return "Unknown Protocol";
    case QSsl::TlsV1_2:
        return "TLS 1.2";
    case QSsl::TlsV1_2OrLater:
        return "TLSv1.2 or later";
    case QSsl::TlsV1_3:
        return "TLS 1.3";
    case QSsl::TlsV1_3OrLater:
        return "TLSv1.3 or later";
    case QSsl::AnyProtocol:
        return "Any Protocol";
    case QSsl::SecureProtocols:
        return "Secure Protocols (TLS 1.2+)";
    default:
        return QString("Unknown enum value (%1) or unsupported protocol").arg(static_cast<int>(protocol));
    }
}
