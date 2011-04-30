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

#include "mainmenubutton.h"

#include <QDebug>
#include <QMenu>
#include <QApplication>
#include <QTreeWidget>

#include "searchcontactsdialog.h"
#include "tasksearchcontacts.h"
#include "taskaddcontact.h"
#include "proto.h"
#include "contactinfo.h"
#include "contactinfolistwindow.h"
#include "account.h"
#include "contact.h"
#include "addcontactdialog.h"
#include "contactlist.h"
#include "contactlistwindow.h"
#include "editphonesdialog.h"
#include "inputlinedialog.h"
#include "centerwindow.h"
#include "centeredmessagebox.h"
#include "aboutdialog.h"

class MRIMClient;

MainMenuButton::MainMenuButton(Account* account, ContactListWindow* w)
	: m_account(account), mainWindow(w), searchForm(0)
{
	setText(tr("Menu"));
	setFixedWidth(sizeHint().width());
	
	QMenu* menu = new QMenu(this);
	
	addContactAction = new QAction(QIcon(":icons/cl_add_contact.png"), tr("Add contact"), this);
	connect(addContactAction, SIGNAL(triggered(bool)), this, SLOT(searchContacts()));

	addSmsContactAction = new QAction(QIcon(":icons/cl_add_phone_contact.png"), tr("Add contact for SMS"), this);
	connect(addSmsContactAction, SIGNAL(triggered(bool)), this, SLOT(addSmsContact()));
	connect(m_account->contactList(), SIGNAL(addSmsContactOnServerError(QString)), SLOT(addSmsContactError(QString)));

	addGroupAction = new QAction(QIcon(":icons/cl_add_group.png"), tr("Add group"), this);
	connect(addGroupAction, SIGNAL(triggered(bool)), this, SLOT(addGroup()));
	connect(m_account->contactList(), SIGNAL(addGroupOnServerError(QString)), this, SLOT(addGroupError(QString)));

	newConferenceAction = new QAction(QIcon(":icons/cl_new_conference.png"), tr("New conference"), this);
	connect(newConferenceAction, SIGNAL(triggered(bool)), this, SLOT(createNewConference()));

	menu->addAction(addContactAction);
	menu->addAction(addSmsContactAction);
	menu->addAction(addGroupAction);

	menu->addSeparator();
	menu->addAction(newConferenceAction);

	menu->addSeparator();
	menu->addAction(QIcon(":icons/settings.png"), tr("Settings"), this, SLOT(showSettingsWindow()));

	menu->addSeparator();
	menu->addAction(QIcon(":icons/about.png"), tr("About"), this, SLOT(showAboutDialog()));

	menu->addSeparator();
	menu->addAction(QIcon(":icons/exit.png"), tr("Quit"), qApp, SLOT(quit()));

	setMenu(menu);
	
	checkOnlineStatus(m_account->onlineStatus());
	connect(m_account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(checkOnlineStatus(OnlineStatus)));
}

void MainMenuButton::searchContacts()
{
	if (searchForm)
	{
		searchForm->show();
		centerWindow(searchForm);
		return;
	}
	
	searchForm = new SearchContactsForm;
	connect(searchForm, SIGNAL(searchButtonClicked()), this, SLOT(beginSearch()));
	searchForm->show();
}

void MainMenuButton::beginSearch()
{
	if (searchForm.isNull())
	{
		qDebug() << "MainMenuButton::beginSearch searchForm is null!";
		return;
	}

	searchForm->setDisabled(true);
	searchForm->setCursor(Qt::WaitCursor);

	MRIMClient* mrimClient = m_account->client();
	Task* task;
		
	if (searchForm->isEmail())
		task = new Tasks::SearchContacts(searchForm->email(), mrimClient, this);
	else
		task = new Tasks::SearchContacts(searchForm->info(), mrimClient, this);
	
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(showSearchResults(quint32, bool)));

	task->exec();
}

void MainMenuButton::showSearchResults(quint32 status, bool timeout)
{
	searchForm->setEnabled(true);
	searchForm->setCursor(Qt::ArrowCursor);

	if (timeout)
	{
		CenteredMessageBox::warning(tr("Search results"), tr("Time is out"));
		return;
	}
	else if (status != MRIM_ANKETA_INFO_STATUS_OK)
	{
		CenteredMessageBox::warning(tr("Search results"), ContactInfo::errorDescription(status));
		return;
	}
	
	searchForm->hide();

	Tasks::SearchContacts* task = qobject_cast<Tasks::SearchContacts*>(sender());
	
	ContactInfoListWindow* lw = new ContactInfoListWindow(m_account);
	lw->setInfo(task->getContactsInfo(), task->getMaxRows());
	lw->resize(lw->sizeHint());
	connect(lw, SIGNAL(addButtonClicked(const ContactInfo&)), SLOT(showAddContactDialog(const ContactInfo&)));
	connect(lw, SIGNAL(moreContactsButtonClicked()), SLOT(searchMoreContacts()));
	connect(lw, SIGNAL(newSearchButtonClicked()), SLOT(newSearch()));	

	lw->show();
}

void MainMenuButton::showAddContactDialog(const ContactInfo& info)
{
	Contact* contact = m_account->contactList()->findContact(info.email().toLatin1());
	if (contact && !contact->isHidden())
	{
		CenteredMessageBox::warning(tr("Add contact"), tr("Contact is already in contact list"));
		return;
	}
	
	AddContactDialog::create(m_account, info, this, SLOT(showAddContactDialogEnd()));
}

void MainMenuButton::showAddContactDialogEnd()
{
	qDebug() << "add contact";
	
	AddContactDialog* dialog = qobject_cast<AddContactDialog*>(sender());
	
	QString authorizationMessage;
	if (dialog->askAuthorization())
		authorizationMessage = dialog->authorizationMessage();
	
	m_account->contactList()->addContactOnServer(dialog->group(), dialog->email().toLatin1(), dialog->nickname(), authorizationMessage);
}

void MainMenuButton::checkOnlineStatus(OnlineStatus status)
{
	bool connected = (status != OnlineStatus::offline && status != OnlineStatus::connecting);
	addContactAction->setEnabled(connected);
	addSmsContactAction->setEnabled(connected);
	addGroupAction->setEnabled(connected);
	newConferenceAction->setEnabled(connected);
//	ignoreUserAction->setEnabled(connected);
}

void MainMenuButton::searchMoreContacts()
{
	QWidget* lw = qobject_cast<QWidget*>(sender());
	lw->close();
	beginSearch();
}

void MainMenuButton::newSearch()
{
	QWidget* lw = qobject_cast<QWidget*>(sender());
	lw->close();
	searchForm->show();
}

void MainMenuButton::addSmsContact()
{
	EditPhonesDialog dlg;
	dlg.setWindowIcon(QIcon(":icons/cl_add_phone_contact.png"));
	dlg.setWindowTitle(tr("Add contact for SMS"));
	if (dlg.exec() == QDialog::Accepted)
	{
		m_account->contactList()->addSmsContactOnServer(dlg.nickname(), dlg.phones());
	}
}

void MainMenuButton::addGroup()
{
	InputLineDialog dlg(tr("Add group"), QIcon(":icons/cl_add_group.png"), tr("Enter group name:"), ".+", "");

	if (dlg.exec() == QDialog::Accepted)
	{
		m_account->contactList()->addGroupOnServer(dlg.text());
	}
}

void MainMenuButton::addGroupError(QString error)
{
	CenteredMessageBox::critical(tr("Add group error"), error);
}

void MainMenuButton::addSmsContactError(QString error)
{
	CenteredMessageBox::critical(tr("Add sms contact error"), error);
}

void MainMenuButton::showSettingsWindow()
{
	if (settingsWindow)
	{
		settingsWindow->raise();
		settingsWindow->activateWindow();
		return;
	}

	settingsWindow = new SettingsWindow(m_account, mainWindow);
	connect(settingsWindow, SIGNAL(statusesCountChanged()), this, SIGNAL(statusesCountChanged()));
	connect(settingsWindow, SIGNAL(newNotificationType()), this, SIGNAL(notificationTypeChanged()));
	settingsWindow->setChatWindowsManager(chatWindowsManager);
	centerWindow(settingsWindow);
	settingsWindow->show();
}

void MainMenuButton::showAboutDialog()
{
	AboutDialog* aboutDialog = new AboutDialog();
	centerWindow(aboutDialog);
	aboutDialog->show();
}

void MainMenuButton::setChatWindowsManager(ChatWindowsManager* cwm)
{
	chatWindowsManager = cwm;
}

void MainMenuButton::createNewConference()
{
	if (newConferenceWindow)
	{
		newConferenceWindow->show();
		centerWindow(newConferenceWindow);
		return;
	}
	
	newConferenceWindow = new NewConferenceDialog(m_account);
	centerWindow(newConferenceWindow);
	connect(newConferenceWindow, SIGNAL(accepted(QString, QByteArray, QList<QByteArray>)), m_account->contactList(), SLOT(newConferenceOnServer(QString, QByteArray, QList<QByteArray>)));
	newConferenceWindow->show();
}
