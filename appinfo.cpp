#include "appinfo.h"

AppInfo::AppInfo(QObject *parent) : QObject(parent)
{
    process = new QProcess();
    connect(process, SIGNAL(started()), this, SLOT(started()));
    connect(process , SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(stopped(int)));
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
    return AppState::ON == state;
}

void AppInfo::started()
{
    state = AppState::ON;
    exitCode = -1;
    emit stateChanged();
}

void AppInfo::stopped(int code)
{
    state = manualOff ? AppState::MANUAL_OFF : AppState::SELF_OFF;
    exitCode = code;
    manualOff = false;
    emit stateChanged();
}

void AppInfo::startProcess()
{
    if (saveOutput) {
        QString now = QDateTime::currentDateTime().toString(DATE_TIME_FORMAT);
        QString logDir = qApp->applicationDirPath() + LOG_PATH + "/" + name;
        QDir dir = Utils::prepareDirectory(logDir);
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
    manualOff = true;
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
