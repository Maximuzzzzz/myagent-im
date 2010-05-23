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

#include "contactlistsortfilterproxymodel.h"

#include "contactlistmodel.h"

ContactListSortFilterProxyModel::ContactListSortFilterProxyModel(QObject * parent)
	: QSortFilterProxyModel(parent), contactListModel(0)
{
}

void ContactListSortFilterProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
	contactListModel = qobject_cast<ContactListModel*>(sourceModel);
	connect(contactListModel, SIGNAL(modelRebuilded()), this, SIGNAL(modelRebuilded()));
	QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool ContactListSortFilterProxyModel::isGroup(const QModelIndex& index)
{
	return contactListModel->isGroup(mapToSource(index));
}

Contact* ContactListSortFilterProxyModel::contactFromIndex(const QModelIndex& index)
{
	return contactListModel->contactFromIndex(mapToSource(index));
}

ContactGroup* ContactListSortFilterProxyModel::groupFromIndex(const QModelIndex & index)
{
	return contactListModel->groupFromIndex(mapToSource(index));
}

/*bool ContactListSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right ) const
{

}*/
