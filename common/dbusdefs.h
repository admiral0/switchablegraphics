#ifndef DBUSDEFS_H
#define DBUSDEFS_H

#include <QDBusArgument>

struct Device{
    int index;
    QString vendor;
    QString device;
    int devtype;
    int domain;
    int bus;
    int dev;
    int func;
    int status;
  };
typedef Device DeviceStruct;
typedef QList< DeviceStruct > DeviceList;

Q_DECLARE_METATYPE(DeviceStruct)
Q_DECLARE_METATYPE(DeviceList)


#endif 