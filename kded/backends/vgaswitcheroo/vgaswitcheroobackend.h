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


#ifndef VGASWITCHEROOBACKEND_H
#define VGASWITCHEROOBACKEND_H
#include "../switcherbackend.h"
extern "C" {
#include <pci/pci.h>
}
#include <QFileSystemWatcher> 

class VgaSwitcherooBackend : public SwitcherBackend
{
  Q_OBJECT
public:
    VgaSwitcherooBackend();
    virtual ~VgaSwitcherooBackend();
    const  QList < Device > getDevices();
    int discrete();
    int integrated();
    inline int getStatus(int deviceIndex) {return devices.at(deviceIndex).status; }
    static bool available();
private slots:
    void changeOccured();
private:
    QString sendString(QString string);
    void populateDevices();
    QList<Device> devices;
    static QString debugfs;
    static int b_available;
    struct pci_access *pacc;
    QFileSystemWatcher watcher;
};

#endif // VGASWITCHEROOBACKEND_H
