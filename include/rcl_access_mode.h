#ifndef RCL_ACCESS_MODE_H
#define RCL_ACCESS_MODE_H

#include <QJsonObject>
#include <QString>

class RAccessMode
{

    public:

        typedef int ModeMask;

        enum Mode
        {
            None    = 0,
            Execute = 1 << 0,
            Write   = 1 << 1,
            Read    = 1 << 2,
        };

        static const ModeMask X;
        static const ModeMask W;
        static const ModeMask R;
        static const ModeMask RW;
        static const ModeMask RX;
        static const ModeMask WX;
        static const ModeMask RWX;

    protected:

        ModeMask userModeMask;
        ModeMask groupModeMask;
        ModeMask otherModeMask;

    private:

        //! Internal initialization function.
        void _init(const RAccessMode *pAccessMode = nullptr);

    public:

        //! Constructor.
        RAccessMode();

        //! Copy constructor.
        RAccessMode(const RAccessMode &accessMode);

        //! Destructor.
        ~RAccessMode() {};

        //! Assignment operator.
        RAccessMode &operator =(const RAccessMode &accessMode);

        //! Return true if values are valid.
        bool isValid() const;

        //! Return user mode mask.
        ModeMask getUserModeMask() const;

        //! Set user mode mask.
        void setUserModeMask(ModeMask modeMask);

        //! Return group mode mask.
        ModeMask getGroupModeMask() const;

        //! Set group mode mask.
        void setGroupModeMask(ModeMask modeMask);

        //! Return other mode mask.
        ModeMask getOtherModeMask() const;

        //! Set other mode mask.
        void setOtherModeMask(ModeMask modeMask);

        //! From string line.
        static RAccessMode fromString(const QString &line, const char delimiter = ',');

        //! To string.
        QString toString(const char delimiter = ',') const;

        //! From human readable string line.
        static RAccessMode fromHuman(const QString &line);

        //! To human readable string.
        QString toHuman() const;

        //! Create user object from Json.
        static RAccessMode fromJson(const QJsonObject &json);

        //! Create Json from user object.
        QJsonObject toJson() const;

        //! Validate mode mask.
        static bool modeMaskIsValid(ModeMask modeMask);

        //! Convert human char to mode.
        static RAccessMode::Mode humanToMode(char c);

        //! Convert mode to human char.
        static char modeToHuman(RAccessMode::Mode mode);

        //! Convert mode-mask to human string.
        static QString modeMaskToHuman(RAccessMode::ModeMask modeMask);

        //! Convert mode-mask to list of modes.
        static QList<RAccessMode::Mode> modeMaskToModes(RAccessMode::ModeMask modeMask);

        //! Apply filter on mode-mask. Output mode-mask can have same or less modes.
        static RAccessMode::ModeMask filterModeMask(RAccessMode::ModeMask modeMask, RAccessMode::ModeMask filter);

};

#endif // RCL_ACCESS_MODE_H
