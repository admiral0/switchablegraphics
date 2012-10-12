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


#ifndef SWITCHABLEGRAPHICS_H
#define SWITCHABLEGRAPHICS_H

#include <KDEDModule>
#include <KConfig>
#include <Solid/Battery>
#include <KStatusNotifierItem>
#include "backendmanager.h"
#include "dbusdefs.h"

class QAction;
class KMenu;
class QAction;
class SwitchableGraphics : public KDEDModule
{
  Q_OBJECT
public:
  SwitchableGraphics(QObject* parent, const QList<QVariant>&);
  virtual ~SwitchableGraphics();
public slots:
  void Integrated();
  void Discrete();
  QList<SwitcherBackend::Device> GetStatus();
  QString Backend();
signals:
  void StatusChanged(QList<SwitcherBackend::Device> dev);
private slots:
  void updateUi(QList<SwitcherBackend::Device> devs);
  void switchDone(int result);
  void batteryStatusChanged(bool plugged,QString udi);
  void batteryChargeChanged(int percent,QString udi);
  void cancelDelayedSwitch();
  void switchNow();
private:
  bool igd;
  bool notifications;
  bool batenabled;
  bool switchDelayed;
  int lowBat;
  BackendManager *manager;
  KConfig *config;
  Solid::Battery *battery;
  KStatusNotifierItem *icon;
  QList<SwitcherBackend::Device> cache;
  KMenu *menu;
  QAction *a_switchNow;
  QAction *a_cancelSwitch;
};

#endif // SWITCHABLEGRAPHICS_H
