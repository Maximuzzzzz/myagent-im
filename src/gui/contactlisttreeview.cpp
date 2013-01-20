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

#include "contactlisttreeview.h"

#include <QDebug>
#include <QHeaderView>
#include <QDragMoveEvent>
#include <QDrag>

#include "core/contactmimedata.h"
#include "core/contact.h"
#include "core/contactgroup.h"
#include "contactcontextmenu.h"
#include "conferencecontextmenu.h"
#include "contactgroupcontextmenu.h"
#include "gui/models/contactlistsortfilterproxymodel.h"

ContactListTreeView::ContactListTreeView(Account* account, QWidget *parent)
	: QTreeView(parent), account_(account), contactListModel(0)
{
	header()->hide();
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setFrameStyle(QFrame::NoFrame);
	setDragEnabled(true);
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setDropIndicatorShown(false);

	contactMenu = new ContactContextMenu(account, this);
	groupMenu = new ContactGroupContextMenu(account, this);
	conferenceMenu = new ConferenceContextMenu(account, this);

	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotActivated(const QModelIndex&)));
}

/*QStyleOptionViewItem ContactListTreeView::viewOptions() const
{
	QStyleOptionViewItem options = QTreeView::viewOptions();
	options.showDecorationSelected = false;
	return options;
}*/

void ContactListTreeView::setModel(QAbstractItemModel* model)
{
	contactListModel = qobject_cast<ContactListSortFilterProxyModel*>(model);
	QTreeView::setModel(model);
	connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(groupCollapsed(QModelIndex)));
	connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(groupExpanded(QModelIndex)));
	connect(contactListModel, SIGNAL(expandGroup(QModelIndex)), this, SLOT(expandGroup(QModelIndex)));
}

void ContactListTreeView::dropEvent(QDropEvent * event)
{
	qDebug() << "drop event";
	QTreeView::dropEvent(event);
}

void ContactListTreeView::dragMoveEvent(QDragMoveEvent* event)
{
	QTreeView::dragMoveEvent(event);
	QModelIndex index = indexAt(event->pos());

	if (index.isValid())
	{
		if (contactListModel->isGroup(index) && index != selectedIndexes().first().parent())
		{
			dropRect = visualRect(index);
			dropRect.adjust(0, 0, -1, -1);
		}
	}
	else
		dropRect = QRect();
}

void ContactListTreeView::startDrag(Qt::DropActions supportedActions)
{
	qDebug() << "ContactListTreeView::startDrag(" << supportedActions << ")";
	QModelIndexList indexes = selectedIndexes();
	if (indexes.count() == 1)
	{
		ContactMimeData* data = static_cast<ContactMimeData*>(contactListModel->mimeData(indexes));
		if (!data)
			return;

		//Contact* contact = data->getContactItem()->getContact();

		QDrag *drag = new QDrag(this);
		/*QPixmap avatarPixmap = contact->getAvatar();
		QPixmap dragPixmap(avatarPixmap.width()+2, avatarPixmap.height()+2);
		QPainter painter(&dragPixmap);
		painter.drawPixmap(1, 1, avatarPixmap);
		QRect rect = dragPixmap.rect();
		painter.drawRect(0, 0, rect.right(), rect.bottom());
		drag->setPixmap(dragPixmap);*/
		drag->setMimeData(data);
		qDebug() << "result action = " << drag->exec(supportedActions, Qt::MoveAction);
		dropRect = QRect();
	}
}

/*void ContactListTreeView::paintEvent(QPaintEvent* event)
{
	QTreeView::paintEvent(event);
	QPainter painter(viewport());
	painter.setPen(Qt::DotLine);
	if (dropRect.isValid()) painter.drawRect(dropRect);
}*/

void ContactListTreeView::dragLeaveEvent(QDragLeaveEvent* event)
{
	dropRect = QRect();
	QTreeView::dragLeaveEvent(event);
}

void ContactListTreeView::contextMenuEvent(QContextMenuEvent* e)
{
	QModelIndex index = indexAt(e->pos());

	if (Contact* contact = contactListModel->contactFromIndex(index))
	{
		if (contact->isConference())
		{
			conferenceMenu->setContact(contact);
			conferenceMenu->exec(QCursor::pos());
		}
		else
		{
			contactMenu->setContact(contact);
			contactMenu->exec(QCursor::pos());
		}
	}
	else if (ContactGroup* group = contactListModel->groupFromIndex(index))
	{
		if (group->isSimple())
		{
			groupMenu->setGroup(group);
			groupMenu->exec(QCursor::pos());
		}
	}
}

void ContactListTreeView::slotActivated(const QModelIndex & index)
{
	Contact* contact = contactListModel->contactFromIndex(index);
	if (contact && !contact->isIgnored())
		Q_EMIT contactItemActivated(contact);
}

void ContactListTreeView::groupCollapsed(QModelIndex index)
{
	ContactGroup* group = contactListModel->groupFromIndex(index);
	if (group != NULL)
		group->setExpanded(false);
}

void ContactListTreeView::groupExpanded(QModelIndex index)
{
	ContactGroup* group = contactListModel->groupFromIndex(index);
	if (group != NULL)
		group->setExpanded(true);
}

void ContactListTreeView::expandGroup(QModelIndex index)
{
	expand(contactListModel->mapFromSource(index));
}
