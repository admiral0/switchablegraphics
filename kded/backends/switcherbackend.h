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


#ifndef SWITCHERBACKEND_H
#define SWITCHERBACKEND_H

#include <QObject>
#include <QString>
#include "../../common/dbusdefs.h"


class SwitcherBackend : public QObject
{
  Q_OBJECT
public:
  enum {
    OFF=0,
    ON=1,
    USED=2
  };
  
  enum {
    SWITCH_SUCCESS=0,
    SWITCH_FAIL=1,
    SWITCH_DELAYED=2 //Needs logout
  };
  enum {
    DEVICE_INTEGRATED=0,
    DEVICE_DISCRETE=1,
    DEVICE_OTHER=2
  };
  virtual ~SwitcherBackend() {};
  
  /**
   * Gets all devices
   */
  virtual const QList<Device> getDevices() = 0;
  /**
   * Switch to low power device
   */
  virtual int integrated() = 0;
  /**
   * Switch to performance device
   */
  virtual int discrete() = 0;
signals:
  void changed(QList<Device> devices);
  
};
#endif // SWITCHERBACKEND_H
