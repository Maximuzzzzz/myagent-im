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

#include "contactlisttreeview.h"

#include <QHeaderView>
#include <QDragMoveEvent>
#include <QPainter>
#include <QDebug>
#include <QLabel>

#include "contactmimedata.h"
#include "contact.h"
#include "contactlistitem.h"
#include "contactcontextmenu.h"
#include "contactgroupcontextmenu.h"
#include "contactlistsortfilterproxymodel.h"
#include "account.h"

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
		if (contactListModel->isGroup(index) && index != selectedIndexes().first().parent())
		{
			dropRect = visualRect(index);
			dropRect.adjust(0, 0, -1, -1);
		}
		else
		{
			dropRect = QRect();
			event->ignore();
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

void ContactListTreeView::paintEvent(QPaintEvent* event)
{
	QTreeView::paintEvent(event);
	QPainter painter(viewport());
	painter.setPen(Qt::DotLine);
	if (dropRect.isValid()) painter.drawRect(dropRect);
}

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
		contactMenu->setContact(contact);
		contactMenu->exec(QCursor::pos());
	}
	else if (ContactGroup* group = contactListModel->groupFromIndex(index))
	{
		groupMenu->setGroup(group);
		groupMenu->exec(QCursor::pos());
	}
}

void ContactListTreeView::slotActivated(const QModelIndex & index)
{
	Contact* contact = contactListModel->contactFromIndex(index);
	if (contact)
		emit contactItemActivated(contact);
}
