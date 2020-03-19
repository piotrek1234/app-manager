#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDesktopServices>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QListWidget>
#include <QMainWindow>
#include <QProcess>
#include <QSettings>
#include <QUrl>
#include "appinfo.h"
#include "consts.h"
#include "settings.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    // app list management
    void on_pbAddApp_clicked();
    void on_pbDuplicateApp_clicked();
    void on_pbMoveDown_clicked();
    void on_pbMoveUp_clicked();
    void on_pbRemoveApp_clicked();
    void on_lwApps_currentRowChanged(int currentRow);
    void refreshList();
    AppInfo* getCurrentAppInfo();

    // app control
    void on_pbOpenLogDir_clicked();
    void on_pbOpenWorkDir_clicked();
    void on_pbStartTerminal_clicked();
    void on_pbStartApp_clicked();
    void on_pbStopApp_clicked();
    void on_pbStopAll_clicked();

    // app info settings
    void on_pbBrowseExe_clicked();
    void on_pbBrowseWorkDir_clicked();
    void on_pbSaveInfo_clicked();
    void loadSettings();
    void saveSettings();
    void setInfoFromUi(AppInfo *info);

    // system settings
    void on_pbSettings_clicked();
    void updateSettings(Settings setitings);

private:
    Ui::MainWindow *ui;
    QVector<AppInfo*> infos;
    int currentAppIndex = 0;
    QString settingsPath;
    Settings internalSettings;
};
#endif // MAINWINDOW_H
