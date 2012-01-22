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

#ifndef STATUSMENU_H
#define STATUSMENU_H

#include <QMenu>

#include "onlinestatus.h"

class Account;

class StatusMenu : public QMenu
{
Q_OBJECT
public:
	explicit StatusMenu(Account* acc, QWidget* parent = 0);

	~StatusMenu();

public Q_SLOTS:
	void updateExtendedStatuses();

Q_SIGNALS:
	void showOnlineStatusesEditor();
	void statusChanged(OnlineStatus status, qint32 id);

private Q_SLOTS:
	QAction* createStatusAction(OnlineStatus status);
	void slotActionTriggered(QAction* action);

private:
	Account* m_account;
	QActionGroup* extendedActions;
	QAction* mainSeparator;
};

#endif
