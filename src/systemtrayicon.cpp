/***************************************************************************
 *   Copyright (C) 2008 by Alexander Volkov                                *
 *   volkov0aa@gmail.com                                                   *
 *                                                                         *
 *   This file is part of instant messenger MyAgent-IM                     *
 *                                                                         *
 *   MyAgent-IM is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   MyAgent-IM is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "systemtrayicon.h"

#include <QWidget>
#include <QDebug>
#include <QApplication>

#include "onlinestatus.h"
#include "resourcemanager.h"
#include "contactlistwindow.h"
#include "account.h"
#include "mrimclient.h"

SystemTrayIcon::SystemTrayIcon(Account* a, ContactListWindow* w)
 : QSystemTrayIcon(w), mainWindow(w), account(a)
{
	updateTooltip();
	setOnlineStatus(OnlineStatus::offline);
	connect(account, SIGNAL(nicknameChanged()), this, SLOT(updateTooltip()));	
	connect(account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(setOnlineStatus(OnlineStatus)));

	connect(account->client(), SIGNAL(newLetter(QString, QString, QDateTime)), this, SLOT(newLetter(QString, QString, QDateTime)));

	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(processActivation(QSystemTrayIcon::ActivationReason)));
	
	contextMenu = new QMenu;
	contextMenu->addAction(QIcon(":icons/exit.png"), tr("Quit"), qApp, SLOT(quit()));
	setContextMenu(contextMenu);
}

SystemTrayIcon::~SystemTrayIcon()
{
	delete contextMenu;
}

void SystemTrayIcon::setOnlineStatus(OnlineStatus status)
{
	setIcon(status.chatWindowIcon());
}

void SystemTrayIcon::processActivation(QSystemTrayIcon::ActivationReason reason)
{
	if (mainWindow && reason == QSystemTrayIcon::Trigger)
	{
		if (mainWindow->isMinimized())
		{
			mainWindow->hide();
			mainWindow->showNormal();
		}
		else if (mainWindow->isVisible() && !mainWindow->isActiveWindow())
		{
			mainWindow->extHide();
			mainWindow->extShow();
		}
		else if (mainWindow->isVisible())
		{
			mainWindow->extHide();
		}
		else if (!mainWindow->isVisible())
		{
			mainWindow->extShow();
			mainWindow->raise();
		}
	}
}

void SystemTrayIcon::updateTooltip()
{
	QString nickname = account->nickname();
	if (nickname.isEmpty())
#ifdef Q_WS_X11
		setToolTip("<b>" + account->email() + "</b>");
#else
		setToolTip(account->email());
#endif
	else
#ifdef Q_WS_X11
		setToolTip("<b>" + account->nickname() + "</b><br>" + account->email());
#else
		setToolTip(account->nickname() + "\n" + account->email());
#endif
}

bool SystemTrayIcon::event(QEvent* e)
{
	qDebug() << "SystemTrayIcon::event type = " << e->type();
	return QSystemTrayIcon::event(e);
}

void SystemTrayIcon::newLetter(QString sender, QString subject, QDateTime dateTime)
{
	showMessage(tr("New letter"), sender + "\n" + subject + "\n" + dateTime.toString(), QSystemTrayIcon::Information, 5000);
}
