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

#include "addcontactdialog.h"

#include <QDebug>

#include "gui/contactinfodialog.h"
#include "account.h"
#include "contactlist.h"
#include "gui/centerwindow.h"

QList<AddContactDialog*> AddContactDialog::windows = QList<AddContactDialog*>();

AddContactDialog::AddContactDialog(Account* account, const ContactInfo& info)
	: m_info(info), m_account(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	setFixedSize(sizeHint());

	ContactList* cl = m_account->contactList();	

	ContactList::GroupsIterator it = cl->groupsBegin();
	for (; it != cl->groupsEnd(); ++it)
	{
		groupComboBox->addItem((*it)->name());
	}

	emailLabel->setText(info.email());
	if (!info.nickname().isEmpty())
		nicknameEdit->setText(info.nickname());
	else
		nicknameEdit->setText(info.email());
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

	connect(infoButton, SIGNAL(clicked(bool)), this, SLOT(showContactInfo()));
}

AddContactDialog::~AddContactDialog()
{
	qDebug() << "AddContactDialog::~AddContactDialog()";
	windows.removeAll(this);
}

AddContactDialog* AddContactDialog::create(Account * account, const ContactInfo & info, QObject* obj, const char* method)
{
	int nWindows = windows.size();
	
	for (int i = 0; i < nWindows; i++)
	{
		AddContactDialog* wnd = windows.at(i);
		if (wnd->email() == info.email())
		{
			wnd->raise();
			wnd->activateWindow();
			return wnd;
		}
	}

	AddContactDialog* wnd = new AddContactDialog(account, info);
	connect(wnd, SIGNAL(accepted()), obj, method);
	windows.append(wnd);
	centerWindow(wnd);
	wnd->show();
	return wnd;
}

void AddContactDialog::showContactInfo()
{
	qDebug() << "AddContactDialog::showContactInfo";
	ContactInfoDialog::create(m_account, m_info);
}

void AddContactDialog::accept()
{
	Q_EMIT accepted();
	close();
}
