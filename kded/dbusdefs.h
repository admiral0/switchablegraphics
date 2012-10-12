#ifndef DBUSDEFS_H
#define DBUSDEFS_H

#include "backends/switcherbackend.h"
#include <QDBusArgument>

typedef QList< SwitcherBackend::Device > DeviceList;
typedef SwitcherBackend::Device DeviceStruct;
Q_DECLARE_METATYPE(DeviceStruct)
Q_DECLARE_METATYPE(DeviceList)


#endif 