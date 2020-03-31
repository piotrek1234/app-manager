#ifndef APPOUTPUTHANDLER_H
#define APPOUTPUTHANDLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTreeWidgetItem>
#include <appinfo.h>

class AppOutputHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppOutputHandler(QObject *parent = nullptr);
    AppOutputHandler* setAppInfo(AppInfo *info);
    AppOutputHandler* setWidgetItem(QTreeWidgetItem *item);
    AppOutputHandler* setTrayIcon(QSystemTrayIcon *tray);
    // todo: set accepter (eg. ItemColumnSetter, TrayNotifier)
    void handleOutput(QString output);
private:
    AppInfo *info;
    QTreeWidgetItem *widgetItem;
    QSystemTrayIcon *tray;
};

#endif // APPOUTPUTHANDLER_H
