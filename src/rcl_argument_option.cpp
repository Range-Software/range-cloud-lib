#include <rbl_utils.h>

#include "rcl_argument_option.h"

void CArgumentOption::_init(const CArgumentOption *pArgumentOption)
{
    if (pArgumentOption)
    {
        this->flag = pArgumentOption->flag;
        this->type = pArgumentOption->type;
        this->value = pArgumentOption->value;
        this->description = pArgumentOption->description;
        this->category = pArgumentOption->category;
        this->exclusive = pArgumentOption->exclusive;
    }
}

CArgumentOption::CArgumentOption()
    : type(None)
{
    this->_init();
}

CArgumentOption::CArgumentOption(const QString &flag, CArgumentOption::Type type, const QVariant &value, const QString &description, CArgumentOption::Category category, bool exclusive)
    : flag(flag)
    , type(type)
    , value(value)
    , description(description)
    , category(category)
    , exclusive(exclusive)
{
    this->_init();
}

CArgumentOption::CArgumentOption(const CArgumentOption &argumentOption)
{
    this->_init(&argumentOption);
}

CArgumentOption::~CArgumentOption()
{

}

CArgumentOption &CArgumentOption::operator =(const CArgumentOption &argumentOption)
{
    this->_init(&argumentOption);
    return (*this);
}

const QString &CArgumentOption::getFlag() const
{
    return this->flag;
}

void CArgumentOption::setFlag(const QString &flag)
{
    this->flag = flag;
}

CArgumentOption::Type CArgumentOption::getType() const
{
    return this->type;
}

void CArgumentOption::setType(CArgumentOption::Type type)
{
    this->type = type;
}

const QVariant &CArgumentOption::getValue() const
{
    return this->value;
}

void CArgumentOption::setValue(const QVariant &value)
{
    this->value = value;
}

const QString &CArgumentOption::getDescription() const
{
    return this->description;
}

void CArgumentOption::setDescription(const QString &description)
{
    this->description = description;
}

CArgumentOption::Category CArgumentOption::getCategory() const
{
    return this->category;
}

void CArgumentOption::setCategory(CArgumentOption::Category category)
{
    this->category = category;
}

bool CArgumentOption::getExclusive() const
{
    return this->exclusive;
}

void CArgumentOption::setExclusive(bool exclusive)
{
    this->exclusive = exclusive;
}

QString CArgumentOption::getFlagMessage() const
{
    QString message;
    message += "--" + this->flag;
    switch (this->type)
    {
        case Char:
        {
            message += "=<C>";
            break;
        }
        case String:
        {
            message += "=<STRING>";
            break;
        }
        case StringList:
        {
            message += "=<STRING1[,STRING2,...]>";
            break;
        }
        case Path:
        {
            message += "=<PATH>";
            break;
        }
        case Integer:
        {
            message += "=<N>";
            break;
        }
        case Real:
        {
            message += "=<REAL NUMBER>";
            break;
        }
        case Date:
        {
            message += "=<DD.MM.YYYY>";
            break;
        }
        default:
        {
            break;
        }
    }
    return message;
}

QString CArgumentOption::getHelpMessage(uint width) const
{
    QString message = this->getFlagMessage();
    message += " ";
    for (uint i=uint(message.length());i<width;i++)
    {
        message += " ";
    }
    message += this->description;
    if (this->type != Switch && !this->value.isNull())
    {
        message += " (default=" + this->value.toString() + ")";
    }
    return message;
}

CArgumentOption CArgumentOption::generateHelpOption()
{
    return CArgumentOption("help",Switch,QVariant(false),"Print help",Help,true);
}

CArgumentOption CArgumentOption::generateVersionOption()
{
    return CArgumentOption("version",Switch,QVariant(RVendor::version.toString()),"Print version",Help,true);
}

bool CArgumentOption::isOption(const QString &argument)
{
    if (argument.size() == 2)
    {
        if (argument[0] == '-' && argument[1] != '-')
        {
            return true;
        }
    }
    else if (argument.size() >= 4)
    {
        if (argument[0] == '-' && argument[1] == '-')
        {
            return true;
        }
    }
    return false;
}
