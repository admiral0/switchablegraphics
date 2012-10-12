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


#include "switchablegraphicshelper.h"
#include <QFile>
#include <QStringList>
#include <QDir>
#include <sys/mount.h>


using namespace KAuth;

ActionReply SwitchableGraphicsHelper::mountdebugfs(QVariantMap args)
{
    Q_UNUSED(args)
    ActionReply reply;

    QFile mounts("/proc/mounts");
    if(!mounts.open(QIODevice::ReadOnly)) {
        reply.setErrorCode(mounts.error());
        reply.setErrorDescription(mounts.errorString());
    } else {
        QString contents=mounts.readAll();
        if(contents.contains("debugfs")) {
            foreach(QString line,contents.split("\n")) {
                if(line.contains("debugfs")) {
                    reply.addData("mountpoint",line.split(" ").at(1));
                }
            }
        } else {
            QDir mountdir("/sys/kernel/debug");
            if(!mountdir.exists()) {
                mountdir.mkpath(mountdir.absolutePath());
            }
            if(mount("none","/sys/kernel/debug","debugfs",MS_RELATIME,0)) {
                reply.setErrorCode(50);
                reply.setErrorDescription("Cannot mount debugfs");
            } else {
                reply.addData("mountpoint","/sys/kernel/debug");
            }
        }
    }
//     log<<"mounted\n";
    return reply;
}

ActionReply SwitchableGraphicsHelper::vgaswitcheroo(QVariantMap args)
{
    ActionReply reply;
    QFile switcher(args.value("filePath","/sys/kernel/debug/vgaswitcheroo/switch").toString());
    if(!switcher.open(QIODevice::WriteOnly)) {
        reply.setErrorCode(switcher.error());
        reply.setErrorDescription(switcher.errorString());
    } else {
        switcher.write(args.value("action","OFF").toByteArray());
        switcher.close();
    }
    return reply;
}

KDE4_AUTH_HELPER_MAIN("org.admiral0.switchablegraphics", SwitchableGraphicsHelper)
