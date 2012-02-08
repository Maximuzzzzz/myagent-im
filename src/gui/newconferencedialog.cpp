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

#include "newconferencedialog.h"

#include "core/account.h"
#include "core/contact.h"
#include "core/contactlist.h"

NewConferenceDialog::NewConferenceDialog(Account* account)
	: m_account(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

	allContactsLabel->setText(tr("All contacts (%1)").arg(QString(m_account->email())));

	connect(addMemberButton, SIGNAL(clicked(bool)), this, SLOT(addMember()));
	connect(removeMemberButton, SIGNAL(clicked(bool)), this, SLOT(removeMember()));
	connect(OKButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	QList<Contact*>::iterator it = m_account->contactList()->contactsBegin();
	while (it != m_account->contactList()->contactsEnd())
	{
		if (!((*it)->isHidden() || (*it)->isConference()))
			allContactsList->addItem((*it)->email());
		++it;
	}
}

NewConferenceDialog::~NewConferenceDialog()
{
}

void NewConferenceDialog::accept()
{
	if (conferenceName->text().isEmpty())
	{
		return;
	}
	if (membersList->count() == 0)
	{
		return;
	}

	QList<QByteArray> members;

	int i;
	for (i = 1; i <= membersList->count(); i++)
		members.append(membersList->item(i - 1)->text().toAscii());

	Q_EMIT accepted(conferenceName->text(), ((onlyMeRadioButton->isChecked()) ? (m_account->email()) : ("")), members);
	close();
}

void NewConferenceDialog::addMember()
{
	if (allContactsList->currentRow() > -1)
	{
		membersList->addItem(allContactsList->currentItem()->text());
		allContactsList->takeItem(allContactsList->currentRow());
	}
}

void NewConferenceDialog::removeMember()
{
	if (membersList->currentRow() > -1)
	{
		allContactsList->addItem(membersList->currentItem()->text());
		membersList->takeItem(membersList->currentRow());
	}
}
