#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QProcess>
#include "appinfo.h"
#include "settings.h"

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
    void on_pbStartApp_clicked();
    void on_pbStopApp_clicked();
    void on_pbAddApp_clicked();
    void refreshList();
    void on_pbRemoveApp_clicked();
    void on_pbSaveInfo_clicked();
    void on_lwApps_currentRowChanged(int currentRow);
    QIcon statusToIcon(AppState state);
    void loadSettings();
    void saveSettings();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_pbOpenLogDir_clicked();
    void on_pbOpenWorkDir_clicked();
    void on_pbBrowseExe_clicked();
    void on_pbBrowseFolder_clicked();
    void on_pbStartTerminal_clicked();

    void on_pbSettings_clicked();
    void updateSettings(Settings setitings);

    void on_pbDuplicateApp_clicked();

private:
    Ui::MainWindow *ui;
    QVector<AppInfo*> infos;
    int currentApp = 0;
    QString settingsPath;
    Settings internalSettings;
};
#endif // MAINWINDOW_H
