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

class QAction;
class Account;
class Contact;

class ContactContextMenu : public QMenu
{
Q_OBJECT
public:
	ContactContextMenu(Account* account, QWidget* parent = 0);
	~ContactContextMenu();

	void setContact(Contact* c);
private slots:
	void showContactInfo();
	void removeContact();
	void renameContact();
	void alwaysVisibleTriggered(bool checked);
	void alwaysInvisibleTriggered(bool checked);
	void askAuthorization();
	void checkOnlineStatus(OnlineStatus status);
	void showHistory();
private:
	void setVisibility();
	
	QAction* showContactInfoAction;
	QAction* removeContactAction;
	QAction* renameContactAction;
	QAction* alwaysVisibleAction;
	QAction* alwaysInvisibleAction;
	QAction* askAuthorizationAction;
	QAction* historyAction;
	Account* m_account;
	Contact* m_contact;
};

#endif
