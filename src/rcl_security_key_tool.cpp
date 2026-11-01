#include <memory>
#include <type_traits>

#include <QFile>
#include <QSslCertificate>
#include <QSslCertificateExtension>
#include <QSslKey>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rcl_pkcs.h"
#include "rcl_security_key_tool.h"

const int RSecurityKeyTool::KeySize = 4096;

namespace
{
    struct CFReleaser
    {
        void operator()(CFTypeRef ref) const
        {
            if (ref)
            {
                CFRelease(ref);
            }
        }
    };

    template <typename T>
    using CFHolder = std::unique_ptr<std::remove_pointer_t<T>,CFReleaser>;

    QString errorMessage(CFErrorRef error)
    {
        if (!error)
        {
            return QString();
        }
        CFHolder<CFErrorRef> errorHolder(error);
        CFHolder<CFStringRef> description(CFErrorCopyDescription(error));
        return QString::fromCFString(description.get());
    }

    QByteArray readFile(const QString &path)
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
        {
            throw RError(RError::OpenFile,R_ERROR_REF,"Failed to open file \'%s\' for reading.",path.toUtf8().constData());
        }
        return file.readAll();
    }

    void writeFile(const QString &path, const QByteArray &data, bool ownerOnly)
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            throw RError(RError::OpenFile,R_ERROR_REF,"Failed to open file \'%s\' for writing.",path.toUtf8().constData());
        }
        if (ownerOnly)
        {
            file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        }
        if (file.write(data) != data.size())
        {
            throw RError(RError::WriteFile,R_ERROR_REF,"Failed to write file \'%s\'.",path.toUtf8().constData());
        }
    }

    QByteArray readRsaPrivateKey(const QString &keyPath)
    {
        try
        {
            return RPkcs::pemToRsaPrivateKey(readFile(keyPath));
        }
        catch (const RError &error)
        {
            throw RError(error.getType(),R_ERROR_REF,"Failed to read private key \'%s\'. %s",
                         keyPath.toUtf8().constData(),error.getMessage().toUtf8().constData());
        }
    }

    QString distinguishedName(const QSslCertificate &certificate, bool subject)
    {
        QStringList fields;
        const QList<QByteArray> attributes = subject ? certificate.subjectInfoAttributes() : certificate.issuerInfoAttributes();
        for (const QByteArray &attribute : attributes)
        {
            const QStringList values = subject ? certificate.subjectInfo(attribute) : certificate.issuerInfo(attribute);
            for (const QString &value : values)
            {
                fields.append(QString::fromLatin1(attribute) + "=" + value);
            }
        }
        return fields.join(", ");
    }

    QString colonHex(const QByteArray &data)
    {
        return QString::fromLatin1(data.toHex(':').toUpper());
    }

    QString certificateToText(const QSslCertificate &certificate)
    {
        const QString dateFormat = "yyyy-MM-dd hh:mm:ss 'UTC'";
        const QSslKey publicKey = certificate.publicKey();

        QString text = "Certificate:\n";
        text += "    Version: " + QString::fromLatin1(certificate.version()) + "\n";
        text += "    Serial Number: " + QString::fromLatin1(certificate.serialNumber()) + "\n";
        text += "    Issuer: " + distinguishedName(certificate,false) + "\n";
        text += "    Validity\n";
        text += "        Not Before: " + certificate.effectiveDate().toUTC().toString(dateFormat) + "\n";
        text += "        Not After : " + certificate.expiryDate().toUTC().toString(dateFormat) + "\n";
        text += "    Subject: " + distinguishedName(certificate,true) + "\n";
        text += "    Subject Public Key Info:\n";
        text += QString("        Public Key Algorithm: %1 (%2 bit)\n")
                    .arg(publicKey.algorithm() == QSsl::Rsa ? "RSA" : publicKey.algorithm() == QSsl::Ec ? "EC" : "Other")
                    .arg(publicKey.length());

        const QList<QSslCertificateExtension> extensions = certificate.extensions();
        if (!extensions.isEmpty())
        {
            text += "    Extensions:\n";
            for (const QSslCertificateExtension &extension : extensions)
            {
                text += "        " + extension.name() + (extension.isCritical() ? " (critical)" : "") + "\n";
            }
        }

        text += "    SHA-256 Fingerprint: " + colonHex(certificate.digest(QCryptographicHash::Sha256)) + "\n";
        text += "    SHA-1 Fingerprint: " + colonHex(certificate.digest(QCryptographicHash::Sha1)) + "\n";
        return text;
    }

    CFHolder<CFDictionaryRef> rsaKeyAttributes(CFStringRef keyClass, int keySize)
    {
        CFHolder<CFNumberRef> keySizeNumber(CFNumberCreate(kCFAllocatorDefault,kCFNumberIntType,&keySize));
        const void *keys[] = { kSecAttrKeyType, kSecAttrKeyClass, kSecAttrKeySizeInBits };
        const void *values[] = { kSecAttrKeyTypeRSA, keyClass, keySizeNumber.get() };
        return CFHolder<CFDictionaryRef>(CFDictionaryCreate(kCFAllocatorDefault,keys,values,keySize > 0 ? 3 : 2,
                                                            &kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks));
    }

    QByteArray externalRepresentation(SecKeyRef key)
    {
        CFErrorRef error = nullptr;
        CFHolder<CFDataRef> data(SecKeyCopyExternalRepresentation(key,&error));
        if (!data)
        {
            throw RError(RError::Application,R_ERROR_REF,"Failed to export key. %s",errorMessage(error).toUtf8().constData());
        }
        return QByteArray::fromCFData(data.get());
    }
}

void RSecurityKeyTool::generateKey(const QString &keyPath)
{
    RLogger::info("SecurityKeyTool: Generating RSA %d key \'%s\'\n",RSecurityKeyTool::KeySize,keyPath.toUtf8().constData());

    CFHolder<CFDictionaryRef> attributes(rsaKeyAttributes(kSecAttrKeyClassPrivate,RSecurityKeyTool::KeySize));

    CFErrorRef error = nullptr;
    CFHolder<SecKeyRef> privateKey(SecKeyCreateRandomKey(attributes.get(),&error));
    if (!privateKey)
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to generate RSA key. %s",errorMessage(error).toUtf8().constData());
    }

    // External representation of an RSA private key is PKCS#1 RSAPrivateKey
    QByteArray pkcs8 = RPkcs::rsaPrivateKeyToPkcs8(externalRepresentation(privateKey.get()));
    writeFile(keyPath,RPkcs::toPem(pkcs8,RPkcs::PrivateKeyLabel),true);
}

void RSecurityKeyTool::generateCsr(const QString &keyPath, const QMap<QString, QString> &subjectMap, const QString &csrPath)
{
    RLogger::info("SecurityKeyTool: Generating CSR \'%s\' with key \'%s\'\n",csrPath.toUtf8().constData(),keyPath.toUtf8().constData());

    QByteArray rsaPrivateKey = readRsaPrivateKey(keyPath);

    CFHolder<CFDictionaryRef> attributes(rsaKeyAttributes(kSecAttrKeyClassPrivate,0));
    CFHolder<CFDataRef> keyData(rsaPrivateKey.toCFData());

    CFErrorRef error = nullptr;
    CFHolder<SecKeyRef> privateKey(SecKeyCreateWithData(keyData.get(),attributes.get(),&error));
    if (!privateKey)
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to load RSA key \'%s\'. %s",keyPath.toUtf8().constData(),errorMessage(error).toUtf8().constData());
    }
    CFHolder<SecKeyRef> publicKey(SecKeyCopyPublicKey(privateKey.get()));
    if (!publicKey)
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to derive public key from \'%s\'.",keyPath.toUtf8().constData());
    }

    auto signer = [&privateKey](const QByteArray &data) -> QByteArray
    {
        CFHolder<CFDataRef> cfData(data.toCFData());
        CFErrorRef signError = nullptr;
        CFHolder<CFDataRef> signature(SecKeyCreateSignature(privateKey.get(),kSecKeyAlgorithmRSASignatureMessagePKCS1v15SHA256,cfData.get(),&signError));
        if (!signature)
        {
            RLogger::error("SecurityKeyTool: Failed to sign data. %s\n",errorMessage(signError).toUtf8().constData());
            return QByteArray();
        }
        return QByteArray::fromCFData(signature.get());
    };

    // External representation of an RSA public key is PKCS#1 RSAPublicKey
    QByteArray csr = RPkcs::buildRsaCsr(subjectMap,externalRepresentation(publicKey.get()),signer);
    writeFile(csrPath,RPkcs::toPem(csr,RPkcs::CertificateRequestLabel),false);
}

void RSecurityKeyTool::outputKeyAsText(const QString &keyPath, const QString &textPath)
{
    QString text = RPkcs::rsaPrivateKeyToText(readRsaPrivateKey(keyPath));
    writeFile(textPath,text.toUtf8(),true);
}

void RSecurityKeyTool::outputCertificateAsText(const QString &certPath, const QString &textPath)
{
    const QList<QSslCertificate> certificates = QSslCertificate::fromPath(certPath,QSsl::Pem);
    if (certificates.isEmpty())
    {
        throw RError(RError::InvalidInput,R_ERROR_REF,"File \'%s\' does not contain a certificate.",certPath.toUtf8().constData());
    }

    // QSslCertificate::toText() is only implemented by the OpenSSL TLS backend
    QString text;
    for (const QSslCertificate &certificate : certificates)
    {
        text += certificateToText(certificate) + QString::fromLatin1(certificate.toPem());
    }
    writeFile(textPath,text.toUtf8(),false);
}
