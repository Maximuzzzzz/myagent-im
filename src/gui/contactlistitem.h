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

#ifndef CONTACTLISTITEM_H
#define CONTACTLISTITEM_H

#include <QStandardItem>
#include <QObject>

#include "onlinestatus.h"

class Contact;
class ContactGroup;

class ContactListItem : public QObject, public QStandardItem
{
	Q_OBJECT
public:
	ContactListItem(Contact* c);
	ContactListItem(ContactGroup* group);
	ContactListItem(const QString& groupName);

	Contact* contact() { return m_contact; }
	const Contact* contact() const { return m_contact; }
	ContactGroup* contactGroup() const { return m_group; }

Q_SIGNALS:
	void groupChanged(bool indeed);

private Q_SLOTS:
//	void setStatusIcon(OnlineStatus status);
	void changeGroup(bool indeed);
	void destroyItem();
//	void checkVisibility();
//	void rename(const QString & nickname);
	void update();

private:
	Contact* m_contact;
	ContactGroup* m_group;
};

#endif
