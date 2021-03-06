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

#include "duktowindow.h"
#include "guibehind.h"
#include "platform.h"
#include "settings.h"

#ifdef Q_OS_WIN
#include "ecwin7.h"
#endif

#include <QQmlEngine>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>

DuktoWindow::DuktoWindow(GuiBehind *gb, Settings *settings, QWidget *parent) :
    QQuickWidget(parent), mGuiBehind(gb), mSettings(settings)
{
    // Configure window
    setAcceptDrops(true);
    setWindowTitle("Dukto");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setMaximumSize(350, 5000);
    setMinimumSize(350, 500);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    connect(engine(), &QQmlEngine::quit, this, &DuktoWindow::close);
}

DuktoWindow::~DuktoWindow() {
#ifdef Q_OS_WIN
    delete mWin7;
#endif
}

void DuktoWindow::showTaskbarProgress(uint percent) {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::Normal);
        mWin7->setProgressValue(percent, 100);
    }
#else
    Q_UNUSED(percent)
#endif
}

void DuktoWindow::hideTaskbarProgress() {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::NoProgress);
    }
#endif
}

void DuktoWindow::stopTaskbarProgress() {
#ifdef Q_OS_WIN
    if (mWin7 != nullptr) {
        mWin7->setProgressState(EcWin7::Error);
    }
#endif
}

#ifdef Q_OS_WIN
bool DuktoWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType)
    if (mWin7 != nullptr) {
        return mWin7->winEvent(reinterpret_cast<MSG*>(message), result);
    } else {
        return false;
    }
}
#endif

void DuktoWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && mGuiBehind->canAcceptDrop())
        event->acceptProposedAction();
}

void DuktoWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void DuktoWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void DuktoWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (!mimeData->hasUrls()) return;

    QStringList files;
    const QList<QUrl> urlList = mimeData->urls();
    for(QList<QUrl>::const_iterator url = urlList.constBegin(); url != urlList.constEnd(); ++url)
        files.append(url->toLocalFile());

    event->acceptProposedAction();
    mGuiBehind->sendDroppedFiles(&files);
}

void DuktoWindow::closeEvent(QCloseEvent *)
{
    mSettings->saveWindowGeometry(saveGeometry());
}

void DuktoWindow::showEvent(QShowEvent *event) {
    Q_UNUSED(event)
#ifdef Q_OS_WIN
    // Taskbar integration with Win7+
    if (mWin7 == nullptr) {
        mWin7 = new EcWin7(this->windowHandle());
    }
#endif
}