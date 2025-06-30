#include "rcl_tls_key_store.h"

void RTlsKeyStore::_init(const RTlsKeyStore *pTlsKeyStore)
{
    if (pTlsKeyStore)
    {
        this->certificateFile = pTlsKeyStore->certificateFile;
        this->keyFile = pTlsKeyStore->keyFile;
        this->password = pTlsKeyStore->password;
    }
}

RTlsKeyStore::RTlsKeyStore()
{
    this->_init();

}

RTlsKeyStore::RTlsKeyStore(const RTlsKeyStore &tlsKeyStore)
{
    this->_init(&tlsKeyStore);
}

RTlsKeyStore::RTlsKeyStore(const QString &certificateFile, const QString &keyFile, const QString &password)
    : certificateFile(certificateFile)
    , keyFile(keyFile)
    , password(password)
{
    this->_init();
}

RTlsKeyStore::~RTlsKeyStore()
{

}

RTlsKeyStore &RTlsKeyStore::operator =(const RTlsKeyStore &tlsKeyStore)
{
    this->_init(&tlsKeyStore);
    return (*this);
}

const QString &RTlsKeyStore::getCertificateFile() const
{
    return this->certificateFile;
}

void RTlsKeyStore::setCertificateFile(const QString &certificateFile)
{
    this->certificateFile = certificateFile;
}

const QString &RTlsKeyStore::getKeyFile() const
{
    return this->keyFile;
}

void RTlsKeyStore::setKeyFile(const QString &keyFile)
{
    this->keyFile = keyFile;
}

const QString &RTlsKeyStore::getPassword() const
{
    return this->password;
}

void RTlsKeyStore::setPassword(const QString &password)
{
    this->password = password;
}
