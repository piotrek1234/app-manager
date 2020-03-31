#include "appoutputhandler.h"

AppOutputHandler::AppOutputHandler(QObject *parent) : QObject(parent)
{

}

AppOutputHandler *AppOutputHandler::setAppInfo(AppInfo *info)
{
    this->info = info;
    return this;
}

AppOutputHandler *AppOutputHandler::setWidgetItem(QTreeWidgetItem *item)
{
    this->widgetItem = item;
    return this;
}

AppOutputHandler *AppOutputHandler::setTrayIcon(QSystemTrayIcon *tray)
{
    this->tray = tray;
    return this;
}

void AppOutputHandler::handleOutput(QString output)
{
    if (output.contains(QRegExp("Started \\w in"))) {
        widgetItem->setText(2, "Started");
    } else if (output.contains("ERROR")) {
        QString exception = "jakiś exception";
        tray->showMessage(info->name, exception, QSystemTrayIcon::Warning);
    }
}
