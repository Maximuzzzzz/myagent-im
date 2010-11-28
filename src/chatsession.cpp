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
#include "filemessage.h"
#include "mrimclient.h"
#include "proto.h"
#include "tasksendsms.h"
#include "rtfexporter.h"
#include "audio.h"
#include "resourcemanager.h"

ChatSession::ChatSession(Account* account, Contact* contact)
	: m_account(account), m_contact(contact)
{
	numering = 0;
}

ChatSession::~ChatSession()
{
	qDebug() << "ChatSession::~ChatSession() ";
	qDeleteAll(messages);
	messages.clear();
}

void ChatSession::appendMessage(Message* msg, bool addInHash)
{
	if (addInHash)
	{
		while (messages.value(++numering, NULL) != NULL) {}
		messages.insert(numering, msg);
	}
	emit messageAppended(msg);
}

bool ChatSession::sendMessage(QString plainText, QByteArray rtf)
{
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF, plainText, rtf, 0x00FFFFFF);

	return sendMessage(msg);
}

bool ChatSession::sendMessage(Message* msg)
{
	Task* task = new Tasks::SendMessage(m_contact, msg, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotMessageStatus(quint32, bool)));

	appendMessage(msg);

	return task->exec();
}

void ChatSession::slotMessageStatus(quint32 status, bool timeout)
{	
	Tasks::SendMessage* task = qobject_cast<Tasks::SendMessage*>(sender());
	if (!timeout && status == MESSAGE_DELIVERED)
	{
		qDebug() << "Message delivered";
		messages.remove(messages.key(task->getMessage()));
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
	m_account->contactList()->setLastSmsFrom(number, m_contact);

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
		
		Message* msg = new Message(Message::Error, MESSAGE_FLAG_RTF | MESSAGE_FLAG_ALARM, plainText, rtf, 0x00FFFFFF);
		appendMessage(msg, false);
		return false;
	}

	QString plainText = tr("Wake up!");
	QTextDocument doc(plainText);
	RtfExporter rtfExporter(&doc);
	QByteArray rtf = rtfExporter.toRtf();
	
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF | MESSAGE_FLAG_ALARM, plainText, rtf, 0x00FFFFFF);
	Task* task = new Tasks::SendMessage(m_contact, msg, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotMessageStatus(quint32, bool)));
	
	if (task->exec())
	{
		awakeDelay.start();
		return true;
	}

	return false;
}

bool ChatSession::fileTransfer(FileMessage* fmsg)
{
	qDebug() << "ChatSession::fileTransfer()";

	fmsg->setAccEmail(m_account->email());
	fmsg->setContEmail(m_contact->email());
	fmsg->sendFiles(m_account->client());

	return true;
}

void ChatSession::fileReceived(FileMessage* fmsg)
{
	qDebug() << "ChatSession::fileReceived";
	emit signalFileReceived(fmsg);
}

void ChatSession::resendMessage(quint32 id)
{
	MessageIterator it = messagesBegin();

	bool found = false;
	for (; it != messagesEnd(); ++it)
	{
		if ((*it)->getId() == id)
		{
			found = true;
			break;
		}
	}

	if (found)
	{
		Message* msg = *it;
		sendMessage(msg);
		theRM.getAudio()->play(STOtprav);
	}
}

void ChatSession::clearHash()
{
	messages.clear();
}
