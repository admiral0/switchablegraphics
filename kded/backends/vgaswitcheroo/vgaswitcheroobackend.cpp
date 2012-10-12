/*
    Copyright (C) 2012  Radu Andries <admiral0@tuxfamily.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "vgaswitcheroobackend.h"
#include <QFile>
#include <QStringList>
#include <kauth.h>
#include <kdebug.h>
extern "C" {
#include <pci/pci.h>
}

using namespace KAuth;

int VgaSwitcherooBackend::b_available(0);
QString VgaSwitcherooBackend::debugfs("");

VgaSwitcherooBackend::VgaSwitcherooBackend()
{
    pacc=pci_alloc();
    pci_init(pacc);
    if(available()) {
        sendString("OFF");
        populateDevices();
        watcher.addPath(debugfs+"/vgaswitcheroo/switch");
        connect(&watcher,SIGNAL(fileChanged(QString)),SLOT(changeOccured()));
    }
}

VgaSwitcherooBackend::~VgaSwitcherooBackend()
{
    pci_cleanup(pacc);
}


int VgaSwitcherooBackend::discrete()
{
    foreach(Device d,devices) {
        if(d.devtype==DEVICE_DISCRETE && d.status==USED)
            return SWITCH_SUCCESS;
    }
    QString sw=sendString("DDIS");
    if(!sw.isEmpty()) {
        kDebug()<< "Error reported is: \"" <<sw<<"\"";
        return SWITCH_FAIL;
    }
    return SWITCH_DELAYED;
}
const QList< SwitcherBackend::Device > VgaSwitcherooBackend::getDevices()
{
    return devices;
}

int VgaSwitcherooBackend::integrated()
{
    bool phys=false;
    bool used=false;
    foreach(Device d,devices) {
        if(d.devtype==DEVICE_INTEGRATED && d.status==USED){
	  used=true;
	}
        if(d.devtype==DEVICE_DISCRETE && d.status==ON){
	  phys=true;
	}
    }
    if(phys){
      sendString("OFF");
      if(used){
	return SWITCH_SUCCESS;
      }
    }
    QString sw=sendString("DIGD");
    if(!sw.isEmpty()) {
        kDebug()<< "Error reported is: \"" <<sw<<"\"";
        return SWITCH_FAIL;
    }
    return SWITCH_DELAYED;
}
bool VgaSwitcherooBackend::available()
{
    if(b_available>0)
        return (b_available==2) ? true : false;
    Action debugfsAction("org.admiral0.switchablegraphics.mountdebugfs");
    debugfsAction.setHelperID("org.admiral0.switchablegraphics");
    ActionReply reply=debugfsAction.execute();
    if(reply.failed()) {
        b_available=1;
        return false;
    }
    debugfs=reply.data()["mountpoint"].toString();
    return true;
}

QString VgaSwitcherooBackend::sendString(QString string)
{
    Action switchAction("org.admiral0.switchablegraphics.vgaswitcheroo");
    switchAction.setHelperID("org.admiral0.switchablegraphics");
    QVariantMap args;
    args["filePath"]=debugfs+"/vgaswitcheroo/switch";
    args["action"]=string;
    switchAction.setArguments(args);
    ActionReply reply=switchAction.execute();
    if(reply.failed()) {
        return reply.errorDescription();
    }
    return QString();
}

void VgaSwitcherooBackend::populateDevices()
{
    devices.clear();
    QFile switcher(debugfs+"/vgaswitcheroo/switch");
    switcher.open(QIODevice::ReadOnly);
    QString raw=switcher.readAll();
    switcher.close();
    QStringList rawCard=raw.split("\n",QString::SkipEmptyParts);
    int index=0;
    char namebuf[1024];
    struct pci_dev *phydev;
    pci_scan_bus(pacc);
    foreach(QString info, rawCard) {
        Device dev;
        info.remove("\n");
        QStringList parts=info.split(":");
        dev.index=index++;
        QString card=parts.at(1);
        if(card=="IGD") {
            dev.devtype=DEVICE_INTEGRATED;
        } else if(card=="DIS") {
            dev.devtype=DEVICE_DISCRETE;
        } else {
            dev.devtype=DEVICE_OTHER;
        }
        dev.domain=parts.at(4).toInt(0,16);
        dev.bus=parts.at(5).toInt(0,16);
        QStringList devfunc=parts.at(6).split(".");
        dev.dev=devfunc.at(0).toInt(0,16);
        dev.func=devfunc.at(1).toInt(0,16);
        if(parts.at(2)=="+") {
            dev.status=USED;
        } else if (parts.at(3)=="Off") {
            dev.status=OFF;
        } else {
            dev.status=ON;
        }
        for (phydev=pacc->devices; phydev; phydev=phydev->next)   /* Iterate over all devices */
        {
	  if(phydev->domain==dev.domain && phydev->bus==dev.bus && phydev->dev==dev.dev && phydev->func==dev.func){
            pci_fill_info(phydev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);     /* Fill in header info we need */
	    dev.vendor= QString(pci_lookup_name(pacc, namebuf, sizeof(namebuf), PCI_LOOKUP_VENDOR, phydev->vendor_id, phydev->device_id));
            dev.device= QString(pci_lookup_name(pacc, namebuf, sizeof(namebuf), PCI_LOOKUP_DEVICE, phydev->vendor_id, phydev->device_id));
	  }
	}
        devices.append(dev);
    }
}

void VgaSwitcherooBackend::changeOccured()
{
    populateDevices();
    emit changed(devices);
}



#include "vgaswitcheroobackend.moc"
