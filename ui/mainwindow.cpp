#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->twApps->setColumnWidth(0, 20);
    ui->twApps->setColumnWidth(1, 140);
    loadSettings();
    prepareLogDir();
    configureTrayIcon();
}

MainWindow::~MainWindow()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setValue(SETTINGS__WINDOW_HEIGHT, this->geometry().height());
    delete tray->contextMenu();
    delete tray;
    delete ui;
}

void MainWindow::on_pbStartApp_clicked()
{
    getCurrentAppInfo()->startProcess();
}

void MainWindow::on_pbStopApp_clicked()
{
    getCurrentAppInfo()->stopProcess();
}

void MainWindow::on_pbAddApp_clicked()
{
    AppInfo *info = new AppInfo();
    connect(info, SIGNAL(stateChanged(AppInfo*)), this, SLOT(handleAppStateChanged(AppInfo*)));
    infos.append(info);
    QIcon icon = Utils::appStateToIcon(info->state);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->twApps);
    item->setIcon(0, icon);
    item->setText(1, info->printableName());
    saveSettings();
}

void MainWindow::refreshList()
{
    int index = ui->twApps->indexOfTopLevelItem(ui->twApps->currentItem());
    ui->twApps->clear();
    for (AppInfo *info : infos) {
        QIcon icon = Utils::appStateToIcon(info->state);
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->twApps);
        item->setIcon(0, icon);
        item->setText(1, info->printableName());
    }
    ui->twApps->setCurrentItem(ui->twApps->topLevelItem(index));
    updateTrayIcon();
}

AppInfo *MainWindow::getCurrentAppInfo()
{
    return infos.at(currentAppIndex);
}

void MainWindow::on_pbRemoveApp_clicked()
{
    AppInfo *info = getCurrentAppInfo();
    delete info;
    infos.remove(currentAppIndex);
    ui->twApps->takeTopLevelItem(currentAppIndex);
    saveSettings();
}

void MainWindow::on_pbSaveInfo_clicked()
{
    AppInfo *info = getCurrentAppInfo();
    setInfoFromUi(info);
    ui->twApps->topLevelItem(currentAppIndex)->setText(1, info->printableName());
    if (ui->pathToExe->text() == "") {
        ui->pbStartApp->setEnabled(false);
        ui->pbStopApp->setEnabled(false);
        ui->pbSaveInfo->setEnabled(false);
    } else {
        ui->pbStartApp->setEnabled(!info->isOn());
        ui->pbStopApp->setEnabled(info->isOn());
        ui->pbSaveInfo->setEnabled(true);
    }
    ui->pbOpenWorkDir->setEnabled(ui->leWorkDir->text() != "");
    ui->pbStartTerminal->setEnabled(ui->leWorkDir->text() != "");

    saveSettings();
}

void MainWindow::loadSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    int count = settings.value(SETTINGS__APPS_COUNT, 0).toInt();
    int height = settings.value(SETTINGS__WINDOW_HEIGHT, 320).toInt();
    QRect size = this->geometry();
    size.setHeight(height);
    this->setGeometry(size);
    internalSettings.terminal = settings.value(SETTINGS__TERMINAL, "").toString();
    internalSettings.showAppsCount = settings.value(SETTINGS__TRAY_APPS_COUNT, DEFAULT_SHOW_TRAY_APPS_COUNT).toBool();
    internalSettings.notifySelfOff = settings.value(SETTINGS__NOTIFY_SELF_OFF, DEFAULT_NOTIFY_SELF_OFF).toBool();
    for(int i = 0; i < count; ++i) {
        QString num = QString::number(i);
        AppInfo *info = new AppInfo();
        info->name = settings.value(SETTINGS__NAME_PREFIX + num, "app").toString();
        info->path = settings.value(SETTINGS__PATH_PREFIX + num, "").toString();
        info->workingDir = settings.value(SETTINGS__WORK_DIR_PREFIX + num, "").toString();
        info->saveOutput = settings.value(SETTINGS__SAVE_OUTPUT_PREFIX + num, false).toBool();
        connect(info, SIGNAL(stateChanged(AppInfo*)), this, SLOT(handleAppStateChanged(AppInfo*)));
        infos.append(info);
        QIcon icon = Utils::appStateToIcon(info->state);
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->twApps);
        item->setIcon(0, icon);
        item->setText(1, info->printableName());
    }
}

void MainWindow::saveSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
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
    if (currentAppIndex >= 0) {
        infos.move(currentAppIndex, currentAppIndex - 1);
        currentAppIndex--;
        ui->twApps->setCurrentItem(ui->twApps->topLevelItem(currentAppIndex));
        refreshList();
        saveSettings();
    }
}

void MainWindow::on_pbMoveDown_clicked()
{
    if (currentAppIndex >= 0 && currentAppIndex < infos.length() - 1) {
        infos.move(currentAppIndex, currentAppIndex + 1);
        currentAppIndex++;
        ui->twApps->setCurrentItem(ui->twApps->topLevelItem(currentAppIndex));
        refreshList();
        saveSettings();
    }
}

void MainWindow::on_pbOpenLogDir_clicked()
{
    QString logDir = qApp->applicationDirPath() + LOG_PATH + "/" + getCurrentAppInfo()->name;
    QDir dir(logDir);
    if (!dir.exists())
        dir.mkpath(".");
    QDesktopServices::openUrl(QUrl::fromLocalFile(logDir));
}

void MainWindow::on_pbOpenWorkDir_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->leWorkDir->text()));
}

void MainWindow::on_pbBrowseExe_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Find file"));
    if (!file.isEmpty()) {
        ui->pathToExe->setText(file);
    }
}

void MainWindow::on_pbBrowseWorkDir_clicked()
{
    QString dir = ui->leWorkDir->text() == "" ? QDir::currentPath() : ui->leWorkDir->text();
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), dir);

    if (!directory.isEmpty()) {
        ui->leWorkDir->setText(directory);
    }
}

void MainWindow::on_pbStartTerminal_clicked()
{
    QProcess::startDetached(internalSettings.terminal, {}, getCurrentAppInfo()->workingDir);
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
    internalSettings.showAppsCount = settings.showAppsCount;
    internalSettings.notifySelfOff = settings.notifySelfOff;
    ui->pbStartTerminal->setEnabled(internalSettings.terminal != "" &&
            currentAppIndex > -1 && getCurrentAppInfo()->workingDir != "");
    updateTrayIcon(true);
}

void MainWindow::handleTrayIconClick(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger || reason == QSystemTrayIcon::ActivationReason::DoubleClick) {
        showMainWindow();
    }
}

void MainWindow::configureTrayIcon()
{
    tray = new QSystemTrayIcon(QIcon(":/tray/apps"));
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(handleTrayIconClick(QSystemTrayIcon::ActivationReason)));
    QMenu *trayMenu = new QMenu();
    tray->setToolTip("AppManager");
    trayMenu->addAction(QIcon(":/tray/apps"), "Pokaż", this, SLOT(showMainWindow()));
    trayMenu->addAction(QIcon(":/icon/stop"), "Zatrzymaj wszystko", this, SLOT(on_pbStopAll_clicked()));
    trayMenu->addAction(QIcon(":/icon/logs"), "Folder z logami", this, SLOT(openLogsDir()));
    trayMenu->addAction(QIcon(":/icon/delete"), "Wyłącz", this, SLOT(beforeExit()));
    tray->setContextMenu(trayMenu);
    tray->show();
}

void MainWindow::updateTrayIcon(bool forceUpdate)
{
    if (forceUpdate || internalSettings.showAppsCount) {
        int runningApps = getRunningAppsCount();
        if (internalSettings.showAppsCount && runningApps > 0) {
            QPixmap pixmap(":/tray/apps_bullet");
            QPainter painter(&pixmap);
            QFont font = QFont(TRAY_FONT_NAME, TRAY_FONT_SIZE);
            painter.setFont(font);
            painter.drawText(1, 15, QString::number(runningApps));
            tray->setIcon(QIcon(pixmap));
        } else {
            tray->setIcon(QIcon(":/tray/apps"));
        }
    }
}

void MainWindow::openLogsDir()
{
    QString logDir = qApp->applicationDirPath() + LOG_PATH;
    QDir dir(logDir);
    if (!dir.exists())
        dir.mkpath(".");
    QDesktopServices::openUrl(QUrl::fromLocalFile(logDir));
}

void MainWindow::prepareLogDir()
{
    QString logDir = qApp->applicationDirPath() + LOG_PATH;
    Utils::prepareDirectory(logDir);
}

void MainWindow::showMainWindow()
{
    if (this->isVisible()) {
        this->setWindowState(this->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
        this->activateWindow();
    } else {
        this->setVisible(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->setVisible(false);
}

void MainWindow::beforeExit()
{
    int running = getRunningAppsCount();
    if (running > 0) {
        QMessageBox msgBox;
        msgBox.setText(QString("Część aplikacji jeszcze pracuje (%1). Co robimy?").arg(running));
        QPushButton *closeAll = msgBox.addButton("Wyłącz wszystko i wyjdź", QMessageBox::YesRole);
        QPushButton *justQuit = msgBox.addButton("Zostaw włączone i wyjdź", QMessageBox::NoRole);
        QPushButton *stay = msgBox.addButton("Nic nie wyłączaj", QMessageBox::RejectRole);
        msgBox.exec();
        if (msgBox.clickedButton() == closeAll) {
            on_pbStopAll_clicked();
            aboutToExit = true;
        } else if (msgBox.clickedButton() == justQuit) {
            qApp->quit();
        } else if (msgBox.clickedButton() == stay) {
            // nothing
        }
    } else {
        qApp->quit();
    }
}

void MainWindow::checkExitAfterAllClosed()
{
    if (aboutToExit && getRunningAppsCount() == 0) {
        qApp->quit();
    }
}

void MainWindow::on_pbDuplicateApp_clicked()
{
    AppInfo *newInfo = getCurrentAppInfo()->clone();
    connect(newInfo, SIGNAL(stateChanged(AppInfo*)), this, SLOT(handleAppStateChanged(AppInfo*)));
    infos.append(newInfo);
    QIcon icon = Utils::appStateToIcon(newInfo->state);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->twApps);
    item->setIcon(0, icon);
    item->setText(1, newInfo->printableName());
    saveSettings();
}

void MainWindow::setInfoFromUi(AppInfo *info)
{
    info->name = ui->leName->text();
    info->path = ui->pathToExe->text();
    info->workingDir = ui->leWorkDir->text();
    info->saveOutput = ui->cbOutput->isChecked();
}

void MainWindow::on_pbStopAll_clicked()
{
    for (AppInfo *info : infos) {
        if (info->isOn()) {
            info->stopProcess();
        }
    }
}

int MainWindow::getRunningAppsCount()
{
    int result = 0;
    for (AppInfo *info : infos) {
        if (info->isOn()) {
            result++;
        }
    }
    return result;
}

void MainWindow::handleAppStateChanged(AppInfo *info)
{
    refreshList();
    checkExitAfterAllClosed();
    if (internalSettings.notifySelfOff && info->state == AppState::SELF_OFF) {
        tray->showMessage("AppManager", "Aplikacja " + info->name + " wyłączyła się");
    }
}

void MainWindow::on_twApps_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    currentAppIndex = ui->twApps->indexOfTopLevelItem(current);
    if (currentAppIndex < 0) {
        ui->leName->setEnabled(false);
        ui->pathToExe->setEnabled(false);
        ui->pbStartApp->setEnabled(false);
        ui->pbStopApp->setEnabled(false);
        ui->pbSaveInfo->setEnabled(false);
        ui->pbOpenWorkDir->setEnabled(false);
        ui->pbOpenLogDir->setEnabled(false);
        ui->cbOutput->setEnabled(false);
        ui->pbBrowseExe->setEnabled(false);
        ui->pbBrowseWorkDir->setEnabled(false);
        ui->pbStartTerminal->setEnabled(false);
        ui->pbDuplicateApp->setEnabled(false);
        ui->pbRemoveApp->setEnabled(false);
        ui->pbMoveUp->setEnabled(false);
        ui->pbMoveDown->setEnabled(false);
    } else {
        AppInfo *info = getCurrentAppInfo();
        ui->leName->setText(info->name);
        ui->pathToExe->setText(info->path);
        ui->leWorkDir->setText(info->workingDir);
        ui->cbOutput->setChecked(info->saveOutput);
        ui->leName->setEnabled(true);
        ui->pathToExe->setEnabled(true);
        ui->pbSaveInfo->setEnabled(true);
        ui->pbOpenWorkDir->setEnabled(ui->leWorkDir->text() != "");
        ui->pbOpenLogDir->setEnabled(true);
        ui->cbOutput->setEnabled(true);
        ui->pbBrowseExe->setEnabled(true);
        ui->pbBrowseWorkDir->setEnabled(true);
        ui->pbStartTerminal->setEnabled(ui->leWorkDir->text() != "" && internalSettings.terminal != "");
        ui->pbDuplicateApp->setEnabled(true);
        ui->pbRemoveApp->setEnabled(true);
        ui->pbMoveUp->setEnabled(currentAppIndex > 0);
        ui->pbMoveDown->setEnabled(currentAppIndex < infos.length() - 1);

        if (ui->pathToExe->text() == "") {
            ui->pbStartApp->setEnabled(false);
            ui->pbStopApp->setEnabled(false);
        } else {
            ui->pbStartApp->setEnabled(!info->isOn());
            ui->pbStopApp->setEnabled(info->isOn());
        }
    }
}
