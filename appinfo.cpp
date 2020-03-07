#include "appinfo.h"

#include <QDateTime>
#include <QDir>
#include <QDebug>
#include "consts.h"

AppInfo::AppInfo(QObject *parent) : QObject(parent)
{
    process = new QProcess();
    connect(process, SIGNAL(started()), this, SLOT(stateOn()));
    connect(process , SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(stateOff(int)));
}

AppInfo::~AppInfo()
{
    delete process;
}

QString AppInfo::printableName()
{
    QString result = name;
    if (!isOn() && exitCode != -1) {
        result += " (exit code " + QString::number(exitCode) + ")";
    }
    return result;
}

bool AppInfo::isOn()
{
    return state == AppState::ON;
}

void AppInfo::stateOn()
{
    state = AppState::ON;
    exitCode = -1;
    emit stateChanged();
}

void AppInfo::stateOff(int code)
{
    state = manual ? AppState::MANUAL_OFF : AppState::SELF_OFF;
    exitCode = code;
    manual = false;
    emit stateChanged();
}

void AppInfo::startProcess(bool withLogs=false)
{
    if (withLogs) {
        QString now = QDateTime::currentDateTime().toString(DATE_TIME_FORMAT);
        QString logDir = qApp->applicationDirPath() + LOG_PATH + "/" + name;
        QDir dir(logDir);
        if (!dir.exists())
            dir.mkpath(".");
        process->setStandardOutputFile(logDir + "/" + now + ".log");
        process->setStandardErrorFile(logDir + "/" + now + "_error.log");
    }
    if (workingDir != "") {
        process->setWorkingDirectory(workingDir);
    }
    process->start(path);
}

void AppInfo::stopProcess()
{
    manual = true;
    process->terminate();
}

AppInfo* AppInfo::clone()
{
    AppInfo *info = new AppInfo();
    info->name = name + " (kopia)";
    info->path = path;
    info->workingDir = workingDir;
    info->saveOutput = saveOutput;
    return info;
}
