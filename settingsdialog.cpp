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
}

void SettingsDialog::saveAndEmitSettings()
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    QString terminal = ui->leTerminal->text();
    settings.setValue(SETTINGS__TERMINAL, terminal);
    Settings internalSettings;
    internalSettings.terminal = terminal;
    emit settingsChanged(internalSettings);
}
