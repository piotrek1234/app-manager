#ifndef CONSTS_H
#define CONSTS_H

#include <QString>

const QString VERSION = "v0.1.1";

const QString DEFAULT_APP_NAME = "app";
const bool DEFAULT_SHOW_TRAY_APPS_COUNT = true;
const bool DEFAULT_NOTIFY_SELF_OFF = true;

const QString DATE_TIME_FORMAT = "yyyy-MM-dd_hh-mm-ss";
const QString LOG_PATH = "/log";

const QString SETTINGS_FILE = "settings.ini";
const QString SETTINGS__APPS_COUNT = "count";
const QString SETTINGS__WINDOW_HEIGHT = "windowHeight";
const QString SETTINGS__TERMINAL = "terminal";
const QString SETTINGS__TRAY_APPS_COUNT = "showTraysAppsCount";
const QString SETTINGS__NOTIFY_SELF_OFF = "notifySelfOff";
const QString SETTINGS__NAME_PREFIX = "name-";
const QString SETTINGS__PATH_PREFIX = "path-";
const QString SETTINGS__WORK_DIR_PREFIX = "workingDir-";
const QString SETTINGS__SAVE_OUTPUT_PREFIX = "saveOutput-";

#endif // CONSTS_H
