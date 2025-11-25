#include <QDateTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rcl_file_manager_cache.h"

RFileManagerCache::RFileManagerCache(QObject *parent)
    : QObject{parent}
    , localUpdateDateTime{0}
    , remoteUpdateDateTime{0}
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_OUT;
}

void RFileManagerCache::read(const QString &fileName)
{
    R_LOG_TRACE_IN;
    RLogger::info("Reading session file \"%s\".\n",fileName.toUtf8().constData());

    QFile inFile(fileName);

    if (!inFile.exists())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"Cloud file manager cache file \"%s\" does not exist.",fileName.toUtf8().constData());
    }

    if(!inFile.open(QIODevice::ReadOnly))
    {
        R_LOG_TRACE_OUT;
        throw RError(RError::Type::OpenFile,R_ERROR_REF,
                     "Failed to open cloud file manager cache file \"%s\" for reading. %s.",
                     inFile.fileName().toUtf8().constData(),
                     inFile.errorString().toUtf8().constData());
    }

    QByteArray byteArray = inFile.readAll();
    RLogger::info("Successfuly read \"%ld\" bytes from \"%s\".\n",byteArray.size(),inFile.fileName().toUtf8().constData());

    this->fromJson(QJsonDocument::fromJson(byteArray).object());

    inFile.close();
    R_LOG_TRACE_OUT;
}

void RFileManagerCache::write(const QString &fileName) const
{
    R_LOG_TRACE_IN;
    RLogger::info("Writing cloud file manager cache file \"%s\".\n",fileName.toUtf8().constData());

    QFile outFile(fileName);

    if(!outFile.open(QIODevice::WriteOnly))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,
                     "Failed to open actions file \"%s\" for writing. %s.",
                     outFile.fileName().toUtf8().constData(),
                     outFile.errorString().toUtf8().constData());
    }

    qint64 bytesOut = outFile.write(QJsonDocument(this->toJson()).toJson());

    RLogger::info("Successfuly wrote \"%ld\" bytes to \"%s\".\n",bytesOut,outFile.fileName().toUtf8().constData());

    outFile.close();
    R_LOG_TRACE_OUT;
}

const qint64 &RFileManagerCache::getLocalUpdateDateTime() const
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->localUpdateDateTime);
}

const qint64 &RFileManagerCache::getRemoteUpdateDateTime() const
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_RETURN(this->remoteUpdateDateTime);
}

void RFileManagerCache::resetCurrentDateTime()
{
    R_LOG_TRACE_IN;
    //! Set last update times to current time.
    this->localUpdateDateTime = QDateTime::currentSecsSinceEpoch();
    this->remoteUpdateDateTime = this->localUpdateDateTime;
    R_LOG_TRACE_OUT;
}

void RFileManagerCache::fromJson(const QJsonObject &json)
{
    R_LOG_TRACE_IN;
    if (const QJsonValue &v = json["localUpdated"]; v.isString())
    {
        this->localUpdateDateTime = QDateTime::fromString(v.toString()).toSecsSinceEpoch();
    }
    if (const QJsonValue &v = json["remoteUpdated"]; v.isString())
    {
        this->remoteUpdateDateTime = QDateTime::fromString(v.toString()).toSecsSinceEpoch();
    }
    R_LOG_TRACE_OUT;
}

QJsonObject RFileManagerCache::toJson() const
{
    R_LOG_TRACE_IN;
    QJsonObject json;

    json["localUpdated"] = QDateTime::fromSecsSinceEpoch(this->localUpdateDateTime).toString();
    json["remoteUpdated"] = QDateTime::fromSecsSinceEpoch(this->remoteUpdateDateTime).toString();

    R_LOG_TRACE_RETURN(json);
}
