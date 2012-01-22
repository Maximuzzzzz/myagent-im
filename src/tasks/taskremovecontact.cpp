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

#include "taskremovecontact.h"

#include "protocol/mrim/mrimclient.h"

Tasks::RemoveContact::RemoveContact(Contact* c, MRIMClient* client, QObject *parent)
	: SimpleBlockingTask<Tasks::RemoveContact>(client, parent), m_contact(c)
{
}

bool Tasks::RemoveContact::exec()
{
	if (!block())
		return false;
	
	connect(mc, SIGNAL(contactModified(quint32, quint32)), this, SLOT(checkResult(quint32, quint32)));
	
	return checkCall(mc->removeContact(m_contact));
}

void Tasks::RemoveContact::checkResult(quint32 msgseq, quint32 status)
{
	if (isMyResponse(msgseq))
	{
		Q_EMIT done(status, false);
		delete this;
	}
}
