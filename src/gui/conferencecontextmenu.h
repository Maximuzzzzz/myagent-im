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

#ifndef CONFERENCECONTEXTMENU_H
#define CONFERENCECONTEXTMENU_H

#include <QMenu>

#include "onlinestatus.h"

class QAction;
class Account;
class Contact;

class ConferenceContextMenu : public QMenu
{
Q_OBJECT
public:
	explicit ConferenceContextMenu(Account* account, QWidget* parent = 0);
	~ConferenceContextMenu();

	void setContact(Contact* c);
private Q_SLOTS:
	void exitConference();
//	void renameContact();
	void showHistory();
	void checkOnlineStatus(OnlineStatus status);

private:
	QAction* exitConferenceAction;
//	QAction* renameContactAction;
	QAction* historyAction;
	Account* m_account;
	Contact* m_contact;
};

#endif
