#include <QDateTime>
#include <QJsonArray>
#include <QRegularExpression>
#include <QFile>
#include <QCryptographicHash>

#include <rbl_logger.h>

#include "rcl_file_info.h"

const uint RFileInfo::MaxNumTags = 8;
const char RFileInfo::delimiter = '|';
const char RFileInfo::accessRightsDelimiter = ',';
const char RFileInfo::versionDelimiter = '.';
const char RFileInfo::tagsDelimiter = ',';

void RFileInfo::_init(const RFileInfo *pFileInfo)
{
    if (pFileInfo)
    {
        this->id = pFileInfo->id;
        this->size = pFileInfo->size;
        this->creationDateTime = pFileInfo->creationDateTime;
        this->updateDateTime = pFileInfo->updateDateTime;
        this->path = pFileInfo->path;
        this->accessRights = pFileInfo->accessRights;
        this->version = pFileInfo->version;
        this->tags = pFileInfo->tags;
        this->md5Checksum = pFileInfo->md5Checksum;
    }
}

RFileInfo::RFileInfo()
    : id{QUuid::createUuid()}
    , size{0}
    , creationDateTime{QDateTime::currentSecsSinceEpoch()}
    , updateDateTime{this->creationDateTime}
{
    this->_init();
}

RFileInfo::RFileInfo(const RFileInfo &fileInfo)
{
    this->_init(&fileInfo);
}

RFileInfo::~RFileInfo()
{

}

RFileInfo &RFileInfo::operator =(const RFileInfo &fileInfo)
{
    this->_init(&fileInfo);
    return (*this);
}

const QUuid &RFileInfo::getId() const
{
    return this->id;
}

void RFileInfo::setId(const QUuid &id)
{
    this->id = id;
}

qint64 RFileInfo::getSize() const
{
    return this->size;
}

void RFileInfo::setSize(qint64 size)
{
    this->size = size;
}

qint64 RFileInfo::getCreationDateTime() const
{
    return this->creationDateTime;
}

void RFileInfo::setCreationDateTime(qint64 creationDateTime)
{
    this->creationDateTime = creationDateTime;
}

qint64 RFileInfo::getUpdateDateTime() const
{
    return this->updateDateTime;
}

void RFileInfo::setUpdateDateTime(qint64 updateDateTime)
{
    this->updateDateTime = updateDateTime;
}

const QString &RFileInfo::getPath() const
{
    return this->path;
}

void RFileInfo::setPath(const QString &remotePath)
{
    this->path = remotePath;
}

const RAccessRights &RFileInfo::getAccessRights() const
{
    return this->accessRights;
}

void RFileInfo::setAccessRights(const RAccessRights &accessRights)
{
    this->accessRights = accessRights;
}

const RVersion &RFileInfo::getVersion() const
{
    return this->version;
}

void RFileInfo::setVersion(const RVersion &version)
{
    this->version = version;
}

const QStringList &RFileInfo::getTags() const
{
    return this->tags;
}

void RFileInfo::setTags(const QStringList &tags)
{
    this->tags = tags;
}

const QByteArray &RFileInfo::getMd5Checksum() const
{
    return this->md5Checksum;
}

void RFileInfo::setMd5Checksum(const QByteArray &md5Checksum)
{
    this->md5Checksum = md5Checksum;
}

QString RFileInfo::toString() const
{
    QString infoString;

    infoString = this->id.toString(QUuid::WithoutBraces) + RFileInfo::delimiter +
                 QString::number(this->size) + RFileInfo::delimiter +
                 QString::number(this->creationDateTime) + RFileInfo::delimiter +
                 QString::number(this->updateDateTime) + RFileInfo::delimiter +
                 this->path + RFileInfo::delimiter +
                 this->accessRights.toString(RFileInfo::accessRightsDelimiter) + RFileInfo::delimiter +
                 this->version.toString(RFileInfo::versionDelimiter) + RFileInfo::delimiter +
                 this->tags.join(RFileInfo::tagsDelimiter) + RFileInfo::delimiter +
                 this->md5Checksum;

    return infoString;
}

RFileInfo RFileInfo::fromString(const QString &line)
{
    QStringList fields = line.split(RFileInfo::delimiter);

    int expectedNuberOfFields = 8;
    if (fields.size() != expectedNuberOfFields)
    {
        RLogger::warning("Invalid line. Has \'%d\' fields but expected are \'%d\'\n",fields.size(),expectedNuberOfFields);
    }

    int index = 0;

    RFileInfo info;
    if (fields.size() > index) info.setId(QUuid(fields.at(index++)));
    if (fields.size() > index) info.setSize(fields.at(index++).toLongLong());
    if (fields.size() > index) info.setCreationDateTime(fields.at(index++).toLongLong());
    if (fields.size() > index) info.setUpdateDateTime(fields.at(index++).toLongLong());
    if (fields.size() > index) info.setPath(fields.at(index++));
    if (fields.size() > index) info.setAccessRights(RAccessRights::fromString(fields.at(index++),RFileInfo::accessRightsDelimiter));
    if (fields.size() > index) info.setVersion(RVersion::fromString(fields.at(index++),RFileInfo::versionDelimiter));
    if (fields.size() > index) info.setTags(fields.at(index++).split(RFileInfo::tagsDelimiter));
    if (fields.size() > index) info.setMd5Checksum(fields.at(index++).toUtf8());

    return info;
}

RFileInfo RFileInfo::fromJson(const QJsonObject &json)
{
    RFileInfo fileInfo;

    if (const QJsonValue &v = json["id"]; v.isString())
    {
        fileInfo.id = QUuid(v.toString());
    }
    if (const QJsonValue &v = json["path"]; v.isString())
    {
        fileInfo.path = v.toString();
    }
    if (const QJsonValue &v = json["size"]; v.isString())
    {
        fileInfo.size = v.toString().toLongLong();
    }
    if (const QJsonValue &v = json["created"]; v.isString())
    {
        fileInfo.creationDateTime = QDateTime::fromString(v.toString()).toSecsSinceEpoch();
    }
    if (const QJsonValue &v = json["updated"]; v.isString())
    {
        fileInfo.updateDateTime = QDateTime::fromString(v.toString()).toSecsSinceEpoch();
    }
    if (const QJsonValue &v = json["access"]; v.isObject())
    {
        fileInfo.accessRights = RAccessRights::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json["version"]; v.isString())
    {
        fileInfo.version = RVersion(v.toString(),RFileInfo::versionDelimiter);
    }
    if (const QJsonValue &v = json["tags"]; v.isArray())
    {
        QStringList tags;
        QJsonArray jsonTags = v.toArray();
        for (const QJsonValue &av : std::as_const(jsonTags))
        {
            if (av.isString())
            {
                tags.append(av.toString());
            }
        }
        fileInfo.setTags(tags);
    }
    if (const QJsonValue &v = json["md5Checksum"]; v.isString())
    {
        fileInfo.md5Checksum = v.toString().toUtf8();
    }

    return fileInfo;
}

QJsonObject RFileInfo::toJson() const
{
    QJsonObject json;

    json["id"] = this->id.toString(QUuid::WithoutBraces);
    json["path"] = this->path;
    json["size"] = QString::number(this->size);
    json["created"] = QDateTime::fromSecsSinceEpoch(this->creationDateTime).toString();
    json["updated"] = QDateTime::fromSecsSinceEpoch(this->updateDateTime).toString();
    json["access"] = this->accessRights.toJson();
    json["version"] = this->version.toString(RFileInfo::versionDelimiter);

    QJsonArray jsonTags;
    for (const QString &tag : this->tags)
    {
        jsonTags.append(QJsonValue(tag));
    }
    json["tags"] = jsonTags;
    json["md5Checksum"] = QString(this->md5Checksum);

    return json;
}

bool RFileInfo::hasTag(const QString &tag) const
{
    return this->tags.contains(tag);
}

bool RFileInfo::hasTags(const QStringList &tags) const
{
    for (const QString &tag : tags)
    {
        if (!this->hasTag(tag))
        {
            return false;
        }
    }
    return true;
}

bool RFileInfo::isPathValid(const QString &path)
{
    static QRegularExpression re("^[^<>:\"/\\\\|?*]{1,256}$");
    return re.match(path).hasMatch();
}

bool RFileInfo::isTagValid(const QString &tag)
{
    static QRegularExpression re("^[a-zA-Z0-9_.-]{1,32}$");
    return re.match(tag).hasMatch();
}

QByteArray RFileInfo::findMd5Checksum(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f))
        {
            return hash.result().toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
        }
    }
    return QByteArray();
}

