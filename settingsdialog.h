#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settings.h"

#include <QDialog>

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
