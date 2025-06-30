#include <QJsonArray>

#include "rcl_access_mode.h"

const RAccessMode::ModeMask RAccessMode::R    = RAccessMode::Read;
const RAccessMode::ModeMask RAccessMode::W    =                     RAccessMode::Write;
const RAccessMode::ModeMask RAccessMode::X    =                                          RAccessMode::Execute;
const RAccessMode::ModeMask RAccessMode::RW   = RAccessMode::Read | RAccessMode::Write;
const RAccessMode::ModeMask RAccessMode::RX   = RAccessMode::Read |                      RAccessMode::Execute;
const RAccessMode::ModeMask RAccessMode::WX   =                     RAccessMode::Write | RAccessMode::Execute;
const RAccessMode::ModeMask RAccessMode::RWX  = RAccessMode::Read | RAccessMode::Write | RAccessMode::Execute;

void RAccessMode::_init(const RAccessMode *pAccessMode)
{
    if (pAccessMode)
    {
        this->userModeMask = pAccessMode->userModeMask;
        this->groupModeMask = pAccessMode->groupModeMask;
        this->otherModeMask = pAccessMode->otherModeMask;
    }
}

RAccessMode::RAccessMode()
    : userModeMask(None)
    , groupModeMask(None)
    , otherModeMask(None)
{
    this->_init();
}

RAccessMode::RAccessMode(const RAccessMode &accessMode)
{
    this->_init(&accessMode);
}

RAccessMode &RAccessMode::operator=(const RAccessMode &accessMode)
{
    this->_init(&accessMode);
    return (*this);
}

bool RAccessMode::isValid() const
{
    return (RAccessMode::modeMaskIsValid(this->userModeMask) &&
            RAccessMode::modeMaskIsValid(this->groupModeMask) &&
            RAccessMode::modeMaskIsValid(this->otherModeMask));
}

RAccessMode::ModeMask RAccessMode::getUserModeMask() const
{
    return this->userModeMask;
}

void RAccessMode::setUserModeMask(ModeMask modeMask)
{
    this->userModeMask = modeMask;
}

RAccessMode::ModeMask RAccessMode::getGroupModeMask() const
{
    return this->groupModeMask;
}

void RAccessMode::setGroupModeMask(ModeMask modeMask)
{
    this->groupModeMask = modeMask;
}

RAccessMode::ModeMask RAccessMode::getOtherModeMask() const
{
    return this->otherModeMask;
}

void RAccessMode::setOtherModeMask(ModeMask modeMask)
{
    this->otherModeMask = modeMask;
}

RAccessMode RAccessMode::fromString(const QString &line, const char delimiter)
{
    RAccessMode accessMode;

    QStringList fields = line.split(delimiter);
    if (fields.count() > 0) accessMode.userModeMask = ModeMask(fields.at(0).toInt());
    if (fields.count() > 1) accessMode.groupModeMask = ModeMask(fields.at(1).toInt());
    if (fields.count() > 2) accessMode.otherModeMask = ModeMask(fields.at(2).toInt());

    return accessMode;
}

QString RAccessMode::toString(const char delimiter) const
{
    QString infoString;

    infoString = QString::number(this->userModeMask) + delimiter +
                 QString::number(this->groupModeMask) + delimiter +
                 QString::number(this->otherModeMask);

    return infoString;
}

RAccessMode RAccessMode::fromHuman(const QString &line)
{
    RAccessMode::ModeMask userMask = RAccessMode::None;
    RAccessMode::ModeMask groupMask = RAccessMode::None;
    RAccessMode::ModeMask otherMask = RAccessMode::None;

    for (qsizetype i=0; i<line.size() && i<3;i++)
    {
        userMask |= RAccessMode::humanToMode(line.at(i).toLatin1());
    }

    for (qsizetype i=3; i<line.size() && i<6;i++)
    {
        groupMask |= RAccessMode::humanToMode(line.at(i).toLatin1());
    }

    for (qsizetype i=6; i<line.size() && i<9;i++)
    {
        otherMask |= RAccessMode::humanToMode(line.at(i).toLatin1());
    }

    RAccessMode accessMode;
    accessMode.setUserModeMask(userMask);
    accessMode.setGroupModeMask(groupMask);
    accessMode.setOtherModeMask(otherMask);

    return accessMode;
}

QString RAccessMode::toHuman() const
{
    QString humanString = RAccessMode::modeMaskToHuman(this->userModeMask)
                        + RAccessMode::modeMaskToHuman(this->groupModeMask)
                        + RAccessMode::modeMaskToHuman(this->otherModeMask);

    return humanString;
}

RAccessMode RAccessMode::fromJson(const QJsonObject &json)
{
    RAccessMode accessMode;

    if (const QJsonValue &v = json["user"]; v.isDouble())
    {
        accessMode.userModeMask = v.toInt();
    }
    if (const QJsonValue &v = json["group"]; v.isDouble())
    {
        accessMode.groupModeMask = v.toInt();
    }
    if (const QJsonValue &v = json["other"]; v.isDouble())
    {
        accessMode.otherModeMask = v.toInt();
    }

    return accessMode;
}

QJsonObject RAccessMode::toJson() const
{
    QJsonObject json;

    json["user"] = this->userModeMask;
    json["group"] = this->groupModeMask;
    json["other"] = this->otherModeMask;

    return json;
}

bool RAccessMode::modeMaskIsValid(ModeMask modeMask)
{
    return (modeMask == RAccessMode::None ||
            modeMask == RAccessMode::R    ||
            modeMask == RAccessMode::W    ||
            modeMask == RAccessMode::X    ||
            modeMask == RAccessMode::RW   ||
            modeMask == RAccessMode::RX   ||
            modeMask == RAccessMode::WX   ||
            modeMask == RAccessMode::RWX);
}

RAccessMode::Mode RAccessMode::humanToMode(char c)
{
    switch (c)
    {
        case 'r': return RAccessMode::Read;
        case 'w': return RAccessMode::Write;
        case 'x': return RAccessMode::Execute;
        default: return RAccessMode::None;
    }
}

char RAccessMode::modeToHuman(Mode mode)
{
    switch (mode)
    {
        case RAccessMode::Read: return 'r';
        case RAccessMode::Write: return 'w';
        case RAccessMode::Execute: return 'x';
        default: return '-';
    }
}

QString RAccessMode::modeMaskToHuman(ModeMask modeMask)
{
    QString humanString;

    humanString += (modeMask & RAccessMode::Read) ? RAccessMode::modeToHuman(RAccessMode::Read) : RAccessMode::modeToHuman(RAccessMode::None);
    humanString += (modeMask & RAccessMode::Write) ? RAccessMode::modeToHuman(RAccessMode::Write) : RAccessMode::modeToHuman(RAccessMode::None);
    humanString += (modeMask & RAccessMode::Execute) ? RAccessMode::modeToHuman(RAccessMode::Execute) : RAccessMode::modeToHuman(RAccessMode::None);

    return humanString;
}

QList<RAccessMode::Mode> RAccessMode::modeMaskToModes(ModeMask modeMask)
{
    QList<RAccessMode::Mode> modes;

    if (modeMask & RAccessMode::Read)    modes.append(RAccessMode::Read);
    if (modeMask & RAccessMode::Write)   modes.append(RAccessMode::Write);
    if (modeMask & RAccessMode::Execute) modes.append(RAccessMode::Execute);

    return modes;
}

RAccessMode::ModeMask RAccessMode::filterModeMask(ModeMask modeMask, ModeMask filter)
{
    QList<RAccessMode::Mode> modeList = RAccessMode::modeMaskToModes(modeMask);

    ModeMask filteredModeMask = RAccessMode::None;
    for (RAccessMode::Mode mode : std::as_const(modeList))
    {
        if (mode & filter)
        {
            filteredModeMask |= mode;
        }
    }
    return filteredModeMask;
}
