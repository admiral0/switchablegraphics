/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include <KCModule>
#include "ui_switchablegraphicsconfig.h"
#include "SwitchableGraphicsIface.h"

class SwitchableGraphics : public KCModule, private Ui_switchablegraphics
{
  Q_OBJECT
    
public:
    SwitchableGraphics(QWidget* parent = 0, const QVariantList& args = QVariantList());
    virtual ~SwitchableGraphics();
    
public slots:  
    void load();
    void save();
private:
  void connectModifications();
  KConfig *config;
  OrgAdmiral0SwitchablegraphicsInterface *iface;
};

#endif // SWITCHABLEGRAPHICS_H
