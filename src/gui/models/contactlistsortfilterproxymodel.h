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

#ifndef CONTACTLISTSORTFILTERPROXYMODEL_H
#define CONTACTLISTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class Contact;
class ContactGroup;
class ContactListModel;

class ContactListSortFilterProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
public:
	ContactListSortFilterProxyModel(QObject * parent = 0);

	bool isGroup(const QModelIndex& index);
	Contact* contactFromIndex(const QModelIndex& index) const;
	ContactGroup* groupFromIndex(const QModelIndex & index) const;

	virtual void setSourceModel(QAbstractItemModel* sourceModel);

public Q_SLOTS:
	void setFilterString(const QString&);
	void allowOnlineOnlyContacts(bool allow);

Q_SIGNALS:
	void modelRebuilded();
	void expandGroup(QModelIndex index);

protected:
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
	virtual bool filterAcceptsColumn(int source_column, const QModelIndex & source_parent) const;
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
	ContactListModel* contactListModel;
	QString filter;
	bool allowOnlineOnly;
};

#endif
