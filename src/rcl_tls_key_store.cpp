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

bool RTlsKeyStore::isValid() const
{
    if (this->certificateFile.isEmpty() || this->keyFile.isEmpty() || this->password.isEmpty())
    {
        return false;
    }
    return true;
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

RTlsKeyStore RTlsKeyStore::fromJson(const QJsonObject &json)
{
    RTlsKeyStore keyStore;

    if (const QJsonValue &v = json["keyFile"]; v.isString())
    {
        keyStore.keyFile = v.toString();
    }
    if (const QJsonValue &v = json["password"]; v.isString())
    {
        keyStore.password = v.toString();
    }
    if (const QJsonValue &v = json["certificateFile"]; v.isString())
    {
        keyStore.certificateFile = v.toString();
    }

    return keyStore;
}

QJsonObject RTlsKeyStore::toJson() const
{
    QJsonObject json;

    json["keyFile"] = this->keyFile;
    json["password"] = this->password;
    json["certificateFile"] = this->certificateFile;

    return json;
}
