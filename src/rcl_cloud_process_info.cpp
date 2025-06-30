#include <QJsonArray>

#include "rcl_cloud_process_info.h"

void RCloudProcessInfo::_init(const RCloudProcessInfo *pProcessInfo)
{
    if (pProcessInfo)
    {
        this->name = pProcessInfo->name;
        this->executable = pProcessInfo->executable;
        this->arguments = pProcessInfo->arguments;
        this->accessRights = pProcessInfo->accessRights;
    }
}

RCloudProcessInfo::RCloudProcessInfo()
{
    this->_init();
}

RCloudProcessInfo::RCloudProcessInfo(const RCloudProcessInfo &processInfo)
{
    this->_init(&processInfo);
}

RCloudProcessInfo::~RCloudProcessInfo()
{

}

RCloudProcessInfo &RCloudProcessInfo::operator=(const RCloudProcessInfo &processInfo)
{
    this->_init(&processInfo);
    return (*this);
}

const QString &RCloudProcessInfo::getName() const
{
    return this->name;
}

void RCloudProcessInfo::setName(const QString &name)
{
    this->name = name;
}

const QString &RCloudProcessInfo::getExecutable() const
{
    return this->executable;
}

QString &RCloudProcessInfo::getExecutable()
{
    return this->executable;
}

void RCloudProcessInfo::setExecutable(const QString &executable)
{
    this->executable = executable;
}

const QStringList &RCloudProcessInfo::getArguments() const
{
    return this->arguments;
}

void RCloudProcessInfo::setArguments(const QStringList &arguments)
{
    this->arguments = arguments;
}

const RAccessRights &RCloudProcessInfo::getAccessRights() const
{
    return this->accessRights;
}

void RCloudProcessInfo::setAccessRights(const RAccessRights &accessRights)
{
    this->accessRights = accessRights;
}

QString RCloudProcessInfo::buildCommand() const
{
    QString command = this->executable;

    for (const QString &argument : this->arguments)
    {
        command += QString(" %1").arg(argument);
    }

    return command;
}

RCloudProcessInfo RCloudProcessInfo::fromJson(const QJsonObject &json)
{
    RCloudProcessInfo processInfo;

    if (const QJsonValue &v = json["name"]; v.isString())
    {
        processInfo.name = v.toString();
    }

    if (const QJsonValue &v = json["executable"]; v.isString())
    {
        processInfo.executable = v.toString();
    }

    if (const QJsonValue &v = json["arguments"]; v.isArray())
    {
        const QJsonArray &arguments = v.toArray();

        processInfo.arguments.reserve(arguments.size());
        for (const QJsonValue &argument : arguments)
        {
            if (argument.isString())
            {
                processInfo.arguments.append(argument.toString());
            }
        }
    }

    if (const QJsonValue &v = json["access"]; v.isObject())
    {
        processInfo.accessRights = RAccessRights::fromJson(v.toObject());
    }

    return processInfo;
}

QJsonObject RCloudProcessInfo::toJson() const
{
    QJsonObject json;

    json["name"] = this->name;
    json["executable"] = this->executable;

    QJsonArray argumentsArray;
    for (const QString &argument : this->arguments)
    {
        argumentsArray.append(argument);
    }
    json["arguments"] = argumentsArray;

    json["access"] = this->accessRights.toJson();

    return json;
}
