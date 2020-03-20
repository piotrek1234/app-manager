#ifndef APPINFO_H
#define APPINFO_H

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QString>
#include "appstate.h"
#include "consts.h"
#include "utils.h"

class AppInfo : public QObject
{
Q_OBJECT

public:
    explicit AppInfo(QObject *parent = nullptr);
    ~AppInfo();
    AppInfo* clone();
    QString printableName();
    bool isOn();
    QString name = DEFAULT_APP_NAME;
    QString path;
    QString workingDir;
    bool saveOutput = false;
    AppState state = AppState::INITIAL_OFF;
public slots:
    void startProcess();
    void stopProcess();
private slots:
    void started();
    void stopped(int code);
private:
    QProcess *process;
    int exitCode = -1;
    bool manualOff = false;
signals:
    void stateChanged(AppInfo* info);
};

#endif // APPINFO_H
