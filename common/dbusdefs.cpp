
#include "dbusdefs.h"
#include <QDBusMetaType>


void init_dbus_metatype(){
    qDBusRegisterMetaType< DeviceStruct >();
    qDBusRegisterMetaType< DeviceList >();
}

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceStruct &d)
{

    argument.beginStructure();
    argument << d.index << d.vendor << d.device << d.devtype << d.domain << d.bus << d.device << d.func << d.status;
    argument.endStructure();


    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceStruct &d)
{
    argument.beginStructure();
    argument >> d.index >> d.vendor >> d.device >> d.devtype >> d.domain >> d.bus >> d.device >> d.func >> d.status;
    argument.endStructure();
    return argument;
}