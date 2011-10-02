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

#include "contactlistitemdelegate.h"

#include <QDebug>

#include "contactgroup.h"
#include "contact.h"
#include "protocol/mrim/proto.h"

ContactListItemDelegate::ContactListItemDelegate(ContactListSortFilterProxyModel* m, QObject *parent)
	: QStyledItemDelegate(parent), m_model(m)
{
}

void ContactListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid())
		return;

	painter->save();

	if (option.state & QStyle::State_Selected)
	{
		QBrush brush(QColor(QColor(214, 212, 202)));
		painter->fillRect(option.rect, brush);
	}

	if (!m_model->isGroup(index))
	{
		Contact* contact = m_model->contactFromIndex(index);

		QPixmap avatar;
		if (contact->isConference())
			avatar = QPixmap(":/icons/conference_small.png");
		else if (contact->isPhone())
			avatar = QPixmap(":/icons/sms_avatar_small.png");
		else
			avatar = QPixmap(":/icons/noavatar_small.png");
		avatar = avatar.scaledToHeight(45, Qt::SmoothTransformation);
		int x = option.rect.x();
		int y = option.rect.y() + ((option.rect.height() - avatar.rect().height()) / 2);
		painter->drawPixmap(x, y, avatar);
		painter->setPen(QColor(Qt::gray));
		painter->drawRect(x, y, avatar.rect().width(), avatar.rect().height());

		x += avatar.rect().width() + 3;

		QPixmap statusIcon;
		if (contact->isConference())
			statusIcon = QPixmap(":/icons/msg_conference.png");
		else if (contact->isPhone())
			statusIcon = QPixmap(":/icons/msg_phone.png");
		else
			statusIcon = contact->status().statusIcon().pixmap(QSize(16, 16));
		painter->drawPixmap(x, y, statusIcon);

		QFont f;
		f.setPointSize(8);
		painter->setPen(QColor(Qt::black));
		painter->setFont(f);

		x += 18;

		if (!contact->isPhone() && contact->phones().count() > 0)
		{
			x -= 4;
			QPixmap phone = QPixmap(":/icons/phone.png");
			painter->drawPixmap(x, y, phone);
			x += 16;
		}

		painter->save();
		if (contact->flags() & CONTACT_FLAG_VISIBLE)
			f.setItalic(true);
		else if (contact->flags() & CONTACT_FLAG_INVISIBLE)
			f.setStrikeOut(true);

		painter->setFont(f);
		painter->drawText(x, y + 10, contact->nickname());
		painter->restore();

		x -= 18;

		painter->setPen(QColor(Qt::gray));
		if (contact->isConference())
			painter->drawText(x, y + 26, tr("Online"));
		else if (contact->isPhone())
			painter->drawText(x, y + 26, contact->phones().first());
		else
			painter->drawText(x, y + 26, contact->status().statusDescr());
	}
	else
	{
		int x = option.rect.x() + 3;
		int y = option.rect.y() + 14;
		ContactGroup* group = m_model->groupFromIndex(index);

		QFont f;
		f.setPointSize(8);
		f.setBold(true);
		painter->setPen(QColor(Qt::black));
		painter->setFont(f);

		painter->drawText(x, y, group->name());
	}

	painter->restore();
}
