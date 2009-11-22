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

#ifndef CHATSMANAGER_H
#define CHATSMANAGER_H

#include <QObject>
#include <QHash>

class Contact;
class ChatSession;
class Message;
class Account;

class ChatsManager : public QObject
{
Q_OBJECT
public:
	ChatsManager(Account* account);
	~ChatsManager();

	ChatSession* getSession(Contact* c);
	
signals:
	void sessionCreated(ChatSession* session);
	void sessionInitialized(ChatSession* session);
	
private slots:
	void processMessage(QByteArray from, Message* msg);
	void removeSession(Contact* contact);
	
private:
	Account* m_account;
	QHash<Contact*, ChatSession*> sessions;
};

#endif
