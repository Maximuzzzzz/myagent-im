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

#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QStandardItemModel>

class ContactList;
class ContactListItem;
class Contact;
class ContactGroup;

class ContactListModel : public QStandardItemModel
{
Q_OBJECT
public:
	ContactListModel(ContactList* contactList = NULL, bool showGroups = true);
	~ContactListModel();
	virtual Qt::DropActions supportedDropActions() const;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
	virtual QStringList mimeTypes() const;

	bool isGroup(const QModelIndex& index);
	Contact* contactFromIndex(const QModelIndex& index);
	ContactGroup* groupFromIndex(const QModelIndex & index);

signals:
	void modelRebuilded();

private slots:
	void rebuild();
	void changeContactGroup(bool indeed);
	void addContact(Contact* c);
	void slotRemoveContactItem(Contact* c);

private:
	ContactList* contactList;
	QStringList myFormats;
	int groupRows;
	QMap<ContactGroup*, ContactListItem*> groupsMap;
	QMap<Contact*, ContactListItem*> contactsMap;
	ContactListItem* phones;
	ContactListItem* conferences;
	bool m_showGroups;
};

#endif
