#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    saveAndEmitSettings();
}

void SettingsDialog::loadSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    ui->leTerminal->setText(settings.value(SETTINGS__TERMINAL, "").toString());
    ui->cbAppsCount->setChecked(settings.value(SETTINGS__TRAY_APPS_COUNT, DEFAULT_SHOW_TRAY_APPS_COUNT).toBool());
    ui->cbNotifySelfOff->setChecked(settings.value(SETTINGS__NOTIFY_SELF_OFF, DEFAULT_NOTIFY_SELF_OFF).toBool());
}

void SettingsDialog::saveAndEmitSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);

    QString terminal = ui->leTerminal->text();
    bool showAppsCount = ui->cbAppsCount->isChecked();
    bool notifySelfOff = ui->cbNotifySelfOff->isChecked();

    settings.setValue(SETTINGS__TERMINAL, terminal);
    settings.setValue(SETTINGS__TRAY_APPS_COUNT, showAppsCount);
    settings.setValue(SETTINGS__NOTIFY_SELF_OFF, notifySelfOff);

    Settings internalSettings;
    internalSettings.terminal = terminal;
    internalSettings.showAppsCount = showAppsCount;
    internalSettings.notifySelfOff = notifySelfOff;
    emit settingsChanged(internalSettings);
}
