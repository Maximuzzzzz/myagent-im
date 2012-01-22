/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#include "contactlistconference.h"

#include <QDebug>

#include "account.h"
#include "contact.h"
#include "models/conferencelistmodel.h"

ContactListConference::ContactListConference(Contact* conference, Account* acc, QWidget* parent)
 : m_account(acc), m_conf(conference)
{
	qDebug() << Q_FUNC_INFO;
	connect(acc->client(), SIGNAL(conferenceClAddContact(const QByteArray&)), this, SLOT(addContact(const QByteArray&)));
	connect(acc, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(onlineStatusChanged(OnlineStatus)));
	accountWasConnected = acc->onlineStatus().connected();
	membersCount = 0;
	if (acc->onlineStatus().connected())
		acc->client()->conferenceClLoad(conference->email());

	m_model = new ConferenceListModel();
	setModel(m_model);
}

ContactListConference::~ContactListConference()
{
}

void ContactListConference::addContact(const QByteArray& contact)
{
	qDebug() << Q_FUNC_INFO << contact;
	membersCount++;
	m_model->addContact(contact);
	Q_EMIT setMembersCount(membersCount);
}

void ContactListConference::onlineStatusChanged(OnlineStatus st)
{
	qDebug() << Q_FUNC_INFO << st.connected();
	if (!accountWasConnected && st.connected())
		m_account->client()->conferenceClLoad(m_conf->email());

	if (!st.connected())
	{
		membersCount = 0;
		Q_EMIT setMembersCount(0);
	}
}
