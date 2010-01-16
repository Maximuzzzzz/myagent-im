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

#include "chatsession.h"

#include <QDebug>

#include "account.h"
#include "contact.h"
#include "message.h"
#include "mrimclient.h"
#include "proto.h"
#include "tasksendmessage.h"
#include "tasksendsms.h"
#include "rtfexporter.h"

ChatSession::ChatSession(Account* account, Contact* contact)
	: m_account(account), m_contact(contact)
{
}

ChatSession::~ChatSession()
{
	qDebug() << "ChatSession::~ChatSession() ";
	//qDebug() << "email = " << m_contact->email();
	qDeleteAll(messages);
	messages.clear();
}

void ChatSession::appendMessage(Message* msg)
{
	messages.append(msg);
	emit messageAppended(msg);
}

bool ChatSession::sendMessage(QString plainText, QByteArray rtf)
{
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF, plainText, rtf, 0x00FFFFFF);
	Task* task = new Tasks::SendMessage(m_contact, msg, m_account->client());
	appendMessage(msg);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotMessageStatus(quint32, bool)));

	return task->exec();
}

const Message* ChatSession::getLastMessage() const
{
	if (!messages.isEmpty())
		return messages.last();
	
	return NULL;
}

void ChatSession::slotMessageStatus(quint32 status, bool timeout)
{	
	Tasks::SendMessage* task = qobject_cast<Tasks::SendMessage*>(sender());
	if (!timeout && status == MESSAGE_DELIVERED)
	{
		qDebug() << "Message delivered";
		emit messageDelivered(true, task->getMessage());
	}
	else
{
qDebug() << "Message NOT delivered";
		emit messageDelivered(false, task->getMessage());
}
}

void ChatSession::sendTyping()
{
	int msec = typingTime.elapsed();
	if (0 <= msec && msec < 10000)
		return;
	
	m_account->client()->sendTyping(m_contact->email());
	typingTime.start();
}

bool ChatSession::sendSms(QByteArray number, QString text)
{
	Task* task = new Tasks::SendSms(number, text, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotSmsStatus(quint32, bool)));

	return task->exec();
}

void ChatSession::slotSmsStatus(quint32 status, bool timeout)
{
	if (!timeout && status == SMS_STATUS_OK)
	{
		Tasks::SendSms* task = qobject_cast<Tasks::SendSms*>(sender());
		emit smsDelivered(task->phoneNumber(), task->text());
	}
	else
		emit smsFailed();
}

bool ChatSession::wakeupContact()
{
	int msec = awakeDelay.elapsed();
	if (msec < 30000)
	{
		QString plainText = tr("You can't use alarm clock so often!");
		QTextDocument doc(plainText);
		RtfExporter rtfExporter(&doc);
		QByteArray rtf = rtfExporter.toRtf();
		
		Message* msg = new Message(Message::Error, MESSAGE_FLAG_RTF | MESSAGE_FLAG_BELL, plainText, rtf, 0x00FFFFFF);
		appendMessage(msg);
		return false;
	}

	QString plainText = tr("Wake up!");
	QTextDocument doc(plainText);
	RtfExporter rtfExporter(&doc);
	QByteArray rtf = rtfExporter.toRtf();
	
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF | MESSAGE_FLAG_BELL, plainText, rtf, 0x00FFFFFF);
	Task* task = new Tasks::SendMessage(m_contact, msg, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotMessageStatus(quint32, bool)));
	
	if (task->exec())
	{
		awakeDelay.start();
		return true;
	}

	return false;
}
