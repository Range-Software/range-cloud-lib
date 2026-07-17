#ifndef RCL_AI_QUERY_H
#define RCL_AI_QUERY_H

#include <QJsonObject>
#include <QString>
#include <QUuid>

//! AI query: the question asked by the user together with optional
//! supporting information (referenced resource file, context, language).
class RAIQuery
{

    protected:

        //! Question asked by the user.
        QString question;
        //! Reference to a resource file stored in Range Cloud (optional).
        QUuid fileId;
        //! Additional free-form context supplied by the client (optional).
        QString context;
        //! Preferred response language (optional, e.g. "en", "de").
        QString language;

    private:

        //! Internal initialization function.
        void _init(const RAIQuery *pAIQuery = nullptr);

    public:

        //! Constructor.
        RAIQuery();

        //! Copy constructor.
        RAIQuery(const RAIQuery &aiQuery);

        //! Destructor.
        ~RAIQuery();

        //! Assignment operator.
        RAIQuery &operator =(const RAIQuery &aiQuery);

        //! Return const reference to question.
        const QString &getQuestion() const;

        //! Set new question.
        void setQuestion(const QString &question);

        //! Return const reference to resource file id.
        const QUuid &getFileId() const;

        //! Set new resource file id.
        void setFileId(const QUuid &fileId);

        //! Return const reference to context.
        const QString &getContext() const;

        //! Set new context.
        void setContext(const QString &context);

        //! Return const reference to preferred response language.
        const QString &getLanguage() const;

        //! Set new preferred response language.
        void setLanguage(const QString &language);

        //! Check whether the query is empty (no question).
        bool isEmpty() const;

        //! Build prompt text (question with optional context and language instruction).
        QString buildPrompt() const;

        //! Create AI query object from Json.
        static RAIQuery fromJson(const QJsonObject &json);

        //! Create Json from AI query object.
        QJsonObject toJson() const;

};

#endif // RCL_AI_QUERY_H
