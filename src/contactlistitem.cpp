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

#include "contactlistitem.h"

#include <QTextDocument>
#include <QDebug>

#include "contact.h"
#include "proto.h"

ContactListItem::ContactListItem(Contact* contact)
{	
	m_contact = contact;
	m_group = 0;
	
	setEditable(false);
	setDragEnabled(true);
	setDropEnabled(false);

	setText(m_contact->nickname());

	if (m_contact->isPhone())
	{
		setIcon(QIcon(":icons/phone.png"));

		setDragEnabled(false);
	}
	else
	{
		checkVisibility();
		setStatusIcon(m_contact->status());

		connect(m_contact, SIGNAL(statusChanged(OnlineStatus)), this, SLOT(setStatusIcon(OnlineStatus)));
		connect(m_contact, SIGNAL(groupChanged(bool)), this, SLOT(changeGroup(bool)));
		connect(m_contact, SIGNAL(visibilityChanged()), this, SLOT(checkVisibility()));
	}
	QFont f = font();
	f.setPointSize(9);
	setFont(f);

	connect(m_contact, SIGNAL(renamed(QString)), this, SLOT(rename(const QString&)));
	connect(m_contact, SIGNAL(destroyed(QObject*)), this, SLOT(destroyItem()));
}

ContactListItem::ContactListItem(ContactGroup* group)
{	
	m_group = group;
	m_contact = 0;

	setEditable(false);
	setDragEnabled(false);
	setDropEnabled(true);

	if (!group)
	{
		setText(tr("Temporary"));
		setDropEnabled(false);
	}
	else
		setText(group->name());
	QFont f = font();
	f.setPointSize(9);
	setFont(f);
}

ContactListItem::ContactListItem(const QString& groupName)
{	
	m_group = 0;
	m_contact = 0;

	setText(groupName);
	
	setEditable(false);
	setDragEnabled(false);
	setDropEnabled(false);
	QFont f = font();
	f.setPointSize(9);
	setFont(f);
}

void ContactListItem::setStatusIcon(OnlineStatus status)
{
	setIcon(status.contactListIcon());
}

void ContactListItem::changeGroup(bool indeed)
{
	qDebug() << "ContactListItem::groupChanged " << indeed;
	emit groupChanged(indeed);
}

void ContactListItem::destroyItem()
{
	delete this;
}

void ContactListItem::checkVisibility()
{
	quint32 flags = m_contact->flags();
	
	QFont f;
	if (flags & CONTACT_FLAG_VISIBLE)
	{
		f.setItalic(true);
		setFont(f);
	}
	else if (flags & CONTACT_FLAG_INVISIBLE)
	{
		f.setStrikeOut(true);
		setFont(f);
	}
	else
	{
		f.setItalic(false);
		f.setStrikeOut(false);
		setFont(f);
	}
}

void ContactListItem::rename(const QString & nickname)
{
	setText(nickname);
}
