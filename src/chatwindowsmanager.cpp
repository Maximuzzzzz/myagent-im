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

#include "chatwindowsmanager.h"

#include <QDebug>
#include "contact.h"


#include "chatwindow.h"
#include "chatsession.h"

ChatWindowsManager::ChatWindowsManager(Account* account, QObject *parent)
	: QObject(parent), m_account(account)
{
}

ChatWindowsManager::~ChatWindowsManager()
{
	qDebug() << "ChatWindowsManager::~ChatWindowsManager()";
	QList<ChatWindow*> wnds = windows.values();
	qDebug() << "ChatWindowsManager::~ChatWindowsManager() windows num = " << wnds.size();
	qDeleteAll(wnds);
	qDebug() << "ChatWindowsManager::~ChatWindowsManager() finished";
}

ChatWindow* ChatWindowsManager::getWindow(ChatSession* session)
{
	ChatWindow* wnd = windows.value(session, NULL);
	if (wnd == NULL)
		wnd = createWindow(session);
	
	wnd->setGeometry(wnd->geometry());
	return wnd;
}

void ChatWindowsManager::removeWindow(QObject* session)
{
	qDebug() << "ChatWindowsManager::removeWindow";
	ChatSession* s = static_cast<ChatSession*>(session);
	ChatWindow* wnd = windows.take(s);
	delete wnd;
}

ChatWindow* ChatWindowsManager::createWindow(ChatSession* session)
{
	ChatWindow* wnd = new ChatWindow(m_account, session);
	windows.insert(session, wnd);
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(removeWindow(QObject*)));

	return wnd;
}
