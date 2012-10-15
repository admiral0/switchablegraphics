/*
    This file is part of switchablegraphics kded module
    Copyright (C) 2012  Radu Andries <admiral0@tuxfamily.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KConfigGroup>
#include "switchablegraphics.h"
#include "backendmanager.h"
#include <Solid/Device>
#include <Solid/DeviceInterface>
#include <KMenu>
#include <KLocalizedString>
#include <kworkspace/kdisplaymanager.h>
#include <KDEDModule>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <QDBusArgument>

#include "switchablegraphicsadaptor.h"
#include "../common/dbusdefs.cpp"


K_PLUGIN_FACTORY(SwitchableGraphicsFactory,registerPlugin<SwitchableGraphics>();)
K_EXPORT_PLUGIN(SwitchableGraphicsFactory("switchablegraphics"))

SwitchableGraphics::SwitchableGraphics(QObject* parent, const QList< QVariant >&): KDEDModule(parent)
{
    init_dbus_metatype();

    icon=0;
    igd=false;
    manager=BackendManager::instance(); //FIXME
    config=new KConfig("switchablegraphicsrc");
    switchDelayed=false;
    UpdateSettings();
    QList< Solid::Device > list = Solid::Device::listFromType(Solid::DeviceInterface::Battery, QString());
    if(!list.empty()) {
        battery=list.first().as<Solid::Battery>();
    }
    connect(manager,SIGNAL(deviceListUpdated(QList<Device>)),SLOT(updateUi(QList<Device>)));
    connect(manager,SIGNAL(switchPerformed(int)),SLOT(switchDone(int)));
    manager->updateDevices();

    //Start dbus service
    new SwitchablegraphicsAdaptor(this);
    QDBusConnection::sessionBus().registerService("org.admiral0.switchablegraphics");
    QDBusConnection::sessionBus().registerObject("/org/admiral0/switchablegraphics",this);
}

SwitchableGraphics::~SwitchableGraphics()
{
    manager->deleteLater();
    delete config;
    if (icon)
      icon->deleteLater();
    battery->deleteLater();
}
void SwitchableGraphics::Discrete()
{
    igd=false;
    manager->discrete();
    if(icon)
        icon->setToolTip("cpu",i18n("Performing Switch"),i18n("Switching GPU to discrete card"));
}
QList< Device > SwitchableGraphics::GetStatus()
{
    return cache;
}
void SwitchableGraphics::Integrated()
{
    igd=true;
    manager->integrated();
    if(icon)
        icon->setToolTip("cpu",i18n("Performing Switch"),i18n("Switching GPU to integrated card"));
}

void SwitchableGraphics::batteryChargeChanged(int percent, QString udi)
{
    Q_UNUSED(udi);
    if(percent <= lowBat && !battery->isPlugged()) {
        Integrated();
    }
}
void SwitchableGraphics::batteryStatusChanged(bool plugged, QString udi)
{
    Q_UNUSED(udi);
    if(plugged) {
        Discrete();
    } else {
        Integrated();
    }
}
void SwitchableGraphics::cancelDelayedSwitch()
{
    switchDelayed=false;
    a_cancelSwitch->setEnabled(false);
    a_switchNow->setEnabled(true);
    manager->cancelDelayedSwitch();
}
void SwitchableGraphics::switchNow()
{
    manager->doSwitch();
}
void SwitchableGraphics::updateUi(QList< Device > devs)
{

    cache.clear();
    foreach(Device d,devs) {
        cache.append(d);
    }
    if(icon) {
        QString iconn,title,status;
        if(switchDelayed) {
            iconn="dialog-warning";
            title=i18n("Logout is requested");
        } else {
            iconn="cpu";
            title=i18n("Switchable Graphics" );
        }
        status=i18n("<i>Backend:</i>");
        status.append(" <b>");
        status.append(manager->getBackendName());
        status.append("</b><br/>");
        int s=devs.size();
        int i=0;
        foreach(Device d,devs) {
            status.append("<i>");
            status.append(d.vendor);
            status.append(" ");
            status.append(d.device);
            status.append(":</i>&nbsp;<b>  ");
            status.append((d.status==SwitcherBackend::USED) ? i18n("Used") :
                          (d.status==SwitcherBackend::ON) ? i18n("Powered") : i18n("Off"));
            (i!=s-1) ? status.append("</b><br/>") : status.append("</b>");
            i++;
        }
        icon->setToolTip(iconn,title,status);
    }

}
void SwitchableGraphics::switchDone(int result)
{
    if(result==SwitcherBackend::SWITCH_DELAYED) {
        switchDelayed=true;
        a_cancelSwitch->setEnabled(true);
        a_switchNow->setDisabled(true);
    }
    if(notifications && icon) {
        if(switchDelayed)
            icon->showMessage(i18n("Switchable Graphics"),i18n("Logout is required to finish switching card"),"dialog-warning");
        else if(result==SwitcherBackend::SWITCH_SUCCESS)
            icon->showMessage(i18n("Switchable Graphics"),i18n("Switched card with success"),"dialog-information");
        else
            icon->showMessage(i18n("Switchable Graphics"),i18n("Cannot switch card. Look at the logs for the error."),"dialog-error");
    }
    if(icon && batenabled) {
        icon->setStatus(KStatusNotifierItem::Passive);
    }
}

QString SwitchableGraphics::Backend()
{
    return manager->getBackendName();
}

void SwitchableGraphics::UpdateSettings()
{
//TODO
    batenabled=config->group("General").readEntry<bool>("powerManagement",true);
    notifications=config->group("General").readEntry<bool>("displayNotifications",true);
    lowBat=config->group("General").readEntry<int>("lowBatteryPercentage",20);
    bool lowBatEnable=config->group("General").readEntry<bool>("lowBatteryManagement",true);

    if(config->group("General").readEntry<bool>("displayNotifier",true)) {
	icon=new KStatusNotifierItem();
        icon->setTitle(i18n("Switchable Graphics"));
        icon->setIconByName("cpu");
        icon->setCategory(KStatusNotifierItem::Hardware);
        icon->setStandardActionsEnabled(false);

        //We want the icon to be visible in the tray if autoswitching is off
        icon->setStatus((batenabled) ? KStatusNotifierItem::Passive : KStatusNotifierItem::Active);
        menu=new KMenu(i18n("Switchable Graphics"),0);
        a_switchNow=menu->addAction(QIcon::fromTheme("cpu"),i18n("Switch Card"));
        a_cancelSwitch=menu->addAction(QIcon::fromTheme("dialog-cancel"),i18n("Cancel Switch"));
        icon->setContextMenu(menu);
        connect(a_cancelSwitch,SIGNAL(triggered(bool)),SLOT(cancelDelayedSwitch()));
        connect(a_switchNow,SIGNAL(triggered(bool)),SLOT(switchNow()));
        a_cancelSwitch->setEnabled(false);
    }else{
	icon->disconnect();
	icon->deleteLater();
    }
    if(batenabled){
      connect(battery,SIGNAL(plugStateChanged(bool,QString)),SLOT(batteryStatusChanged(bool,QString)));
        if(lowBatEnable) {
            connect(battery,SIGNAL(chargePercentChanged(int,QString)),SLOT(batteryChargeChanged(int,QString)));
        }
    }else{
      battery->disconnect();
    }
}


#include "switchablegraphics.moc"
