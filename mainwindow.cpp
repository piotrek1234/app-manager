#include "appinfo.h"
#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "consts.h"

#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingsPath = SETTINGS_FILE;
    loadSettings();
    QString logDir = qApp->applicationDirPath() + LOG_PATH;
    QDir dir(logDir);
    if (!dir.exists())
        dir.mkpath(".");
}

MainWindow::~MainWindow()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue(SETTINGS__WINDOW_HEIGHT, this->geometry().height());
    delete ui;
}

void MainWindow::on_pbStartApp_clicked()
{
    AppInfo *info = infos.at(currentApp);
    info->startProcess(ui->cbOutput->isChecked());
}

void MainWindow::on_pbStopApp_clicked()
{
    infos.at(currentApp)->stopProcess();
}

void MainWindow::on_pbAddApp_clicked()
{
    AppInfo *info = new AppInfo();
    connect(info, SIGNAL(stateChanged()), this, SLOT(refreshList()));
    infos.append(info);
    QIcon icon = statusToIcon(info->state);
    QListWidgetItem *item = new QListWidgetItem(icon, info->printableName());
    ui->lwApps->addItem(item);
    saveSettings();
}

void MainWindow::refreshList()
{
    int index = ui->lwApps->currentRow();
    ui->lwApps->clear();
    for(AppInfo* info : infos) {
        QIcon icon = statusToIcon(info->state);
        QListWidgetItem *item = new QListWidgetItem(icon, info->printableName());
        ui->lwApps->addItem(item);
    }
    ui->lwApps->setCurrentRow(index);
}

void MainWindow::on_pbRemoveApp_clicked()
{
    AppInfo *info = infos.at(currentApp);
    delete info;
    infos.remove(currentApp);
    ui->lwApps->takeItem(currentApp);
    saveSettings();
}

void MainWindow::on_pbSaveInfo_clicked()
{
    AppInfo *info = infos.at(currentApp);
    info->name = ui->leName->text();
    info->path = ui->pathToExe->text();
    info->workingDir = ui->leFolder->text();
    info->saveOutput = ui->cbOutput->isChecked();
    ui->lwApps->item(currentApp)->setText(info->printableName());
    if (ui->pathToExe->text() == "") {
        ui->pbStartApp->setEnabled(false);
        ui->pbStopApp->setEnabled(false);
        ui->pbSaveInfo->setEnabled(false);
    } else {
        ui->pbStartApp->setEnabled(!info->isOn());
        ui->pbStopApp->setEnabled(info->isOn());
        ui->pbSaveInfo->setEnabled(true);
    }
    ui->pbOpenWorkDir->setEnabled(ui->leFolder->text() != "");

    saveSettings();
}

void MainWindow::on_lwApps_currentRowChanged(int currentRow)
{
    currentApp = currentRow;
    if (currentRow < 0) {
        ui->leName->setEnabled(false);
        ui->pathToExe->setEnabled(false);
        ui->pbStartApp->setEnabled(false);
        ui->pbStopApp->setEnabled(false);
        ui->pbSaveInfo->setEnabled(false);
        ui->pbOpenWorkDir->setEnabled(false);
        ui->pbOpenLogDir->setEnabled(false);
        ui->cbOutput->setEnabled(false);
        ui->pbBrowseExe->setEnabled(false);
        ui->pbBrowseFolder->setEnabled(false);
        ui->pbStartTerminal->setEnabled(false);
        ui->pbDuplicateApp->setEnabled(false);
        ui->pbRemoveApp->setEnabled(false);
        ui->pbMoveUp->setEnabled(false);
        ui->pbMoveDown->setEnabled(false);
    } else {
        AppInfo *info = infos.at(currentApp);
        ui->leName->setText(info->name);
        ui->pathToExe->setText(info->path);
        ui->leFolder->setText(info->workingDir);
        ui->cbOutput->setChecked(info->saveOutput);
        ui->leName->setEnabled(true);
        ui->pathToExe->setEnabled(true);
        ui->pbSaveInfo->setEnabled(true);
        ui->pbOpenWorkDir->setEnabled(ui->leFolder->text() != "");
        ui->pbOpenLogDir->setEnabled(true);
        ui->cbOutput->setEnabled(true);
        ui->pbBrowseExe->setEnabled(true);
        ui->pbBrowseFolder->setEnabled(true);
        ui->pbStartTerminal->setEnabled(ui->leFolder->text() != "" && internalSettings.terminal != "");
        ui->pbDuplicateApp->setEnabled(true);
        ui->pbRemoveApp->setEnabled(true);
        ui->pbMoveUp->setEnabled(currentApp > 0);
        ui->pbMoveDown->setEnabled(currentApp < infos.length() - 1);

        if (ui->pathToExe->text() == "") {
            ui->pbStartApp->setEnabled(false);
            ui->pbStopApp->setEnabled(false);
        } else {
            ui->pbStartApp->setEnabled(!info->isOn());
            ui->pbStopApp->setEnabled(info->isOn());
        }
    }
}

QIcon MainWindow::statusToIcon(AppState state)
{
    switch(state) {
        case AppState::ON:
            return QIcon(":/status/on");
            break;
        case AppState::SELF_OFF:
            return QIcon(":/status/off_warn");
            break;
        default:
            return QIcon(":/status/off");
    }
}

void MainWindow::loadSettings()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    int count = settings.value(SETTINGS__APPS_COUNT, 0).toInt();
    int height = settings.value(SETTINGS__WINDOW_HEIGHT, 320).toInt();
    QRect size = this->geometry();
    size.setHeight(height);
    this->setGeometry(size);
    internalSettings.terminal = settings.value(SETTINGS__TERMINAL, "").toString();
    for(int i = 0; i < count; ++i) {
        QString num = QString::number(i);
        AppInfo *info = new AppInfo();
        info->name = settings.value(SETTINGS__NAME_PREFIX + num, "app").toString();
        info->path = settings.value(SETTINGS__PATH_PREFIX + num, "").toString();
        info->workingDir = settings.value(SETTINGS__WORK_DIR_PREFIX + num, "").toString();
        info->saveOutput = settings.value(SETTINGS__SAVE_OUTPUT_PREFIX + num, false).toBool();
        connect(info, SIGNAL(stateChanged()), this, SLOT(refreshList()));
        infos.append(info);
        QIcon icon = statusToIcon(info->state);
        QListWidgetItem *item = new QListWidgetItem(icon, info->printableName());
        ui->lwApps->addItem(item);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    int count = infos.length();
    settings.setValue(SETTINGS__APPS_COUNT, count);
    for(int i = 0; i < count; ++i) {
        QString num = QString::number(i);
        AppInfo *info = infos.at(i);
        settings.setValue(SETTINGS__NAME_PREFIX + num, info->name);
        settings.setValue(SETTINGS__PATH_PREFIX + num, info->path);
        settings.setValue(SETTINGS__WORK_DIR_PREFIX + num, info->workingDir);
        settings.setValue(SETTINGS__SAVE_OUTPUT_PREFIX + num, info->saveOutput);
    }
}

void MainWindow::on_pbMoveUp_clicked()
{
    if (currentApp >= 0) {
        infos.move(currentApp, currentApp - 1);
        currentApp--;
        ui->lwApps->setCurrentRow(currentApp);
        refreshList();
        saveSettings();
    }
}

void MainWindow::on_pbMoveDown_clicked()
{
    if (currentApp >= 0 && currentApp < infos.length() - 1) {
        infos.move(currentApp, currentApp + 1);
        currentApp++;
        ui->lwApps->setCurrentRow(currentApp);
        refreshList();
        saveSettings();
    }
}

void MainWindow::on_pbOpenLogDir_clicked()
{
    QString logDir = qApp->applicationDirPath() + LOG_PATH + "/" + infos.at(currentApp)->name;
    qDebug() << logDir;
    QDir dir(logDir);
    if (!dir.exists())
        dir.mkpath(".");
    QDesktopServices::openUrl(QUrl::fromLocalFile(logDir));
}

void MainWindow::on_pbOpenWorkDir_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->leFolder->text()));
}

void MainWindow::on_pbBrowseExe_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Find file"));
    if (!file.isEmpty()) {
        ui->pathToExe->setText(file);
    }
}

void MainWindow::on_pbBrowseFolder_clicked()
{
    QString dir = ui->leFolder->text() == "" ? QDir::currentPath() : ui->leFolder->text();
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), dir);

    if (!directory.isEmpty()) {
        ui->leFolder->setText(directory);
    }
}

void MainWindow::on_pbStartTerminal_clicked()
{
    AppInfo *info = infos.at(currentApp);
    QProcess::startDetached(internalSettings.terminal, {}, info->workingDir);
}

void MainWindow::on_pbSettings_clicked()
{
    SettingsDialog *dialog = new SettingsDialog();
    connect(dialog, SIGNAL(settingsChanged(Settings)), this, SLOT(updateSettings(Settings)));
    dialog->exec();
}

void MainWindow::updateSettings(Settings settings)
{
    internalSettings.terminal = settings.terminal;
    qDebug() << internalSettings.terminal;
    ui->pbStartTerminal->setEnabled(internalSettings.terminal != "" &&
            currentApp > -1 && infos.at(currentApp)->workingDir != "");
}

void MainWindow::on_pbDuplicateApp_clicked()
{
    AppInfo *newInfo = infos.at(currentApp)->clone();
    connect(newInfo, SIGNAL(stateChanged()), this, SLOT(refreshList()));
    infos.append(newInfo);
    QIcon icon = statusToIcon(newInfo->state);
    QListWidgetItem *item = new QListWidgetItem(icon, newInfo->printableName());
    ui->lwApps->addItem(item);
    saveSettings();
}
