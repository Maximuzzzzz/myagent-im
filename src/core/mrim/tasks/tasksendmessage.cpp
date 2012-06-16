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

#include "tasksendmessage.h"

#include "mrim/mrimclient.h"
#include "contact.h"
#include "message.h"

namespace Tasks
{

SendMessage::SendMessage(Contact* c, Message* m, MRIMClient* client, QObject *parent)
	: Task(client, parent), contact(c), message(m)
{
}

SendMessage::~SendMessage()
{
}

bool SendMessage::exec()
{
	if (mc == NULL || contact == NULL)
	{
		delete this;
		return false;
	}

	connect(mc, SIGNAL(messageStatus(quint32, quint32)), this, SLOT(checkResult(quint32, quint32)));

	setTimer();

	seq = mc->sendMessage(contact->email(), message);
	message->setId(seq);
	if (seq == 0)
	{
		delete this;
		return false;
	}

	return true;
}

void SendMessage::checkResult(quint32 msgseq, quint32 status)
{
	if (seq == msgseq)
	{
		unsetTimer();
		Q_EMIT done(status, false);
		delete this;
	}
}

void SendMessage::timeout()
{
	Task::timeout();
}

}
