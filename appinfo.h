#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QApplication>
#include "offstate.h"

class AppInfo : public QObject
{
Q_OBJECT

public:
    explicit AppInfo(QObject *parent = nullptr);
    ~AppInfo();
    QProcess *process;
    QString name = "app";
    QString path;
    QString workingDir;
    bool saveOutput = false;
    AppState state = AppState::INITIAL_OFF;
    int exitCode = -1;
    QString printableName();
    bool isOn();
    bool manual = false;
public slots:
    void startProcess(bool withLogs);
    void stopProcess();
    AppInfo* clone();
private slots:
    void stateOn();
    void stateOff(int code);
signals:
    void stateChanged();
};

#endif // APPINFO_H
