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

#include "contactlistmodel.h"

#include <QIcon>
#include <QDebug>
#include <QPixmap>
#include <QDir>
#include <QDebug>

#include "contact.h"
#include "contactlist.h"
#include "contactlistitem.h"
#include "contactmimedata.h"
#include "proto.h"

ContactListModel::ContactListModel(ContactList* contactList, bool showGroups)
 : m_showGroups(showGroups)
{
	m_contactList = contactList;
	
	connect(m_contactList, SIGNAL(updated()), this, SLOT(rebuild()));

	connect(m_contactList, SIGNAL(contactAdded(Contact*)), this, SLOT(addContact(Contact*)));
	connect(m_contactList, SIGNAL(contactRemoved(Contact*)), this, SLOT(slotRemoveContactItem(Contact*)));
	connect(m_contactList, SIGNAL(contactIgnored(bool)), this, SIGNAL(modelRebuilded()));

	rebuild();
	
	myFormats << "MyAgent-IM/Contact";
	
	phones = 0;
	conferences = 0;
	temporary = 0;
	notInGroup = 0;
	notAuthorized = 0;
}

ContactListModel::~ContactListModel()
{
	m_contactList = 0;
	phones = 0;
	conferences = 0;
	temporary = 0;
	notInGroup = 0;
	notAuthorized = 0;
}

void ContactListModel::rebuild()
{
	clear();
	groupsMap.clear();
	contactsMap.clear();
	phones = 0;
	conferences = 0;
	temporary = 0;
	notInGroup = 0;
	notAuthorized = 0;
	
	if (m_contactList == NULL) return;
	
	QStandardItem* rootItem = invisibleRootItem();

	if (m_showGroups)
	{
		ContactList::GroupsIterator git = m_contactList->groupsBegin();
		for (; git != m_contactList->groupsEnd(); ++git)
		{
			ContactListItem* groupItem = new ContactListItem(*git);
			groupsMap[*git] = groupItem;
			rootItem->appendRow(groupItem);

			ContactGroup* currGroup = groupFromIndex(groupItem->index());
			if (currGroup->isExpanded())
				emit expandGroup(groupItem->index());
		}
		groupRows = rowCount(indexFromItem(rootItem));
		qDebug() << "ContactListModel::rebuild groupRows = " << groupRows;
	}
	
	ContactList::ContactsIterator cit = m_contactList->contactsBegin();
	for (; cit != m_contactList->contactsEnd(); ++cit)
	{
		addContact(*cit);
	}
	
	emit modelRebuilded();
}

Qt::DropActions ContactListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QMimeData* ContactListModel::mimeData(const QModelIndexList& indexes) const
{
	qDebug() << "mime data";
	if (indexes.empty())
		return NULL;
	
	ContactListItem* item = dynamic_cast<ContactListItem*>(itemFromIndex(indexes.first()));
	ContactMimeData* data = new ContactMimeData(item->contact());
	return data;
}

bool ContactListModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex& parent)
{
	qDebug() << "mime data dropped, parent row = " << parent.row();
	
	if (action == Qt::IgnoreAction)
		return true;
	
	if (!data->hasFormat("MyAgent-IM/Contact"))
		return false;
	
	if (action != Qt::MoveAction)
		return false;
	
	if (!parent.isValid())
		return false;
	
	ContactGroup* newGroup = groupFromIndex(parent);
	if (!newGroup)
		return false;
	
	const ContactMimeData* contactData = static_cast<const ContactMimeData*>(data);
	Contact* contact = contactData->contact();
	contact->changeGroup(newGroup->id());
	
	return false;
}

QStringList ContactListModel::mimeTypes() const
{
	return myFormats;
}

void ContactListModel::changeContactGroup(bool indeed)
{
	qDebug() << "ContactListModel::changeContactGroup " << indeed;
	if (!indeed)
	{
		qDebug() << "can't changeContactGroup";
		return;
	}
	qDebug() << "--";
	qDebug() << "Moving contact";
	ContactListItem* contactItem = qobject_cast<ContactListItem*>(sender());
	qDebug() << "contactItem" << contactItem;
	QStandardItem* removedItem =
		contactItem->QStandardItem::parent()->takeRow(contactItem->row()).first();
	qDebug() << "removedItem";
	QStandardItem* groupItem = groupsMap.value(contactItem->contact()->group());
	qDebug() << "groupItem" << groupItem;
	groupItem->appendRow(removedItem);
	qDebug() << "Contact moved";
}

Contact* ContactListModel::contactFromIndex(const QModelIndex & index)
{
	if (index.isValid())
	{
		ContactListItem* item = static_cast<ContactListItem*>(itemFromIndex(index));
		return item->contact();
	}
	return NULL;
}

ContactGroup* ContactListModel::groupFromIndex(const QModelIndex & index)
{
	if (index.isValid())
	{
		ContactListItem* item = static_cast<ContactListItem*>(itemFromIndex(index));
		return item->contactGroup();
	}
	return NULL;
}

bool ContactListModel::isGroup(const QModelIndex & index)
{
	if (index.isValid())
	{
		ContactListItem* item = static_cast<ContactListItem*>(itemFromIndex(index));
		if (item->contactGroup())
			return true;
	}

	return false;
}

void ContactListModel::addContact(Contact* c)
{
	qDebug() << "ContactListModel::addContact" << c->email();

	if (c->isHidden())
	{
		qDebug() << "hidden contact:";
		qDebug() << c->email();
		return;
	}

	ContactListItem* contactItem = new ContactListItem(c);
	QStandardItem* groupItem;

	if (!m_showGroups)
		groupItem = invisibleRootItem();
	else if (c->isPhone())
	{
		qDebug() << "contact " << c->nickname() << " is phone";
		if (!phones)
		{
			phones = new ContactListItem(m_contactList->phones());
			invisibleRootItem()->insertRow(groupRows++, phones);
			if (m_contactList->phones()->isExpanded())
				emit expandGroup(phones->index());
		}
		
		groupItem = phones;
	}
	else if (c->isConference())
	{
		qDebug() << "contact " << c->nickname() << " is conference";
		if (!conferences)
		{
			qDebug() << "conference group doesn't exists"; //TODO: after deleting doesn't append again
			conferences = new ContactListItem(m_contactList->conferences());
			invisibleRootItem()->insertRow(groupRows++, conferences);
			if (m_contactList->conferences()->isExpanded())
				emit expandGroup(conferences->index());
		}

		groupItem = conferences;
	}
	else if (c->isTemporary())
	{
		qDebug() << "contact " << c->nickname() << " is temporary";
		if (!temporary)
		{
			qDebug() << "temporary group doesn't exists"; //TODO: after deleting doesn't append again
			temporary = new ContactListItem(m_contactList->temporary());
			invisibleRootItem()->insertRow(groupRows++, temporary);
			if (m_contactList->temporary()->isExpanded())
				emit expandGroup(temporary->index());
		}

		groupItem = temporary;
	}
	else if (c->isNotAuthorized())
	{
		qDebug() << "contact " << c->nickname() << " not authorized";
		if (!notAuthorized)
		{
			qDebug() << "not authorized group doesn't exists"; //TODO: after deleting doesn't append again
			notAuthorized = new ContactListItem(m_contactList->notAuthorized());
			invisibleRootItem()->insertRow(groupRows++, notAuthorized);
			if (m_contactList->notAuthorized()->isExpanded())
				emit expandGroup(notAuthorized->index());
		}

		groupItem = notAuthorized;
	}
	else
	{
		connect(contactItem, SIGNAL(groupChanged(bool)), this, SLOT(changeContactGroup(bool)));
		
		ContactGroup* group = c->group();
		if (!group)
		{
			if (!notInGroup)
			{
				notInGroup = new ContactListItem(m_contactList->notInGroup());
				if (m_contactList->notInGroup()->isExpanded())
					emit expandGroup(notInGroup->index());
			}
			groupItem = notInGroup;
		}
		else
		{
			groupItem = groupsMap.value(group);
			if (!groupItem)
			{
				qDebug() << "can't find group item";
				if (!notInGroup)
				{
					notInGroup = new ContactListItem(m_contactList->notInGroup());
					if (m_contactList->notInGroup()->isExpanded())
						emit expandGroup(notInGroup->index());
				}
				groupItem = notInGroup;
			}
		}
	}

	groupItem->appendRow(contactItem);
	qDebug() << "appending contact" << c->email() << c->nickname() << "on" << contactItem;
	contactsMap[c] = contactItem;
}

void ContactListModel::slotRemoveContactItem(Contact* c)
{
	qDebug() << "Deleting contact" << c->email() << c->nickname();

	ContactListItem* removingContactItem = contactsMap.take(c);

	if (c->isHidden())
		return;

	removingContactItem->QStandardItem::parent()->takeRow(removingContactItem->row());
	if (c->isConference())
	{
		if (conferences->rowCount() == 0)
		{
			invisibleRootItem()->takeRow(conferences->row());
			groupRows--;
			delete conferences;
			conferences = 0;
		}
	}
	else if (c->isPhone())
	{
		if (phones->rowCount() == 0)
		{
			invisibleRootItem()->takeRow(phones->row());
			groupRows--;
			delete phones;
			phones = 0;
		}
	}
	else if (c->isTemporary())
	{
		if (temporary->rowCount() == 0)
		{
			invisibleRootItem()->takeRow(temporary->row());
			groupRows--;
			delete temporary;
			temporary = 0;
		}
	}
	else if (c->isNotAuthorized())
	{
		if (notAuthorized->rowCount() == 0)
		{
			invisibleRootItem()->takeRow(notAuthorized->row());
			groupRows--;
			delete notAuthorized;
			notAuthorized = 0;
		}
	}
	else
		if (notInGroup->rowCount() == 0)
		{
			invisibleRootItem()->takeRow(notInGroup->row());
			groupRows--;
			delete notInGroup;
			notInGroup = 0;
		}
	delete c;
}
