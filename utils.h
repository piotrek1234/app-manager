#ifndef UTILS_H
#define UTILS_H

#include <QDir>
#include <QIcon>
#include "appstate.h"

class Utils
{
public:
    static QIcon appStateToIcon(AppState state);
    static QDir prepareDirectory(QString path);
};

#endif // UTILS_H
