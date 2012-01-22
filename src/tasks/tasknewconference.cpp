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

#include "tasknewconference.h"

#include "protocol/mrim/mrimclient.h"

Tasks::NewConference::NewConference(QString confName, QByteArray owner, MRIMClient* client, QList<QByteArray> members, QObject* parent)
	: SimpleBlockingTask<Tasks::NewConference>(client, parent), m_conferenceName(confName), m_members(members), m_owner(owner)
{
}

bool Tasks::NewConference::exec()
{
	if (!block())
		return false;
	
	connect(mc, SIGNAL(conferenceBegan(quint32, quint32, quint32, const QByteArray&)), this, SLOT(checkResult(quint32, quint32, quint32, const QByteArray&)));
	
	return checkCall(mc->addConference(m_conferenceName, m_owner, m_members));
}

void Tasks::NewConference::checkResult(quint32 msgseq, quint32 status, quint32 contactId, const QByteArray& chatAgent)
{
	if (isMyResponse(msgseq))
	{
		m_contactId = contactId;
		m_chatAgent = chatAgent;
		Q_EMIT done(status, false);
		deleteLater();
	}
}

ContactData Tasks::NewConference::contactData() const
{
	return ContactData(m_contactId, 0, 0, m_chatAgent, m_conferenceName);
}
