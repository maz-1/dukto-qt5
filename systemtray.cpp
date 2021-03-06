/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 * Copyright (C) 2015 Arthur Zamarin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#if defined(NOTIFY_LIBNOTIFY)
#include <libnotify/notify.h>
#endif

#include "systemtray.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QApplication>

SystemTray::SystemTray(DuktoWindow& window, QObject* parent) :
    QSystemTrayIcon(parent),
    window(window)
{
#if defined(NOTIFY_LIBNOTIFY)
    notify_init ("Dukto");
#endif
    auto trayIcon = QIcon(":/dukto.png");
#ifdef __APPLE__
    trayIcon.setIsMask(true);
#endif
    this->setIcon(trayIcon);
#ifndef __APPLE__
    connect(this, &SystemTray::activated, this, &SystemTray::on_activated);
#endif
    QMenu *trayMenu = new QMenu(&window);
    QAction *ShowHide = new QAction(QString("Show/Hide"), trayMenu);
    connect(ShowHide, &QAction::triggered, [=]() { on_activated(QSystemTrayIcon::Trigger); });
    trayMenu->addAction(ShowHide);
    QAction *Exit = new QAction(QString("Exit"), trayMenu);
    connect(Exit, &QAction::triggered, [=]() { on_activated(QSystemTrayIcon::MiddleClick); });
    trayMenu->addAction(Exit);
    this->setContextMenu(trayMenu);
    
}

SystemTray::~SystemTray()
{
#if defined(NOTIFY_LIBNOTIFY)
    notify_uninit();
#endif
}

void SystemTray::on_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            window.setVisible(!window.isVisible());
            break;
        case QSystemTrayIcon::MiddleClick:
            window.close();
            QApplication::quit();
            break;
        case QSystemTrayIcon::Context:
            this->contextMenu()->exec();
            break;
        default:
            break;
    }
}

void SystemTray::received_file(QStringList* files, qint64)
{
    if(files->size() == 1)
        notify("Recieved File", files->at(0));
    else
        notify("Recieved Files and Folders", "");
}

void SystemTray::received_text(QString* text, qint64)
{
    notify("Recieved Text Snippet", *text);
}

void SystemTray::notify(const QString &title, const QString &body)
{
#if defined(NOTIFY_NATIVE_TRAY)
    this->showMessage(title, body);
#elif defined(NOTIFY_LIBNOTIFY)
    NotifyNotification* msg = notify_notification_new(title.toUtf8().constData(), body.toUtf8().constData(), nullptr);
    notify_notification_show (msg, nullptr);
    g_object_unref(G_OBJECT(msg));
#else
    Q_UNUSED(title);
    Q_UNUSED(body);
#endif
}
