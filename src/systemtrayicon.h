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

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QDateTime>

#include "onlinestatus.h"

class ContactListWindow;
class Account;

class SystemTrayIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
	SystemTrayIcon(Account* a, ContactListWindow* w);
	~SystemTrayIcon();

	virtual bool event(QEvent* e );
signals:
	void statusChanged(OnlineStatus status);
public slots:
	void setOnlineStatus(OnlineStatus status);
	void setStatus(OnlineStatus status);
private slots:
	void processActivation(QSystemTrayIcon::ActivationReason reason);
	void updateTooltip();
	void newLetter(QString sender, QString subject, QDateTime dateTime);
	void showMainWindow();
	void slotStatusChanged(QAction* action);
private:
	OnlineStatus status;

	ContactListWindow* mainWindow;
	Account* account;
	QMenu* contextMenu;
	QPoint mainWindowPos;
	QSize mainWindowSize;

	QAction* createAction(OnlineStatus status);
};

#endif
