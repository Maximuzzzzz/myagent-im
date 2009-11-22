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

#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <QObject>
#include <QList>

#include "message.h"

class Account;
class Contact;

class ChatSession : public QObject
{
Q_OBJECT
public:
	ChatSession(Account* account, Contact* contact);
	~ChatSession();

	Account* account() { return m_account; }
	Contact* contact() { return m_contact; }
	
	const Message* getLastMessage() const;

	typedef QList<Message*>::const_iterator MessageIterator;
	MessageIterator messagesBegin() const { return messages.begin(); }
	MessageIterator messagesEnd() const { return messages.end(); }

signals:
	void messageAppended(const Message* msg) const;
	void messageDelivered(bool really);
	void smsDelivered(QByteArray phoneNumber, QString text);
	void smsFailed();

public slots:	
	void appendMessage(Message* msg);
	bool sendMessage(QString plainText, QByteArray rtf);
	bool sendSms(QByteArray number, QString text);
	void sendTyping();
	bool wakeupContact();
private slots:
	void slotMessageStatus(quint32 status, bool timeout);
	void slotSmsStatus(quint32 status, bool timeout);
	
private:
	Account* m_account;
	Contact* m_contact;
	QList<Message*> messages;
	
	QTime typingTime;
	QTime awakeDelay;
};

#endif
