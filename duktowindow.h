/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
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
 
#ifndef DUKTOWINDOW_H
#define DUKTOWINDOW_H

#include <QQuickWidget>

#ifdef Q_OS_WIN
class EcWin7;
#endif
class GuiBehind;
class Settings;

class DuktoWindow : public QQuickWidget
{
    Q_OBJECT
public:
    explicit DuktoWindow(GuiBehind* gb, Settings *settings, QWidget *parent = nullptr);
    virtual ~DuktoWindow();
    void showTaskbarProgress(uint percent);
    void hideTaskbarProgress();
    void stopTaskbarProgress();

protected:
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *event);

private:
    GuiBehind *mGuiBehind;
    Settings *mSettings;
#ifdef Q_OS_WIN
    EcWin7 *mWin7 = nullptr;
#endif
};

#endif // DUKTOWINDOW_H
