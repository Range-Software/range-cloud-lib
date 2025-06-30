#include "rcl_report_record.h"

void RReportRecord::_init(const RReportRecord *pReportRecord)
{
    if (pReportRecord)
    {
        this->creationDateTime = pReportRecord->creationDateTime;
        this->report = pReportRecord->report;
        this->comment = pReportRecord->comment;
    }
}

RReportRecord::RReportRecord()
    : creationDateTime{QDateTime::currentSecsSinceEpoch()}
{
    this->_init();
}

RReportRecord::RReportRecord(const RReportRecord &reportRecord)
{
    this->_init(&reportRecord);
}

RReportRecord &RReportRecord::operator =(const RReportRecord &reportRecord)
{
    this->_init(&reportRecord);
    return (*this);
}

qint64 RReportRecord::getCreationDateTime() const
{
    return this->creationDateTime;
}

void RReportRecord::setCreationDateTime(qint64 creationDateTime)
{
    this->creationDateTime = creationDateTime;
}

const QString &RReportRecord::getReport() const
{
    return this->report;
}

void RReportRecord::setReport(const QString &report)
{
    this->report = report;
}

const QString &RReportRecord::getComment() const
{
    return this->comment;
}

void RReportRecord::setComment(const QString &comment)
{
    this->comment = comment;
}

QString RReportRecord::toString(const char delimiter) const
{
    QString infoString;

    infoString = QDateTime::fromSecsSinceEpoch(this->creationDateTime).toString() + delimiter +
                 this->report.toUtf8().toBase64() + delimiter +
                 this->comment.toUtf8().toBase64();

    return infoString;
}

RReportRecord RReportRecord::fromJson(const QJsonObject &json)
{
    RReportRecord reportRecord;

    if (const QJsonValue &v = json["created"]; v.isString())
    {
        reportRecord.creationDateTime = QDateTime::fromString(v.toString()).toSecsSinceEpoch();
    }
    if (const QJsonValue &v = json["report"]; v.isString())
    {
        reportRecord.report = QByteArray::fromBase64(v.toString().toUtf8());
    }
    if (const QJsonValue &v = json["comment"]; v.isString())
    {
        reportRecord.comment = QByteArray::fromBase64(v.toString().toUtf8());
    }

    return reportRecord;
}

QJsonObject RReportRecord::toJson() const
{
    QJsonObject json;

    json["created"] = QDateTime::fromSecsSinceEpoch(this->creationDateTime).toString();
    json["report"] = QString(this->report.toUtf8().toBase64());
    json["comment"] = QString(this->comment.toUtf8().toBase64());

    return json;
}
