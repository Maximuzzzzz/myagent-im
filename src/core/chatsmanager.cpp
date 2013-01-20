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

#include "chatsmanager.h"

#include <QDebug>

#include "mrim/mrimclient.h"
#include "account.h"
#include "contact.h"
#include "contactlist.h"
#include "chatsession.h"
#include "message.h"
#include "mrim/proto.h"

ChatsManager::ChatsManager(Account* account)
 : QObject(account), m_account(account)
{
	connect(m_account->client(), SIGNAL(messageReceived(QByteArray, Message*)), this, SLOT(processMessage(QByteArray, Message*)));
	//connect(m_account->client(), SIGNAL(multReceived(QByteArray, QString)), this, SLOT(processMult(QByteArray, QString)));
	connect(m_account->client(), SIGNAL(fileReceived(QByteArray, quint32, quint32, QByteArray, QString, QByteArray)), this, SLOT(processFileMessage(QByteArray, quint32, quint32, QByteArray, QString, QByteArray)));
	connect(m_account->client(), SIGNAL(microblogChanged(QByteArray, QString, QDateTime)), this, SLOT(processMicroblogChanged(QByteArray, QString, QDateTime)));
}

ChatsManager::~ChatsManager()
{
	Q_FOREACH (ChatSession* session, sessions)
		delete session;

	qDebug() << "ChatsManager::~ChatsManager()";
}

void ChatsManager::processMessage(QByteArray from, Message* msg)
{
	qDebug() << Q_FUNC_INFO << msg->flags();
	Contact* contact;
	
	if (msg->flags() & (MESSAGE_FLAG_SMS | MESSAGE_SMS_DELIVERY_REPORT))
		contact = m_account->contactList()->findContactWithPhone(from);
	else
		contact = m_account->contactList()->getContact(from);

	if (!contact)
	{
		qDebug() << "Sms from unknown number" << from;
		return;
	}

	ChatSession* session = getSession(contact);

	qDebug() << msg->flags();
	session->appendMessage(msg);
	qDebug() << msg->flags();
	Q_EMIT messageReceived(session, msg);
}

/*void ChatsManager::processMult(QByteArray from, QString multId)
{
	Contact* contact = m_account->contactList()->getContact(from);

	ChatSession* session = getSession(contact);
	session->appendMult(multId);

	Q_EMIT multReceived(session, multId);
}*/

void ChatsManager::processFileMessage(QByteArray from, quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips)
{
	ChatSession* session = getSession(m_account->contactList()->getContact(from));
	session->fileReceived(totalSize, sessionId, filesAnsi, filesUtf, ips);
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
		Q_EMIT sessionCreated(session);
		Q_EMIT sessionInitialized(session);
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

void ChatsManager::processMicroblogChanged(QByteArray from, QString text, const QDateTime &mbDateTime)
{
	Contact* contact = m_account->contactList()->getContact(from);

	if (!contact)
	{
		qDebug() << "Could not find contact" << from;
		return;
	}

	ChatSession* session = sessions.value(contact, NULL);
	if (session == NULL)
	{
		contact->setShowMicroblogText(true);
		contact->setMicroblog(text, mbDateTime);
	}
	else
		session->slotMicroblogChanged(text, mbDateTime);
}
