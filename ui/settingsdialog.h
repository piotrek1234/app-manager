#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include "consts.h"
#include "settings.h"
#include "ui_settingsdialog.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
signals:
    void settingsChanged(Settings settings);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingsDialog *ui;
    void loadSettings();
    void saveAndEmitSettings();
};

#endif // SETTINGSDIALOG_H
