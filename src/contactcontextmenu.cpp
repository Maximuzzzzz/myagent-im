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
#include "removecontactdialog.h"
#include "resourcemanager.h"

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

	ignoreContactAction = new QAction(QIcon(":icons/cl_ignore_contact.png"), tr("Ignore"), this);
	ignoreContactAction->setCheckable(true);
	connect(ignoreContactAction, SIGNAL(triggered()), this, SLOT(ignoreContact()));

	historyAction = new QAction(QIcon(":icons/history.png"), tr("History"), this);
	connect(historyAction, SIGNAL(triggered()), this, SLOT(showHistory()));

	connect(m_account->contactList(), SIGNAL(groupAdded(ContactGroup*)), this, SLOT(slotGroupAdded(ContactGroup*)));
	connect(m_account->contactList(), SIGNAL(groupRemoved(ContactGroup*)), this, SLOT(slotGroupRemoved(ContactGroup*)));
	connect(m_account->contactList(), SIGNAL(groupsCleared()), this, SLOT(slotGroupsCleared()));

	moveToGroup = new SubmenuMoveToGroup(this);
	connect(moveToGroup, SIGNAL(moveContact(quint32)),this, SLOT(slotChangeGroup(quint32)));

	addAction(showContactInfoAction);
	addAction(askAuthorizationAction);
	addAction(renameContactAction);
	addAction(historyAction);
	moveToGroupAction = addMenu(moveToGroup);
	addSeparator();
	addAction(alwaysVisibleAction);
	addAction(alwaysInvisibleAction);
	addSeparator();
	addAction(removeContactAction);
	addAction(ignoreContactAction);
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

	RemoveContactDialog* rd = new RemoveContactDialog(tr("Remove contact"), tr("Are you sure you want to remove contact %1?").arg(contactName));
	if (rd->exec() == QDialog::Accepted)
	//if (CenteredMessageBox::question(tr("Remove contact"), tr("Are you sure you want to remove contact %1?").arg(contactName), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		qDebug() << "real removing";
		m_account->contactList()->removeContactOnServer(m_contact);
		if (rd->removeHistory())
			theRM.removeFolder(m_contact->path());
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

	bool ignored = (m_contact->flags() & CONTACT_FLAG_IGNORE) != 0;
	ignoreContactAction->setChecked(ignored);

	bool notPhone = !m_contact->isPhone();
	askAuthorizationAction->setVisible(notPhone);
	alwaysVisibleAction->setVisible(notPhone);
	alwaysInvisibleAction->setVisible(notPhone);
	historyAction->setVisible(notPhone);
	ignoreContactAction->setVisible(notPhone);

	bool notAuthorized = m_contact->isNotAuthorized();
	moveToGroupAction->setVisible(!notAuthorized);
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
	ignoreContactAction->setEnabled(connected);
	renameContactAction->setEnabled(connected);
	showContactInfoAction->setEnabled(connected);
	moveToGroupAction->setEnabled(connected);
	
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
	InputLineDialog dialog(tr("Rename contact"), QIcon(":icons/cl_rename_contact.png"), tr("Enter new name for user %1:").arg(m_contact->nickname() + "(" + m_contact->email() + ")"), ".+", m_contact->nickname());
	
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

void ContactContextMenu::slotChangeGroup(quint32 groupId)
{
	qDebug() << "ContactContextMenu::slotChangeGroup" << groupId;
	m_contact->changeGroup(groupId);
}

void ContactContextMenu::slotGroupAdded(ContactGroup* group)
{
	qDebug() << "ContactContextMenu::slotGroupAdded";

	moveToGroup->addGroup(group->id(), group->name());
}

void ContactContextMenu::slotGroupRemoved(ContactGroup* group)
{
	qDebug() << "ContactContextMenu::slotGroupRemoved";

	moveToGroup->removeGroup(group->id());
}

void ContactContextMenu::slotGroupsCleared()
{
	qDebug() << "ContactContextMenu::slotGroupsCleared()";

	moveToGroup->clearAll();
}

void ContactContextMenu::ignoreContact()
{
	if (!m_contact->isIgnored())
		if (QMessageBox::question(this, tr("Ignoring contact"), tr("Do you really want to ignore this contact?"),
		 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
			return;

	ignoreContactAction->setChecked(!m_contact->isIgnored());
	m_account->contactList()->ignoreContactOnServer(m_contact, !m_contact->isIgnored());
}

//-------------------------------------------------------------

SubmenuMoveToGroup::SubmenuMoveToGroup(QWidget* parent)
	: QMenu(parent)
{
	qDebug() << "SubmenuMoveToGroup::SubmenuMoveToGroup";
	setTitle(tr("Move to group"));
}

SubmenuMoveToGroup::~SubmenuMoveToGroup()
{
	qDebug() << "SubmenuMoveToGroup::~SubmenuMoveToGroup()";
}

void SubmenuMoveToGroup::addGroup(quint32 groupId, QString groupName)
{
	qDebug() << "SubmenuMoveToGroup::addGroup" << groupId << groupName;
	QAction* newAction = new QAction(QIcon(":icons/cl_add_group.png"), groupName, this);
	addAction(newAction);
	m_groups.insert(groupId, newAction);
	connect(newAction, SIGNAL(triggered()), this, SLOT(moveContactTo()));
}

void SubmenuMoveToGroup::removeGroup(quint32 groupId)
{
	qDebug() << "SubmenuMoveToGroup::removeGroup" << groupId;
	QAction* actionForRemove = m_groups.value(groupId);
	disconnect(actionForRemove, SIGNAL(triggered()), this, SLOT(moveContactTo()));
	m_groups.remove(groupId);
	delete actionForRemove;
}

void SubmenuMoveToGroup::clearAll()
{
	qDebug() << "SubmenuMoveToGroup::clearAll()";
	QHash<quint32, QAction*>::iterator it = m_groups.begin();
	while (it != m_groups.end())
	{
		qDebug() << "deleting" << *it;
		disconnect((*it), SIGNAL(triggered()), this, SLOT(moveContactTo()));
		delete *it;
		m_groups.erase(it);
		it = m_groups.begin();
	}
}

void SubmenuMoveToGroup::moveContactTo()
{
	qDebug() << "SubmenuMoveToGroup::moveContactTo()";
	quint32 groupId = m_groups.key(qobject_cast<QAction*>(sender()));
	emit moveContact(groupId);
}
