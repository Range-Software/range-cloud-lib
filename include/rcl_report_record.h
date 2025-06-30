#ifndef RCL_REPORT_RECORD_H
#define RCL_REPORT_RECORD_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class RReportRecord
{

    public:

        static constexpr uint defaultMaxReportLength = 1048576;
        static constexpr uint defaultMaxCommentLength = 1024;

    protected:

        qint64 creationDateTime;
        QString report;
        QString comment;

    private:

        //! Internal initialization function.
        void _init(const RReportRecord *pReportRecord = nullptr);

    public:

        //! Constructor.
        RReportRecord();

        //! Copy constructor.
        RReportRecord(const RReportRecord &reportRecord);

        //! Destructor.
        ~RReportRecord() {};

        //! Assignment operator.
        RReportRecord &operator =(const RReportRecord &reportRecord);

        //! Return const reference to time stamp.
        qint64 getCreationDateTime() const;

        //! Set new time stamp.
        void setCreationDateTime(qint64 creationDateTime);

        //! Return const reference to report.
        const QString &getReport() const;

        //! Set new report.
        void setReport(const QString &report);

        //! Return const reference to comment.
        const QString &getComment() const;

        //! Set new comment.
        void setComment(const QString &comment);

        //! To string.
        QString toString(const char delimiter = ',') const;

        //! Create report record object from Json.
        static RReportRecord fromJson(const QJsonObject &json);

        //! Create Json from report record object.
        QJsonObject toJson() const;

};

#endif // RCL_REPORT_RECORD_H
