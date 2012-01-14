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
#include <QTextCursor>
#include <QTextDocument>

#include "account.h"
#include "contact.h"
#include "contactlist.h"
#include "message.h"
#include "protocol/mrim/mrimclient.h"
#include "protocol/mrim/proto.h"
#include "tasks/tasksendsms.h"
#include "tasks/tasksendmessage.h"
#include "tasks/taskbroadcastmessage.h"
#include "rtfexporter.h"
#include "audio.h"
#include "resourcemanager.h"

ChatSession::ChatSession(Account* account, Contact* contact)
	: m_account(account), m_contact(contact)
{
}

ChatSession::~ChatSession()
{
	qDebug() << Q_FUNC_INFO;
	qDeleteAll(messages);
	messages.clear();
}

void ChatSession::appendMessage(Message* msg, bool enqueue)
{
	qDebug() << Q_FUNC_INFO;
	if (enqueue)
	{
		messages.append(msg);
	}

	Q_EMIT messageAppended(msg);

	if (!enqueue)
		delete msg;
}
/*
void ChatSession::appendMult(QString multId)
{
	Q_EMIT multAppended(multId, msg);
}*/

void ChatSession::appendBroadcastMessage(Message* msg, ReceiversList rec, bool addInHash)
{
	if (addInHash)
	{
		messages.append(msg);
		broadcastMessages.insert(msg, rec);
	}
	Q_EMIT messageAppended(msg);
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

bool ChatSession::broadcastMessage(ReceiversList receivers, QString plainText, QByteArray rtf)
{
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF, plainText, rtf, 0x00FFFFFF);

	return broadcastMessage(msg, receivers);
}

bool ChatSession::broadcastMessage(Message* msg, ReceiversList receivers)
{
	qDebug() << Q_FUNC_INFO;
	Task* task = new Tasks::BroadcastMessage(receivers, msg, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotBroadcastMessageStatus(quint32, bool)));

	appendBroadcastMessage(msg, receivers);

	return task->exec();
}

bool ChatSession::sendMult(const MultInfo* info)
{
	QString text = tr("Your interlocutor sent you %1 mult. But you have old version of myagent-im. You could download new version here: http://code.google.com/p/myagent-im").arg(info->alt());
	QTextDocument doc;
	QTextCursor cursor(&doc);
	cursor.insertHtml(text);

	RtfExporter rtfExporter(&doc);
	Message* msg = new Message(Message::Outgoing, MESSAGE_FLAG_RTF | MESSAGE_FLAG_FLASH, text, rtfExporter.toRtf(), 0x00FFFFFF);
	msg->setMultParameters(info->id(), info->alt());

	return sendMessage(msg);
}

void ChatSession::slotMessageStatus(quint32 status, bool timeout)
{
	Tasks::SendMessage* task = qobject_cast<Tasks::SendMessage*>(sender());
	if (!timeout && status == MESSAGE_DELIVERED)
	{
		qDebug() << "Message delivered";
		messages.removeAll(task->getMessage());
		Q_EMIT messageDelivered(true, task->getMessage());
	}
	else
	{
		qDebug() << "Message NOT delivered";
		Q_EMIT messageDelivered(false, task->getMessage());
	}
}

void ChatSession::slotBroadcastMessageStatus(quint32 status, bool timeout)
{
	Tasks::BroadcastMessage* task = qobject_cast<Tasks::BroadcastMessage*>(sender());
/*	if (!timeout && status == MESSAGE_DELIVERED)
	{*/
		qDebug() << "Message delivered";
		messages.removeAll(task->getMessage());
		broadcastMessages.remove(broadcastMessages.key(task->getReceivers()));
		Q_EMIT messageDelivered(true, task->getMessage());
/*	}
	else
	{
		qDebug() << "Message NOT delivered";
		Q_EMIT messageDelivered(false, task->getMessage());
	}*/
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
		Q_EMIT smsDelivered(task->phoneNumber(), task->text());
	}
	else
		Q_EMIT smsFailed();
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

void ChatSession::fileReceived(quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips)
{
	qDebug() << "ChatSession::fileReceived";
	Q_EMIT signalFileReceived(totalSize, sessionId, filesAnsi, filesUtf, ips);
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

		BroadcastMessageIterator bit = broadcastMessagesBegin();

		found = false;
		ReceiversList rec = ReceiversList();
		for (; bit != broadcastMessagesEnd(); ++bit)
		{
			if (bit.key()->getId() == id)
			{
				rec = (*bit);
				found = true;
				break;
			}
		}

		if (found)
			broadcastMessage(msg, rec);
		else
			sendMessage(msg);
		theRM.getAudio()->play(STOtprav);
	}
}

void ChatSession::clearHash()
{
	messages.clear();
}

void ChatSession::slotMicroblogChanged(QString text, const QDateTime &mbDateTime)
{
	Q_EMIT microblogChanged(text, mbDateTime);
}
