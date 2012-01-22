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

#include "taskaddcontact.h"

#include <QDebug>

#include "protocol/mrim/mrimclient.h"

Tasks::AddContact::AddContact(quint32 group, const QByteArray& email, const QString& nickname, const QString& authorizationMessage, MRIMClient* client, QObject* parent)
	: SimpleBlockingTask<Tasks::AddContact>(client, parent), m_group(group), m_nickname(nickname), m_email(email), m_authorizationMessage(authorizationMessage)
{
}

bool Tasks::AddContact::exec()
{
	qDebug() << "Tasks::AddContact::exec()";
	if (!block())
		return false;
	
	connect(mc, SIGNAL(contactAdded(quint32, quint32, quint32)), this, SLOT(checkResult(quint32, quint32, quint32)));
	
	return checkCall(mc->addContact(m_group, m_nickname, m_email, m_authorizationMessage));
}

void Tasks::AddContact::checkResult(quint32 msgseq, quint32 status, quint32 contactId)
{
	if (isMyResponse(msgseq))
	{
		m_contactId = contactId;
		Q_EMIT done(status, false);
		deleteLater();
	}
}

ContactData Tasks::AddContact::contactData() const
{
	return ContactData(m_contactId, 0, m_group, m_email, m_nickname);
}
