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
#include <QDateTime>

class Contact;
class ChatSession;
class Message;
class FileMessage;
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
	void messageReceived(ChatSession* session, Message* msg);
	//void multReceived(ChatSession* session, QString multId);
	
private slots:
	void processMessage(QByteArray from, Message* msg);
	//void processMult(QByteArray from, QString multId);
	void processFileMessage(QByteArray from, quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips);
	void processMicroblogChanged(QByteArray from, QString text, QDateTime mbDateTime);
	void removeSession(Contact* contact);
	
private:
	Account* m_account;
	QHash<Contact*, ChatSession*> sessions;
};

#endif
