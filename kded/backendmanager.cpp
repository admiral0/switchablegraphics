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


#include "backendmanager.h"
#include <QMutex>

//Include backends
#include "backends/vgaswitcheroo/vgaswitcheroobackend.h"
#include "backends/null/nullbackend.h"

BackendManager* BackendManager::_self(0);

BackendManager::BackendManager()
{
  //Order matters
    if(VgaSwitcherooBackend::available()){
      backendName="VgaSwitcheroo";
      backend=new VgaSwitcherooBackend();
    }else{
      backendName="NullBackend";
      backend=new NullBackend();
    }
    connect(backend,SIGNAL(changed(QList<Device>)),SLOT(updateDevices()));
    connect(this,SIGNAL(needsUpdateDevicesInternal()),SLOT(updateDevicesInternal()));
}
void BackendManager::discrete()
{
  int res;
  res=backend->discrete();
  emit switchPerformed(res);
}
BackendManager* BackendManager::instance()
{
  if(_self==0){
    _self=new BackendManager();
  }
  return _self;
}

void BackendManager::integrated()
{
  int res;
  res=backend->integrated();
  emit switchPerformed(res);
}
void BackendManager::updateDevices()
{
  emit needsUpdateDevicesInternal();
}


void BackendManager::cancelDelayedSwitch()
{
  foreach(Device d, backend->getDevices()){
    if(d.devtype==SwitcherBackend::DEVICE_INTEGRATED && d.status==SwitcherBackend::USED){
      integrated();
      return;
    }
    if(d.devtype==SwitcherBackend::DEVICE_DISCRETE && d.status==SwitcherBackend::USED){
      discrete();
      return;
    }  
  }
}
void BackendManager::doSwitch()
{
  foreach(Device d, backend->getDevices()){
    if(d.devtype==SwitcherBackend::DEVICE_INTEGRATED && d.status==SwitcherBackend::USED){
      discrete();
      return;
    }
    if(d.devtype==SwitcherBackend::DEVICE_DISCRETE && d.status==SwitcherBackend::USED){
      integrated();
      return;
    }  
  }
}
void BackendManager::updateDevicesInternal()
{
  QList<Device> lists;
  lists= backend->getDevices();
  emit deviceListUpdated(lists);
}
QString BackendManager::getBackendName()
{
  return backendName;
}


#include "backendmanager.moc"