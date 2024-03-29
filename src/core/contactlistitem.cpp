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

#include "contactlistitem.h"

#include <QDebug>

#include "contact.h"
#include "contactgroup.h"

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
	else if (m_contact->isConference())
	{
		setIcon(QIcon(":icons/msg_conference.png"));

		setDragEnabled(false);
	}
	else
	{
//		checkVisibility();

		connect(m_contact, SIGNAL(statusChanged(OnlineStatus)), this, SLOT(update()));
		connect(m_contact, SIGNAL(groupChanged(bool)), this, SLOT(changeGroup(bool)));
		connect(m_contact, SIGNAL(visibilityChanged()), this, SLOT(update()));
		connect(m_contact, SIGNAL(ignoredChanged()), this, SLOT(update()));
	}
	QFont f = font();
	/*if (!m_contact->isIgnored())
		f.setPointSize(9);
	setFont(f);*/
	setSizeHint(QSize(sizeHint().width(), 52));

	connect(m_contact, SIGNAL(renamed(QString)), this, SLOT(update()));
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

	if (!group->isSimple())
		setDropEnabled(false);

	/*QFont f = font();
	f.setPointSize(9);
	setFont(f);*/
}

ContactListItem::ContactListItem(const QString& groupName)
{	
	m_group = 0;
	m_contact = 0;

	setText(groupName);
	
	setEditable(false);
	setDragEnabled(false);
	setDropEnabled(false);
	/*QFont f = font();
	f.setPointSize(9);
	setFont(f);*/
}

/*void ContactListItem::setStatusIcon(OnlineStatus status)
{
	emitDataChanged();
}*/

void ContactListItem::update()
{
	emitDataChanged();
}

void ContactListItem::changeGroup(bool indeed)
{
	qDebug() << Q_FUNC_INFO << indeed;
	Q_EMIT groupChanged(indeed);
}

void ContactListItem::destroyItem()
{
	delete this;
}
/*
void ContactListItem::checkVisibility()
{
	quint32 flags = m_contact->flags();
	setStatusIcon(m_contact->status());

	QFont f;
	if (flags & CONTACT_FLAG_VISIBLE)
	{
		//f.setItalic(true);
		//setFont(f);
	}
	else if (flags & CONTACT_FLAG_INVISIBLE)
	{
		//f.setStrikeOut(true);
		//setFont(f);
	}
	else if (flags & CONTACT_FLAG_IGNORE)
	{
		//f.setPointSize(7);
		//setFont(f);
		//setIcon(QIcon(":icons/cl_ignore_contact.png"));
	}
	else
	{
		//f.setItalic(false);
		//f.setStrikeOut(false);
		//f.setPointSize(9);
		//setFont(f);
	}
}

void ContactListItem::rename(const QString & nickname)
{
	qDebug() << "ContactListItem::rename" << nickname;
	//setText(nickname);
}
*/
