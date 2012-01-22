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

#ifndef CONTACTLISTCONFERENCE_H
#define CONTACTLISTCONFERENCE_H

#include <QListView>

#include "onlinestatus.h"

class Contact;
class Account;
class ConferenceListModel;

class ContactListConference : public QListView
{
	Q_OBJECT
public:
	ContactListConference(Contact* conference, Account* acc, QWidget* parent = 0);
	~ContactListConference();

Q_SIGNALS:
	void setMembersCount(quint32);

private Q_SLOTS:
	void addContact(const QByteArray& contact);
	void onlineStatusChanged(OnlineStatus);

private:
	Account* m_account;
	Contact* m_conf;
	bool accountWasConnected;

	ConferenceListModel* m_model;
	QList<Contact*> m_contacts;

	quint32 membersCount;
};

#endif // CONTACTLISTCONFERENCE_H
