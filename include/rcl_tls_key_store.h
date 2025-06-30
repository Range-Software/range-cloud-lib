#ifndef RCL_TLS_KEY_STORE_H
#define RCL_TLS_KEY_STORE_H

#include <QString>

class RTlsKeyStore
{

    private:

        //! Internal initialization function.
        void _init(const RTlsKeyStore *pTlsKeyStore = nullptr);

    protected:

        //! Public certificate.
        QString certificateFile;
        //! Private key.
        QString keyFile;
        //! Private key password.
        QString password;

    public:

        //! Constructor.
        RTlsKeyStore();

        //! Copy constructor.
        RTlsKeyStore(const RTlsKeyStore &tlsKeyStore);

        //! Copy constructor.
        RTlsKeyStore(const QString &certificateFile, const QString &keyFile, const QString &password);

        //! Destructor.
        ~RTlsKeyStore();

        //! Assignment operator (message).
        RTlsKeyStore &operator =(const RTlsKeyStore &tlsKeyStore);

        const QString &getCertificateFile() const;

        void setCertificateFile(const QString &certificateFile);

        const QString &getKeyFile() const;

        void setKeyFile(const QString &keyFile);

        const QString &getPassword() const;

        void setPassword(const QString &password);

};

#endif // RCL_TLS_KEY_STORE_H
