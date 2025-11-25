#include "rcl_file_quota.h"

const qint64 RFileQuota::defaultStoreSize = 104857600; // 100MB
const qint64 RFileQuota::defaultFileSize = 104857600; // 100MB
const qint64 RFileQuota::defaultFileCount = 100;

void RFileQuota::_init(const RFileQuota *pFileQuota)
{
    if (pFileQuota)
    {
        this->storeSize = pFileQuota->storeSize;
        this->fileSize = pFileQuota->fileSize;
        this->fileCount = pFileQuota->fileCount;
    }
}

RFileQuota::RFileQuota()
    : storeSize{RFileQuota::defaultStoreSize}
    , fileSize{RFileQuota::defaultFileSize}
    , fileCount{RFileQuota::defaultFileCount}
{
    this->_init();
}

RFileQuota::RFileQuota(qint64 storeSize, qint64 fileSize, qint64 fileCount)
    : storeSize{storeSize}
    , fileSize{fileSize}
    , fileCount{fileCount}
{
    this->_init();
}

RFileQuota::RFileQuota(const RFileQuota &fileQuota)
{
    this->_init(&fileQuota);
}

RFileQuota::~RFileQuota()
{

}

RFileQuota &RFileQuota::operator =(const RFileQuota &fileQuota)
{
    this->_init(&fileQuota);
    return (*this);
}

qint64 RFileQuota::getStoreSize() const
{
    return this->storeSize;
}

void RFileQuota::setStoreSize(qint64 storeSize)
{
    this->storeSize = storeSize;
}

qint64 RFileQuota::getFileSize() const
{
    return this->fileSize;
}

void RFileQuota::setFileSize(qint64 fileSize)
{
    this->fileSize = fileSize;
}

qint64 RFileQuota::getFileCount() const
{
    return this->fileCount;
}

void RFileQuota::setFileCount(qint64 fileCount)
{
    this->fileCount = fileCount;
}

RFileQuota RFileQuota::fromJson(const QJsonObject &json)
{
    RFileQuota fileQuota;

    if (const QJsonValue &v = json["storeSize"]; v.isString())
    {
        fileQuota.storeSize = v.toString().toLongLong();
    }
    if (const QJsonValue &v = json["fileSize"]; v.isString())
    {
        fileQuota.fileSize = v.toString().toLongLong();
    }
    if (const QJsonValue &v = json["fileCount"]; v.isString())
    {
        fileQuota.fileCount = v.toString().toLongLong();
    }

    return fileQuota;
}

QJsonObject RFileQuota::toJson() const
{
    QJsonObject json;

    json["storeSize"] = QString::number(this->storeSize);
    json["fileSize"] = QString::number(this->fileSize);
    json["fileCount"] = QString::number(this->fileCount);

    return json;
}

bool RFileQuota::quotaExceeded(const RFileQuota &fileQuota) const
{
    return ((this->storeSize >=0 && this->storeSize < fileQuota.storeSize) ||
            (this->fileSize >=0 && this->fileSize < fileQuota.fileSize) ||
            (this->fileCount >=0 && this->fileCount < fileQuota.fileCount));
}
