#ifndef RCL_PKCS_H
#define RCL_PKCS_H

#include <functional>

#include <QByteArray>
#include <QMap>
#include <QString>

//! Minimal DER/PEM encoding of RSA keys (PKCS#1, PKCS#8) and certificate
//! signing requests (PKCS#10). The cryptography itself is left to the caller,
//! the CSR is signed through a signer callback.
class RPkcs
{

    public:

        //! Signs data (SHA-256 digest, PKCS#1 v1.5 padding) and returns the signature.
        typedef std::function<QByteArray(const QByteArray &data)> Signer;

        //! PEM labels.
        static const QString PrivateKeyLabel;
        static const QString RsaPrivateKeyLabel;
        static const QString EncryptedPrivateKeyLabel;
        static const QString CertificateRequestLabel;

    public:

        //! Build DER encoded PKCS#10 certificate signing request signed with sha256WithRSAEncryption.
        //! Subject keys are those of ROpenSslTool::CertificateSubject, empty values are skipped.
        static QByteArray buildRsaCsr(const QMap<QString,QString> &subjectMap, const QByteArray &rsaPublicKey, const Signer &signer);

        //! Wrap PKCS#1 RSAPrivateKey into PKCS#8 PrivateKeyInfo.
        static QByteArray rsaPrivateKeyToPkcs8(const QByteArray &rsaPrivateKey);

        //! Extract PKCS#1 RSAPrivateKey from PKCS#8 PrivateKeyInfo.
        static QByteArray pkcs8ToRsaPrivateKey(const QByteArray &privateKeyInfo);

        //! Extract PKCS#1 RSAPrivateKey from PEM data (PKCS#8 or PKCS#1), throws if not found.
        static QByteArray pemToRsaPrivateKey(const QByteArray &pem);

        //! Human readable form of PKCS#1 RSAPrivateKey, same as 'openssl pkey -text -noout'.
        static QString rsaPrivateKeyToText(const QByteArray &rsaPrivateKey);

        //! Encode DER data as PEM with given label.
        static QByteArray toPem(const QByteArray &der, const QString &label);

        //! Decode first PEM block with given label to DER, empty if not found.
        static QByteArray fromPem(const QByteArray &pem, const QString &label);

};

#endif // RCL_PKCS_H
