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

#include "conferencecontextmenu.h"

#include <QDebug>

#include <QApplication>

#include "account.h"
#include "contact.h"
#include "historyviewer.h"
#include "gui/centeredmessagebox.h"

ConferenceContextMenu::ConferenceContextMenu(Account* account, QWidget* parent)
	: QMenu(parent), m_account(account), m_contact(0)
{
	connect(m_account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(checkOnlineStatus(OnlineStatus)));

	exitConferenceAction = new QAction(QIcon(":icons/cl_delete_contact.png"), tr("Exit conference"), this);
	connect(exitConferenceAction, SIGNAL(triggered()), this, SLOT(exitConference()));

/*	renameContactAction = new QAction(QIcon(":icons/cl_rename_contact.png"), tr("Rename"), this);
	connect(renameContactAction, SIGNAL(triggered()), this, SLOT(renameContact()));*/

	historyAction = new QAction(QIcon(":icons/history.png"), tr("History"), this);
	connect(historyAction, SIGNAL(triggered()), this, SLOT(showHistory()));

//	addAction(renameContactAction);
	addAction(historyAction);
	addSeparator();
	addAction(exitConferenceAction);
}

ConferenceContextMenu::~ConferenceContextMenu()
{
}

void ConferenceContextMenu::exitConference()
{
	qDebug() << "ConferenceContextMenu::exitConference()";

	QString confName = m_contact->nickname();

	if (CenteredMessageBox::question(tr("Exit conference"), tr("Are you sure you want to exit conference '%1?'").arg(confName), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok)
	{
		qDebug() << "real removing";
		m_account->contactList()->removeContactOnServer(m_contact);
	}
}

void ConferenceContextMenu::setContact(Contact* c)
{
	m_contact = c;
	
	checkOnlineStatus(m_account->onlineStatus());
}

/*void ConferenceContextMenu::renameContact()
{
	InputLineDialog dialog(tr("Rename contact"), QIcon(":icons/cl_rename_contact.png"), tr("Enter new name for user %1:").arg(m_contact->nickname() + "(" + m_contact->email() + ")"), ".+");
	
	if (dialog.exec() == QDialog::Accepted)
	{
		QString nickname = dialog.text();
		m_contact->rename(nickname);
	}
}*/

void ConferenceContextMenu::showHistory()
{
	QString viewDescription = "HistoryView:" + m_contact->email();
	
	foreach (QWidget* widget, QApplication::topLevelWidgets())
	{
		if (widget->objectName() == viewDescription)
		{
			widget->raise();
			widget->activateWindow();
			return;
		}
	}
	
	HistoryViewer* historyViewer = new HistoryViewer(m_contact);
	historyViewer->setObjectName(viewDescription);
	historyViewer->setAttribute(Qt::WA_DeleteOnClose);
	historyViewer->show();
}

void ConferenceContextMenu::checkOnlineStatus(OnlineStatus status)
{
	if (!m_contact)
		return;

	bool connected = status.connected();

	exitConferenceAction->setEnabled(connected);
//	renameContactAction->setEnabled(connected);
}
