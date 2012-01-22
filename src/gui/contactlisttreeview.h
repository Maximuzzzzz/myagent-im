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

#ifndef CONTACTLISTTREEVIEW_H
#define CONTACTLISTTREEVIEW_H

#include <QTreeView>
#include <QRect>

class Account;
class Contact;
class ContactContextMenu;
class ConferenceContextMenu;
class ContactGroupContextMenu;
class ContactListSortFilterProxyModel;

class ContactListTreeView : public QTreeView
{
Q_OBJECT
public:
	explicit ContactListTreeView(Account* account, QWidget *parent = 0);

	virtual void setModel(QAbstractItemModel* model);

Q_SIGNALS:
	void contactItemActivated(Contact* contact);

protected:
	//virtual QStyleOptionViewItem viewOptions() const;
	virtual void dropEvent(QDropEvent* event);
	virtual void dragMoveEvent(QDragMoveEvent* event);
	virtual void dragLeaveEvent(QDragLeaveEvent* event);
	virtual void startDrag(Qt::DropActions supportedActions);
	//virtual void paintEvent(QPaintEvent* event);
	virtual void contextMenuEvent(QContextMenuEvent* e);

private Q_SLOTS:
	void slotActivated(const QModelIndex& index);
	void groupCollapsed(QModelIndex index);
	void groupExpanded(QModelIndex index);
	void expandGroup(QModelIndex index);

private:
	QRect dropRect;
	Account* account_;
	ContactContextMenu* contactMenu;
	ContactGroupContextMenu* groupMenu;
	ConferenceContextMenu* conferenceMenu;
	ContactListSortFilterProxyModel* contactListModel;
};

#endif
