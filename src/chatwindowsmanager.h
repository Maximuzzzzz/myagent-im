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

#ifndef CHATWINDOWSMANAGER_H
#define CHATWINDOWSMANAGER_H

#include <QObject>
#include <QHash>

class QIcon;
class QTabWidget;

class ChatSession;
class ChatWindow;
class Account;

class ChatWindowsManager : public QObject
{
Q_OBJECT
public:
	ChatWindowsManager(Account* account, QObject *parent = 0);
	~ChatWindowsManager();

	ChatWindow* getWindow(ChatSession* session);
	void raiseWindow(ChatWindow* wnd);

	bool isAnyWindowVisible();
	void reloadStatus(bool doUseTabs);
	Account* getAccount() { return m_account; }

signals:
	void ignoreSet(bool ignore);

private slots:
	void mainWindowActivate(ChatWindow* wnd);
	void onShowWindow();
	ChatWindow* createWindow(ChatSession* session);
	void removeWindow(QObject* session);
	void slotRemoveTab(int tab);
	void changeIconAndTitle(const QIcon& icon, ChatWindow* sender);
	void changeTab(int index);

private:
	void loadMainWindow();

	Account* m_account;
	QHash<ChatSession*, ChatWindow*> windows;
	bool useTabs;
	QTabWidget* tabs;
	QWidget* tabsWindow;
};

#endif
