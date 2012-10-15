/*
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


#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include "backends/switcherbackend.h"

class BackendManager : public QObject
{
    Q_OBJECT
public:
    static BackendManager* instance();
    QString getBackendName();
public slots:
    void discrete();
    void integrated();
    void updateDevices();
    void cancelDelayedSwitch();
    void doSwitch();
private slots:
  void updateDevicesInternal();
signals:
    void needsUpdateDevicesInternal();
    void switchPerformed(int result);
    void deviceListUpdated(QList<Device> devices);
private:
    BackendManager();
    static BackendManager* _self;
    SwitcherBackend *backend;
    QString backendName;
};

#endif // BACKENDMANAGER_H
