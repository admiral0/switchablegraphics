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


#include "switchablegraphics.h"

#include <KPluginFactory>
#include <KAboutData>
#include <QDBusMetaType>
#include "../common/dbusdefs.cpp"

K_PLUGIN_FACTORY(SwitchableGraphicsFactory, registerPlugin<SwitchableGraphics>(); )
K_EXPORT_PLUGIN(SwitchableGraphicsFactory("kcm_switchablegraphics","kcm_switchablegraphics") )

SwitchableGraphics::SwitchableGraphics(QWidget* parent, const QVariantList& args): KCModule(SwitchableGraphicsFactory::componentData(), parent, args)
{
  init_dbus_metatype();
  KAboutData *about = new KAboutData(
     "SwitchableGraphics", 0, ki18n( "..." ),
     KDE_VERSION_STRING, KLocalizedString(), KAboutData::License_GPL_V3,
     ki18n( "Copyright 2012 Radu Andries" ) );
   about->addAuthor( ki18n("Radu Andries") );
   
   
   iface = new OrgAdmiral0SwitchablegraphicsInterface("org.admiral0.switchablegraphics","/org/admiral0/switchablegraphics",QDBusConnection::sessionBus());
   setAboutData( about );
   
   setButtons(Apply);
   
   config=new KConfig("switchablegraphicsrc");
   setupUi(this);
   
   connectModifications();
}

SwitchableGraphics::~SwitchableGraphics(){
  delete config;
  iface->deleteLater();
}

void SwitchableGraphics::connectModifications()
{
  connect(this->autoSwitch,SIGNAL(toggled(bool)),SLOT(changed()));
  connect(this->lowBatSwitchFlag,SIGNAL(toggled(bool)),SLOT(changed()));
  connect(this->sliderPerc,SIGNAL(valueChanged(int)),SLOT(changed()));
  connect(this->inputPerc,SIGNAL(valueChanged(int)),SLOT(changed()));
  connect(this->statusIconFlag,SIGNAL(toggled(bool)),SLOT(changed()));
  connect(this->notificationsFlag,SIGNAL(toggled(bool)),SLOT(changed()));
}

void SwitchableGraphics::load()
{
    this->autoSwitch->setChecked(config->group("General").readEntry<bool>("powerManagement",true));
    this->lowBatSwitchFlag->setChecked(config->group("General").readEntry<bool>("lowBatteryManagement",true));
    this->sliderPerc->setValue(config->group("General").readEntry<int>("lowBatteryPercentage",20));
    this->inputPerc->setValue(config->group("General").readEntry<int>("lowBatteryPercentage",20));
    this->statusIconFlag->setChecked(config->group("General").readEntry<bool>("displayNotifier",true));
    this->notificationsFlag->setChecked(config->group("General").readEntry<bool>("displayNotifications",true));
    
    QDBusPendingReply<DeviceList> devsDbus=iface->GetStatus();
    devsDbus.waitForFinished();
    QDBusPendingReply<QString> backendDbus=iface->Backend();
    backendDbus.waitForFinished();
    
    DeviceList devs=devsDbus.value();
    QString backend=backendDbus.value();
    QString istatus;
    
    istatus.append(i18n("<b>Backend:</b>"));
    istatus.append("<i>");
    istatus.append(backend);
    istatus.append("</i><br>");
    foreach(Device d,devs){
      istatus.append("<b>");
      istatus.append(d.vendor);
      istatus.append(" ");
      istatus.append(d.device);
      istatus.append(":</b>&nbsp;<i>");
      istatus.append( (d.status==0) ? i18n("Off") : (d.status==1) ? i18n("On") : i18n("Powered") );
      istatus.append("</i><br>");
    }
    this->infoLabel->setText(istatus);
    
}

void SwitchableGraphics::save()
{
    KConfigGroup gen= config->group("General");
    gen.writeEntry<bool>("powerManagement",this->autoSwitch->isChecked());
    gen.writeEntry<bool>("lowBatteryManagement",this->lowBatSwitchFlag->isChecked());
    gen.writeEntry<int>("lowBatteryPercentage",this->inputPerc->value());
    gen.writeEntry<bool>("displayNotifier",this->statusIconFlag->isChecked());
    gen.writeEntry<bool>("displayNotifications",this->notificationsFlag->isChecked());
    config->sync();
    iface->UpdateSettings();
}



#include "switchablegraphics.moc"


