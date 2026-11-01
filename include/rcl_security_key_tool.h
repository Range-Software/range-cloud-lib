#ifndef RCL_SECURITY_KEY_TOOL_H
#define RCL_SECURITY_KEY_TOOL_H

#include <QMap>
#include <QString>

//! Key and certificate signing request generation with Apple Security framework (macOS only).
//! Produces the same files as ROpenSslTool: unencrypted PKCS#8 PEM key and PKCS#10 PEM CSR.
class RSecurityKeyTool
{

    public:

        //! RSA key size in bits.
        static const int KeySize;

    public:

        //! Generate RSA private key and store it as PKCS#8 PEM.
        static void generateKey(const QString &keyPath);

        //! Generate CSR signed with private key read from PEM file (PKCS#8 or PKCS#1).
        static void generateCsr(const QString &keyPath, const QMap<QString,QString> &subjectMap, const QString &csrPath);

        //! Write human readable form of private key read from PEM file (PKCS#8 or PKCS#1).
        static void outputKeyAsText(const QString &keyPath, const QString &textPath);

        //! Write human readable form of certificates read from PEM file.
        static void outputCertificateAsText(const QString &certPath, const QString &textPath);

};

#endif // RCL_SECURITY_KEY_TOOL_H
