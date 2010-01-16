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

#include "chatsmanager.h"

#include <QDebug>

#include "mrimclient.h"
#include "account.h"
#include "contact.h"
#include "contactlist.h"
#include "chatsession.h"
#include "message.h"
#include "proto.h"
#include "audio.h"

ChatsManager::ChatsManager(Account* account)
 : QObject(account), m_account(account)
{
	connect(m_account->client(), SIGNAL(messageReceived(QByteArray, Message*)), this, SLOT(processMessage(QByteArray, Message*)));
}

ChatsManager::~ChatsManager()
{
	qDebug() << "ChatsManager::~ChatsManager()";
}

void ChatsManager::processMessage(QByteArray from, Message* msg)
{
	Contact* contact;
	
	if (msg->flags() & (MESSAGE_FLAG_SMS | MESSAGE_FLAG_SMS_STATUS))
		contact = m_account->contactList()->findContactWithPhone(from);
	else
		contact = m_account->contactList()->getContact(from);

	if (!contact)
	{
		qDebug() << "Sms from unknown number" << from;
		return;
	}

	ChatSession* session = getSession(contact);
	if (msg->type() == Message::Incoming)
	{
		if (msg->flags() & MESSAGE_FLAG_BELL)
			audio.play(STRing);
		else
			audio.play(STMessage);
	}
	session->appendMessage(msg);
}

ChatSession* ChatsManager::getSession(Contact* contact)
{
	ChatSession* session = sessions.value(contact, NULL);
	if (session == NULL)
	{
		qDebug() << "ChatsManager::getSession creating session";
		session = new ChatSession(m_account, contact);
		connect(contact, SIGNAL(destroyed(Contact*)), this, SLOT(removeSession(Contact*)));
		sessions.insert(contact, session);
		emit sessionCreated(session);
		emit sessionInitialized(session);
	}
	
	return session;
}

void ChatsManager::removeSession(Contact* contact)
{
	qDebug() << "ChatsManager::removeSession";
	ChatSession* session = sessions.take(contact);
	//qDebug() << "session = " << (void*)session;
	delete session;
}
