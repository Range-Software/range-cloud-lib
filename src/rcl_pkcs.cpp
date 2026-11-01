#include <rbl_error.h>

#include "rcl_open_ssl_tool.h"
#include "rcl_pkcs.h"

const QString RPkcs::PrivateKeyLabel = "PRIVATE KEY";
const QString RPkcs::RsaPrivateKeyLabel = "RSA PRIVATE KEY";
const QString RPkcs::EncryptedPrivateKeyLabel = "ENCRYPTED PRIVATE KEY";
const QString RPkcs::CertificateRequestLabel = "CERTIFICATE REQUEST";

namespace
{
    // DER tags
    const quint8 TagInteger = 0x02;
    const quint8 TagBitString = 0x03;
    const quint8 TagOctetString = 0x04;
    const quint8 TagNull = 0x05;
    const quint8 TagOid = 0x06;
    const quint8 TagUtf8String = 0x0C;
    const quint8 TagPrintableString = 0x13;
    const quint8 TagSequence = 0x30;
    const quint8 TagSet = 0x31;
    const quint8 TagContext0 = 0xA0;

    // Encoded object identifiers
    const QByteArray OidRsaEncryption = QByteArray::fromHex("2a864886f70d010101");          // 1.2.840.113549.1.1.1
    const QByteArray OidSha256WithRsaEncryption = QByteArray::fromHex("2a864886f70d01010b"); // 1.2.840.113549.1.1.11

    QByteArray derLength(qsizetype length)
    {
        QByteArray encoded;
        if (length < 0x80)
        {
            encoded.append(char(length));
            return encoded;
        }
        while (length > 0)
        {
            encoded.prepend(char(length & 0xFF));
            length >>= 8;
        }
        encoded.prepend(char(0x80 | encoded.size()));
        return encoded;
    }

    QByteArray der(quint8 tag, const QByteArray &content)
    {
        return char(tag) + derLength(content.size()) + content;
    }

    QByteArray derRsaAlgorithm(const QByteArray &oid)
    {
        return der(TagSequence,der(TagOid,oid) + der(TagNull,QByteArray()));
    }

    bool isPrintableString(const QString &value)
    {
        for (const QChar &c : value)
        {
            if (!(c.isLetterOrNumber() && c.unicode() < 0x80) && !QString(" '()+,-./:=?").contains(c))
            {
                return false;
            }
        }
        return true;
    }

    // Read one DER element at position, advance position past it.
    bool derRead(const QByteArray &data, qsizetype &position, quint8 &tag, QByteArray &content)
    {
        if (position + 2 > data.size())
        {
            return false;
        }
        tag = quint8(data.at(position++));
        qsizetype length = quint8(data.at(position++));
        if (length & 0x80)
        {
            int nBytes = length & 0x7F;
            if (nBytes == 0 || nBytes > 4 || position + nBytes > data.size())
            {
                return false;
            }
            length = 0;
            for (int i=0;i<nBytes;i++)
            {
                length = (length << 8) | quint8(data.at(position++));
            }
        }
        if (position + length > data.size())
        {
            return false;
        }
        content = data.mid(position,length);
        position += length;
        return true;
    }

    QByteArray derExpect(const QByteArray &data, qsizetype &position, quint8 expectedTag)
    {
        quint8 tag;
        QByteArray content;
        if (!derRead(data,position,tag,content) || tag != expectedTag)
        {
            throw RError(RError::InvalidInput,R_ERROR_REF,"Malformed DER data, expected tag 0x%02x.",expectedTag);
        }
        return content;
    }

    // Formatted as OpenSSL prints big numbers: small values in decimal, others as hex bytes, 15 per line.
    QString integerToText(const QString &label, const QByteArray &value)
    {
        QByteArray magnitude = value;
        while (magnitude.size() > 1 && magnitude.at(0) == 0)
        {
            magnitude.remove(0,1);
        }
        if (magnitude.size() <= 8)
        {
            quint64 number = 0;
            for (char c : std::as_const(magnitude))
            {
                number = (number << 8) | quint8(c);
            }
            return QString("%1: %2 (0x%3)\n").arg(label).arg(number).arg(number,0,16);
        }

        QString text = label + ":";
        for (qsizetype i=0;i<value.size();i++)
        {
            if (i % 15 == 0)
            {
                text += "\n    ";
            }
            text += QString("%1").arg(quint8(value.at(i)),2,16,QChar('0'));
            if (i + 1 < value.size())
            {
                text += ":";
            }
        }
        return text + "\n";
    }

    int integerBitLength(const QByteArray &value)
    {
        for (qsizetype i=0;i<value.size();i++)
        {
            quint8 byte = quint8(value.at(i));
            if (byte != 0)
            {
                int bits = 0;
                while (byte)
                {
                    bits++;
                    byte >>= 1;
                }
                return int(value.size() - i - 1) * 8 + bits;
            }
        }
        return 0;
    }
}

QByteArray RPkcs::buildRsaCsr(const QMap<QString, QString> &subjectMap, const QByteArray &rsaPublicKey, const Signer &signer)
{
    // Same order as passed to 'openssl req -subj'
    const QList<std::pair<QString,quint8>> subjectAttributes =
    {
        { ROpenSslTool::CertificateSubject::Country::key,          0x06 }, // 2.5.4.6
        { ROpenSslTool::CertificateSubject::State::key,            0x08 }, // 2.5.4.8
        { ROpenSslTool::CertificateSubject::Location::key,         0x07 }, // 2.5.4.7
        { ROpenSslTool::CertificateSubject::Organization::key,     0x0A }, // 2.5.4.10
        { ROpenSslTool::CertificateSubject::OrganizationUnit::key, 0x0B }, // 2.5.4.11
        { ROpenSslTool::CertificateSubject::CommonName::key,       0x03 }  // 2.5.4.3
    };

    QByteArray name;
    for (const auto &[key, oidSuffix] : subjectAttributes)
    {
        const QString value = subjectMap.value(key);
        if (value.isEmpty())
        {
            continue;
        }
        // Country is PrintableString (RFC 5280), others UTF8String (string_mask = utf8only)
        bool printable = (key == ROpenSslTool::CertificateSubject::Country::key) && isPrintableString(value);
        QByteArray attribute = der(TagOid,QByteArray::fromHex("5504") + char(oidSuffix))
                             + der(printable ? TagPrintableString : TagUtf8String,value.toUtf8());
        name += der(TagSet,der(TagSequence,attribute));
    }

    QByteArray subjectPublicKeyInfo = der(TagSequence,
                                          derRsaAlgorithm(OidRsaEncryption)
                                          + der(TagBitString,char(0x00) + rsaPublicKey));

    QByteArray certificationRequestInfo = der(TagSequence,
                                              der(TagInteger,QByteArray(1,char(0x00)))
                                              + der(TagSequence,name)
                                              + subjectPublicKeyInfo
                                              + der(TagContext0,QByteArray()));

    QByteArray signature = signer(certificationRequestInfo);
    if (signature.isEmpty())
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to sign certificate signing request.");
    }

    return der(TagSequence,
               certificationRequestInfo
               + derRsaAlgorithm(OidSha256WithRsaEncryption)
               + der(TagBitString,char(0x00) + signature));
}

QByteArray RPkcs::rsaPrivateKeyToPkcs8(const QByteArray &rsaPrivateKey)
{
    return der(TagSequence,
               der(TagInteger,QByteArray(1,char(0x00)))
               + derRsaAlgorithm(OidRsaEncryption)
               + der(TagOctetString,rsaPrivateKey));
}

QByteArray RPkcs::pkcs8ToRsaPrivateKey(const QByteArray &privateKeyInfo)
{
    qsizetype position = 0;
    QByteArray sequence = derExpect(privateKeyInfo,position,TagSequence);

    position = 0;
    derExpect(sequence,position,TagInteger);
    QByteArray algorithm = derExpect(sequence,position,TagSequence);
    QByteArray rsaPrivateKey = derExpect(sequence,position,TagOctetString);

    qsizetype algorithmPosition = 0;
    if (derExpect(algorithm,algorithmPosition,TagOid) != OidRsaEncryption)
    {
        throw RError(RError::InvalidInput,R_ERROR_REF,"Private key is not an RSA key.");
    }

    return rsaPrivateKey;
}

QByteArray RPkcs::pemToRsaPrivateKey(const QByteArray &pem)
{
    if (QByteArray pkcs8 = RPkcs::fromPem(pem,RPkcs::PrivateKeyLabel); !pkcs8.isEmpty())
    {
        return RPkcs::pkcs8ToRsaPrivateKey(pkcs8);
    }
    if (QByteArray pkcs1 = RPkcs::fromPem(pem,RPkcs::RsaPrivateKeyLabel); !pkcs1.isEmpty())
    {
        return pkcs1;
    }
    if (pem.contains(RPkcs::EncryptedPrivateKeyLabel.toLatin1()))
    {
        throw RError(RError::InvalidInput,R_ERROR_REF,"Encrypted private key is not supported.");
    }
    throw RError(RError::InvalidInput,R_ERROR_REF,"Data does not contain an RSA private key.");
}

QString RPkcs::rsaPrivateKeyToText(const QByteArray &rsaPrivateKey)
{
    const QStringList labels =
    {
        "modulus", "publicExponent", "privateExponent", "prime1", "prime2", "exponent1", "exponent2", "coefficient"
    };

    qsizetype position = 0;
    QByteArray sequence = derExpect(rsaPrivateKey,position,TagSequence);

    position = 0;
    derExpect(sequence,position,TagInteger); // version

    QList<QByteArray> values;
    for (qsizetype i=0;i<labels.size();i++)
    {
        values.append(derExpect(sequence,position,TagInteger));
    }

    QString text = QString("Private-Key: (%1 bit, 2 primes)\n").arg(integerBitLength(values.at(0)));
    for (qsizetype i=0;i<labels.size();i++)
    {
        text += integerToText(labels.at(i),values.at(i));
    }
    return text;
}

QByteArray RPkcs::toPem(const QByteArray &der, const QString &label)
{
    QByteArray base64 = der.toBase64();
    QByteArray pem = "-----BEGIN " + label.toLatin1() + "-----\n";
    for (qsizetype i=0;i<base64.size();i+=64)
    {
        pem += base64.mid(i,64) + "\n";
    }
    pem += "-----END " + label.toLatin1() + "-----\n";
    return pem;
}

QByteArray RPkcs::fromPem(const QByteArray &pem, const QString &label)
{
    const QByteArray begin = "-----BEGIN " + label.toLatin1() + "-----";
    const QByteArray end = "-----END " + label.toLatin1() + "-----";

    qsizetype beginPosition = pem.indexOf(begin);
    if (beginPosition < 0)
    {
        return QByteArray();
    }
    beginPosition += begin.size();
    qsizetype endPosition = pem.indexOf(end,beginPosition);
    if (endPosition < 0)
    {
        return QByteArray();
    }
    return QByteArray::fromBase64(pem.mid(beginPosition,endPosition-beginPosition).simplified().replace(" ",""));
}
