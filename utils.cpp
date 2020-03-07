#include "utils.h"

QIcon Utils::appStateToIcon(AppState state)
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

QDir Utils::prepareDirectory(QString path)
{
    QDir directory(path);
    if (!directory.exists())
        directory.mkpath(".");
    return directory;
}
