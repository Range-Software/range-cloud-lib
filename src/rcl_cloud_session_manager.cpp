#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rcl_http_client_settings.h"
#include "rcl_cloud_session_manager.h"

const QString RCloudSessionManager::sessionNameTemplate = tr("New Cloud session");

const QString RCloudSessionManager::DefaultRangeSession::name = tr("Range Software");
const QString RCloudSessionManager::DefaultRangeSession::host = "range-software.com";
const uint    RCloudSessionManager::DefaultRangeSession::publicPort = 4011;
const uint    RCloudSessionManager::DefaultRangeSession::privatePort = 4012;

const QString RCloudSessionManager::DefaultCloudSession::name = tr("Range Cloud");
const QString RCloudSessionManager::DefaultCloudSession::host = "range-software.com";
const uint    RCloudSessionManager::DefaultCloudSession::publicPort = 4021;
const uint    RCloudSessionManager::DefaultCloudSession::privatePort = 4022;

RCloudSessionManager::RCloudSessionManager(QObject *parent)
    : QObject(parent)
{
    this->sessions.append(RCloudSessionManager::getDefaultRangeSession());
    this->sessions.append(RCloudSessionManager::getDefaultCloudSession());
    this->activeSessionName = this->sessions.at(1).getName();
}

void RCloudSessionManager::read(const QString &fileName)
{
    RLogger::info("Reading session file \"%s\".\n",fileName.toUtf8().constData());

    QFile inFile(fileName);

    if (!inFile.exists())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"Session file \"%s\" does not exist.",fileName.toUtf8().constData());
    }

    if(!inFile.open(QIODevice::ReadOnly))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,
                     "Failed to open session file \"%s\" for reading. %s.",
                     inFile.fileName().toUtf8().constData(),
                     inFile.errorString().toUtf8().constData());
    }

    QByteArray byteArray = inFile.readAll();
    RLogger::info("Successfuly read \"%ld\" bytes from \"%s\".\n",byteArray.size(),inFile.fileName().toUtf8().constData());

    this->fromJson(QJsonDocument::fromJson(byteArray).object());

    inFile.close();
}

void RCloudSessionManager::write(const QString &fileName) const
{
    RLogger::info("Writing session file \"%s\".\n",fileName.toUtf8().constData());

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
}

void RCloudSessionManager::setActiveSessionName(const QString &sessionName)
{
    if (this->activeSessionName != sessionName)
    {
        this->activeSessionName = sessionName;
        emit this->activeSessionChanged(this->findSession(this->activeSessionName));
    }
}

QString RCloudSessionManager::findActiveSessionName() const
{
    if (this->sessions.size() == 0)
    {
        return QString();
    }

    QString sessionName = this->activeSessionName;
    for (const RCloudSessionInfo &sessionInfo : this->sessions)
    {
        if (sessionInfo.getName() == sessionName)
        {
            return sessionName;
        }
    }

    return this->sessions.at(0).getName();
}

QStringList RCloudSessionManager::getSessionNames() const
{
    QStringList sessionNames;

    for (const RCloudSessionInfo &sessionInfo : this->sessions)
    {
        sessionNames.append(sessionInfo.getName());
    }

    return sessionNames;
}

RCloudSessionInfo RCloudSessionManager::findSession(const QString &sessionName) const
{
    for (const RCloudSessionInfo &sessionInfo : this->sessions)
    {
        if (sessionInfo.getName() == sessionName)
        {
            return sessionInfo;
        }
    }
    return RCloudSessionInfo();
}

void RCloudSessionManager::renameSession(const QString &oldSessionName, const QString &newSessionName)
{
    for (RCloudSessionInfo &sessionInfo : this->sessions)
    {
        if (sessionInfo.getName() == oldSessionName)
        {
            sessionInfo.setName(newSessionName);
            emit this->sessionRenamed(oldSessionName,newSessionName);
            if (this->activeSessionName == oldSessionName)
            {
                this->setActiveSessionName(this->activeSessionName);
            }
        }
    }
}

qsizetype RCloudSessionManager::removeSession(const QString &sessionName)
{
    qsizetype nRemoved = this->sessions.removeIf(
        [sessionName](const RCloudSessionInfo &sessionInfo)
        {
            return sessionInfo.getName() == sessionName;
        }
    );

    if (nRemoved)
    {
        emit this->sessionRemoved(sessionName);
    }

    return nRemoved;
}

void RCloudSessionManager::removeAllSessions()
{
    for (RCloudSessionInfo &sessionInfo : this->sessions)
    {
        emit this->sessionRemoved(sessionInfo.getName());
    }
    this->sessions.clear();
    emit this->activeSessionChanged(RCloudSessionInfo());
}

QString RCloudSessionManager::guessNewSessionName() const
{
    QString nesSessionName = RCloudSessionManager::sessionNameTemplate;
    uint cntr = 1;
    for (const RCloudSessionInfo &sessionInfo : this->sessions)
    {
        if (sessionInfo.getName() == nesSessionName)
        {
            nesSessionName = RCloudSessionManager::sessionNameTemplate + " (" + QString::number(++cntr) + ")";
        }
    }
    return nesSessionName;
}

RCloudSessionInfo RCloudSessionManager::getDefaultRangeSession()
{
    RCloudSessionInfo sessionInfo;

    sessionInfo.setName(RCloudSessionManager::DefaultRangeSession::name);
    sessionInfo.setHostName(RCloudSessionManager::DefaultRangeSession::host);
    sessionInfo.setPublicPort(RCloudSessionManager::DefaultRangeSession::publicPort);
    sessionInfo.setPrivatePort(RCloudSessionManager::DefaultRangeSession::privatePort);
    sessionInfo.setTimeout(RHttpClientSettings::defaultTimeout);

    return sessionInfo;
}

RCloudSessionInfo RCloudSessionManager::getDefaultCloudSession()
{
    RCloudSessionInfo sessionInfo;

    sessionInfo.setName(RCloudSessionManager::DefaultCloudSession::name);
    sessionInfo.setHostName(RCloudSessionManager::DefaultCloudSession::host);
    sessionInfo.setPublicPort(RCloudSessionManager::DefaultCloudSession::publicPort);
    sessionInfo.setPrivatePort(RCloudSessionManager::DefaultCloudSession::privatePort);
    sessionInfo.setTimeout(RHttpClientSettings::defaultTimeout);

    return sessionInfo;
}

void RCloudSessionManager::insertSession(const RCloudSessionInfo &sessionInfo)
{
    bool sessionFound = false;
    for (RCloudSessionInfo &si : this->sessions)
    {
        if (si.getName() == sessionInfo.getName())
        {
            si = sessionInfo;
            sessionFound = true;
        }
    }
    if (!sessionFound)
    {
        this->sessions.append(sessionInfo);
    }

    emit this->sessionInserted(sessionInfo.getName());

    if (this->activeSessionName.isEmpty())
    {
        this->setActiveSessionName(this->activeSessionName);
    }
    if (sessionInfo.getName() == this->activeSessionName)
    {
        emit this->activeSessionChanged(this->findSession(this->activeSessionName));
    }
}

void RCloudSessionManager::fromJson(const QJsonObject &json)
{
    for (const RCloudSessionInfo &sessionInfo : std::as_const(this->sessions))
    {
        emit this->sessionRemoved(sessionInfo.getName());
    }

    if (const QJsonValue &v = json["activeSessions"]; v.isString())
    {
        this->activeSessionName = v.toString();
    }
    if (const QJsonValue &v = json["sessions"]; v.isArray())
    {
        const QJsonArray &actionsArray = v.toArray();
        this->sessions.clear();
        this->sessions.reserve(actionsArray.size());
        for (const QJsonValue &action : actionsArray)
        {
            this->sessions.append(RCloudSessionInfo::fromJson(action.toObject()));
        }
    }

    bool activeSessionFound = false;
    for (const RCloudSessionInfo &sessionInfo : std::as_const(this->sessions))
    {
        if (sessionInfo.getName() == this->activeSessionName)
        {
            activeSessionFound = true;
            break;
        }
    }
    if (!activeSessionFound)
    {
        this->activeSessionName = this->sessions.at(0).getName();
    }

    for (const RCloudSessionInfo &sessionInfo : std::as_const(this->sessions))
    {
        emit this->sessionInserted(sessionInfo.getName());
    }

    emit this->activeSessionChanged(this->findSession(this->activeSessionName));
}

QJsonObject RCloudSessionManager::toJson() const
{
    QJsonObject json;

    // Active session
    json["activeSessions"] = this->activeSessionName;
    // Sessions
    QJsonArray actionsArray;
    for (const RCloudSessionInfo &sessionInfo : this->sessions)
    {
        actionsArray.append(sessionInfo.toJson());
    }
    json["sessions"] = actionsArray;

    return json;
}
