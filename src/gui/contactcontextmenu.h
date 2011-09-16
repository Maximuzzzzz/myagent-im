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

#ifndef CONTACTCONTEXTMENU_H
#define CONTACTCONTEXTMENU_H

#include <QMenu>
#include <QDateTime>

#include "onlinestatus.h"
#include "contactgroup.h"

class QAction;
class Account;
class Contact;

class SubmenuMoveToGroup : public QMenu
{
Q_OBJECT
public:
	SubmenuMoveToGroup(Contact* c, QWidget* parent = 0);
	~SubmenuMoveToGroup();

	quint32 groupsCount() { return m_groups.count(); };

	void setContact(Contact* c);
Q_SIGNALS:
	void moveContact(quint32);

public Q_SLOTS:
	void addGroup(ContactGroup* group);
	void renameGroup(ContactGroup* group);
	void removeGroup(ContactGroup* group);
	void clearAll();

private Q_SLOTS:
	void moveContactTo();

private:
	//QHash<quint32, QAction*> m_groups;
	QMap<ContactGroup*, QAction*> m_groups;
	Contact* m_contact;
};

class ContactContextMenu : public QMenu
{
Q_OBJECT
public:
	ContactContextMenu(Account* account, QWidget* parent = 0);
	~ContactContextMenu();

	void setContact(Contact* c);
private Q_SLOTS:
	void showContactInfo();
	void removeContact();
	void ignoreContact();
	void renameContact();
	void alwaysVisibleTriggered(bool checked);
	void alwaysInvisibleTriggered(bool checked);
	void askAuthorization();
	void checkOnlineStatus(OnlineStatus status);
	void showHistory();
	void slotChangeGroup(quint32 groupId);
	void slotGroupAdded(ContactGroup* group);
	void slotGroupRenamed(ContactGroup* group);
	void slotGroupRemoved(ContactGroup* group);
	void slotGroupsCleared();

private:
	void setVisibility();
	
	QAction* showContactInfoAction;
	QAction* removeContactAction;
	QAction* ignoreContactAction;
	QAction* renameContactAction;
	QAction* alwaysVisibleAction;
	QAction* alwaysInvisibleAction;
	QAction* askAuthorizationAction;
	QAction* historyAction;
	QAction* moveToGroupAction;
	SubmenuMoveToGroup* moveToGroup;
	Account* m_account;
	Contact* m_contact;
};

#endif
