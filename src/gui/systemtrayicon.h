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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

#include "core/onlinestatus.h"

class QDateTime;
class QAction;

class ContactListWindow;
class Account;
class Contact;
class StatusMenu;
class PopupWindowsStack;

class SystemTrayIcon : public QSystemTrayIcon
{
Q_OBJECT
public:
	enum SysTrayPosition {
		Left,
		Right,
		Top,
		Bottom
	};
	SystemTrayIcon(Account* a, ContactListWindow* w, StatusMenu* sm);
	~SystemTrayIcon();

	virtual bool event(QEvent* e);
	SysTrayPosition sysTrayPosition();

Q_SIGNALS:
	void messageActivated(const QByteArray& email);

public Q_SLOTS:
	void newMessage(Contact * from, const QString & to, const QDateTime& dateTime);
	void notificationTypeChange();

private Q_SLOTS:
	void processActivation(QSystemTrayIcon::ActivationReason reason);
	void updateTooltip();
	void newLetter(QString sender, QString subject, const QDateTime& dateTime);
	void newLetters(quint32 unreadMessages);
	void toggleMainWindowVisibility();
	void slotContextMenuAboutToShow();
	void setOnlineStatus(OnlineStatus status);
	void popsStackMouseEntered();
	void popsStackMouseLeaved();
	void iconTimerTimeOver();
	void popusRemovedAll();
	void doExProc(QString prog);

private:
	void setupMainWindowVisibilityAction();

	ContactListWindow* mainWindow;
	Account* account;
	QMenu* contextMenu;
	QPoint mainWindowPos;
	QSize mainWindowSize;
	QTimer* timer;
	QTimer* iconTimer;

	QIcon statusIcon;
	QIcon messageIcon;

	PopupWindowsStack* popsStack;

	QAction* mainWindowVisibilityAction;

	bool m_popupsExists;
	bool iconDisplayOff;
};

#endif
