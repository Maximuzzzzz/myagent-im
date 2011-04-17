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

#include "taskbroadcastmessage.h"

#include <QDebug>

#include "mrimclient.h"
#include "message.h"

namespace Tasks
{

BroadcastMessage::BroadcastMessage(QList<QByteArray> receivers, Message* m, MRIMClient* client, QObject *parent)
	: Task(client, parent), m_receivers(receivers), message(m)
{
}

BroadcastMessage::~BroadcastMessage()
{
}

bool BroadcastMessage::exec()
{
	if (mc == NULL || m_receivers.count() > 0)
	{
		delete this;
		return false;
	}

	connect(mc, SIGNAL(messageStatus(quint32, quint32)), this, SLOT(checkResult(quint32, quint32)));

	setTimer();

	seq = mc->broadcastMessage(message, m_receivers);
	message->setId(seq);
	if (seq == 0)
	{
		delete this;
		return false;
	}

	return true;
}

void BroadcastMessage::checkResult(quint32 msgseq, quint32 status)
{
	if (seq == msgseq)
	{
		unsetTimer();
		emit done(status, false);
		delete this;
	}
}

void BroadcastMessage::timeout()
{
	qDebug() << "BroadcastMessage::timeout()";
	Task::timeout();
}

}

