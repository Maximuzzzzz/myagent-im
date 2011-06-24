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

#include "contactlistbroadcastmodel.h"

#include <QIcon>
#include <QDebug>
#include <QPixmap>
#include <QDir>
#include <QDebug>

#include "contact.h"
#include "contactlist.h"
#include "gui/contactlistitem.h"
#include "protocol/mrim/proto.h"

ContactListBroadcastModel::ContactListBroadcastModel(QByteArray contactEmail, ContactList* contactList)
 : m_contactEmail(contactEmail)
{
	this->contactList = contactList;

	connect(contactList, SIGNAL(updated()), this, SLOT(rebuild()));

	connect(contactList, SIGNAL(contactAdded(Contact*)), this, SLOT(addContact(Contact*)));
	connect(contactList, SIGNAL(contactRemoved(Contact*)), this, SLOT(slotRemoveContactItem(Contact*)));
	connect(contactList, SIGNAL(contactIgnored(bool)), this, SLOT(rebuild()));

	rebuild();
}

ContactListBroadcastModel::~ContactListBroadcastModel()
{
}

void ContactListBroadcastModel::rebuild()
{
	clear();
	contactsMap.clear();

	if (contactList == NULL) return;

	ContactList::ContactsIterator cit = contactList->contactsBegin();
	for (; cit != contactList->contactsEnd(); ++cit)
		addContact(*cit);
}

Contact* ContactListBroadcastModel::contactFromIndex(const QModelIndex & index)
{
	if (index.isValid())
	{
		ContactListItem* item = static_cast<ContactListItem*>(itemFromIndex(index));
		return item->contact();
	}
	return NULL;
}

void ContactListBroadcastModel::addContact(Contact* c)
{
//	qDebug() << "ContactListBroadcastModel::addContact" << c->email();

	if (c->isHidden() || c->isIgnored())
	{
		qDebug() << "hidden or ignored contact:";
		qDebug() << c->email();
		return;
	}

	ContactListItem* contactItem = new ContactListItem(c);
	contactItem->setCheckable(true);
	if (c->email() == m_contactEmail)
		contactItem->setCheckState(Qt::Checked);

	invisibleRootItem()->appendRow(contactItem);
	contactsMap[c] = contactItem;
}

void ContactListBroadcastModel::slotRemoveContactItem(Contact* c)
{
	qDebug() << "Deleting contact" << c->email() << c->nickname();

	ContactListItem* removingContactItem = contactsMap.take(c);

	if (c->isHidden())
		return;

	removingContactItem->QStandardItem::parent()->takeRow(removingContactItem->row());
}

QList<QByteArray> & ContactListBroadcastModel::broadcastList()
{
	res.clear();
	QMap<Contact*, ContactListItem*>::iterator it = contactsMap.begin();
	for (; it != contactsMap.end(); ++it)
	{
		ContactListItem* item = it.value();
		if (item->checkState() == Qt::Checked)
			res.append(it.key()->email());
	}
	return res;
}
