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

#include "contactcontextmenu.h"

#include <QDebug>

#include <QApplication>

#include "proto.h"
#include "contactinfodialog.h"
#include "editphonesdialog.h"
#include "account.h"
#include "contact.h"
#include "contactlist.h"
#include "historyviewer.h"
#include "ui_askauthorization.h"
#include "inputlinedialog.h"
#include "centeredmessagebox.h"

ContactContextMenu::ContactContextMenu(Account* account, QWidget* parent)
	: QMenu(parent), m_account(account), m_contact(0)
{
	connect(m_account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(checkOnlineStatus(OnlineStatus)));
	
	showContactInfoAction = new QAction(QIcon(":icons/anketa.png"), tr("Info"), this);
	connect(showContactInfoAction, SIGNAL(triggered()), this, SLOT(showContactInfo()));
	
	removeContactAction = new QAction(QIcon(":icons/cl_delete_contact.png"), tr("Remove"), this);
	connect(removeContactAction, SIGNAL(triggered()), this, SLOT(removeContact()));
	
	renameContactAction = new QAction(QIcon(":icons/cl_rename_contact.png"), tr("Rename"), this);
	connect(renameContactAction, SIGNAL(triggered()), this, SLOT(renameContact()));
	
	alwaysVisibleAction = new QAction(tr("I'm always visible for..."), this);
	alwaysVisibleAction->setCheckable(true);
	connect(alwaysVisibleAction, SIGNAL(triggered(bool)), this, SLOT(alwaysVisibleTriggered(bool)));
	
	alwaysInvisibleAction = new QAction(tr("I'm always invisible for..."), this);
	alwaysInvisibleAction->setCheckable(true);
	connect(alwaysInvisibleAction, SIGNAL(triggered(bool)), this, SLOT(alwaysInvisibleTriggered(bool)));
	
	askAuthorizationAction = new QAction(QIcon(":icons/authorize_request.png"), tr("Ask authorization"), this);
	connect(askAuthorizationAction, SIGNAL(triggered()), this, SLOT(askAuthorization()));

	historyAction = new QAction(QIcon(":icons/history.png"), tr("History"), this);
	connect(historyAction, SIGNAL(triggered()), this, SLOT(showHistory()));
	
	addAction(showContactInfoAction);
	addAction(askAuthorizationAction);
	addAction(renameContactAction);
	addAction(historyAction);
	addSeparator();
	addAction(alwaysVisibleAction);
	addAction(alwaysInvisibleAction);
	addSeparator();
	addAction(removeContactAction);
}

ContactContextMenu::~ContactContextMenu()
{
}

void ContactContextMenu::showContactInfo()
{
	if (m_contact->isPhone())
	{
		EditPhonesDialog dlg(m_contact->nickname(), m_contact->phones());
		
		if (dlg.exec() == QDialog::Accepted)
		{
			m_contact->changePhones(dlg.phones());
		}
	}
	else
		ContactInfoDialog::create(m_account, m_contact->email());
}

void ContactContextMenu::removeContact()
{
	qDebug() << "ContactContextMenu::removeContact()";

	QString contactName;
	if (m_contact->isPhone())
		contactName = m_contact->nickname();
	else
		contactName = m_contact->email();

	if (CenteredMessageBox::question(tr("Remove contact"), tr("Are you sure you want to remove contact") + " " + contactName + "?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok)
	{
		qDebug() << "real removing";
		m_account->contactList()->removeContactOnServer(m_contact);
	}
}

void ContactContextMenu::alwaysVisibleTriggered(bool checked)
{
	if (checked && alwaysInvisibleAction->isChecked())
		alwaysInvisibleAction->setChecked(false);
	
	setVisibility();
}

void ContactContextMenu::alwaysInvisibleTriggered(bool checked)
{
	if (checked && alwaysVisibleAction->isChecked())
		alwaysVisibleAction->setChecked(false);
	
	setVisibility();
}

void ContactContextMenu::setVisibility()
{
	bool alwaysVisible = alwaysVisibleAction->isChecked();
	bool alwaysInvisible = alwaysInvisibleAction->isChecked();
	
	m_contact->setMyVisibility(alwaysVisible, alwaysInvisible);
}

void ContactContextMenu::setContact(Contact* c)
{
	m_contact = c;
	
	checkOnlineStatus(m_account->onlineStatus());
	
	bool visible = (m_contact->flags() & CONTACT_FLAG_VISIBLE) != 0;
	alwaysVisibleAction->setChecked(visible);
	
	bool invisible = (m_contact->flags() & CONTACT_FLAG_INVISIBLE) != 0;
	alwaysInvisibleAction->setChecked(invisible);

	bool notPhone = !m_contact->isPhone();
	askAuthorizationAction->setVisible(notPhone);
	alwaysVisibleAction->setVisible(notPhone);
	alwaysInvisibleAction->setVisible(notPhone);
	historyAction->setVisible(notPhone);
}

void ContactContextMenu::askAuthorization()
{
	qDebug() << "askAuthorization";
	
	QDialog dialog(this);
	Ui::AskAuthorization ui;
	ui.setupUi(&dialog);
	ui.emailLabel->setText(m_contact->email());
	dialog.setFixedSize(dialog.sizeHint());
	
	connect(ui.infoButton, SIGNAL(clicked(bool)), this, SLOT(showContactInfo()));
	
	if (dialog.exec() == QDialog::Accepted)
		m_account->client()->askAuthorization(m_contact->email(), ui.messageEdit->toPlainText());
}

void ContactContextMenu::checkOnlineStatus(OnlineStatus status)
{
	if (!m_contact)
		return;

	bool connected = status.connected();

	alwaysVisibleAction->setEnabled(connected);
	alwaysInvisibleAction->setEnabled(connected);
	removeContactAction->setEnabled(connected);
	renameContactAction->setEnabled(connected);
	showContactInfoAction->setEnabled(connected);
	
	if (m_contact->status() == OnlineStatus::unauthorized)
		askAuthorizationAction->setEnabled(connected);
	else
		askAuthorizationAction->setDisabled(true);

	if (m_contact->isTemporary())
	{
		alwaysVisibleAction->setEnabled(false);
		alwaysInvisibleAction->setEnabled(false);
		askAuthorizationAction->setEnabled(false);
	}
}

void ContactContextMenu::renameContact()
{
	InputLineDialog dialog(tr("Rename contact"), QIcon(":icons/cl_rename_contact.png"), tr("Enter new name for user %1:").arg(m_contact->nickname() + "(" + m_contact->email() + ")"), ".+");
	
	if (dialog.exec() == QDialog::Accepted)
	{
		QString nickname = dialog.text();
		m_contact->rename(nickname);
	}
}

void ContactContextMenu::showHistory()
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
