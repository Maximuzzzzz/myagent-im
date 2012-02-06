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

#include "taskaddsmscontact.h"

#include "mrim/mrimclient.h"
#include "mrim/proto.h"

Tasks::AddSmsContact::AddSmsContact(const QString & nickname, const QStringList& phones, MRIMClient * client, QObject * parent)
	: SimpleBlockingTask<Tasks::AddSmsContact>(client, parent), m_nickname(nickname), m_phones(phones)
{
}

bool Tasks::AddSmsContact::exec()
{
	if (!block())
		return false;
	
	connect(mc, SIGNAL(contactAdded(quint32, quint32, quint32)), this, SLOT(checkResult(quint32, quint32, quint32)));
	
	return checkCall(mc->addSmsContact(m_nickname, m_phones));
}

ContactData Tasks::AddSmsContact::contactData() const
{
	return ContactData(m_contactId, CONTACT_FLAG_PHONE, SMS_CONTACT_GROUP, "phone", m_nickname, m_phones);
}

void Tasks::AddSmsContact::checkResult(quint32 msgseq, quint32 status, quint32 contactId)
{
	if (isMyResponse(msgseq))
	{
		m_contactId = contactId;
		Q_EMIT done(status, false);
		deleteLater();
	}
}
