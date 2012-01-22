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

#ifndef CONTACTLISTBROADCASTMODEL_H
#define CONTACTLISTBROADCASTMODEL_H

#include <QStandardItemModel>

class ContactList;
class ContactListItem;
class Contact;

class ContactListBroadcastModel : public QStandardItemModel
{
Q_OBJECT
public:
	explicit ContactListBroadcastModel(QByteArray contactEmail, ContactList* contactList = 0, QObject* parent = 0);
	~ContactListBroadcastModel();

	Contact* contactFromIndex(const QModelIndex& index);
	QList<QByteArray> & broadcastList();

private Q_SLOTS:
	void rebuild();
	void addContact(Contact* c);
	void slotRemoveContactItem(Contact* c);

private:
	ContactList* contactList;
	QMap<Contact*, ContactListItem*> contactsMap;
	QByteArray m_contactEmail;
	QList<QByteArray> res;
};
#endif // CONTACTLISTBROADCASTMODEL_H
