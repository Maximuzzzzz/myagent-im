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

#include "authorizedialog.h"

#include <QDebug>

#include "contactinfodialog.h"
#include "contactgroup.h"
#include "account.h"
#include "contactlist.h"
#include "contact.h"
#include "gui/centerwindow.h"

AuthorizeDialog::AuthorizeDialog(Account* account, const QByteArray& email, const QString& nickname, const QString& message)
	: m_account(account), m_email(email)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	
	label->setText(QString(tr("User %1 (%2) asks authorization:")).arg(email.constData()).arg(nickname));
	authTextView->setPlainText(message);

	ContactList* cl = account->contactList();

	ContactList::GroupsIterator it = cl->groupsBegin();
	for (; it != cl->groupsEnd(); ++it)
	{
		groupBox->addItem((*it)->name());
	}

	if (cl->getContact(email)->group())
	{
		qDebug() << cl->getContact(email)->group()->id() << groupBox->count();
		groupBox->setCurrentIndex(cl->getContact(email)->group()->id());
	}

	connect(okButton, SIGNAL(clicked()), this, SLOT(slotOkClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(infoButton, SIGNAL(clicked(bool)), this, SLOT(showContactInfo()));

	setFixedSize(sizeHint());

	centerWindow(this);
}

AuthorizeDialog::~AuthorizeDialog()
{
	qDebug() << "AuthorizeDialog::~AuthorizeDialog() email " << m_email;
}

void AuthorizeDialog::showContactInfo()
{
	qDebug() << "AuthorizeDialog::showContactInfo()";
	ContactInfoDialog::create(m_account, m_email);
}

bool AuthorizeDialog::addContact() const
{
	return addContactCheckBox->isChecked();
}

int AuthorizeDialog::group() const
{
	return groupBox->currentIndex();
}

void AuthorizeDialog::slotOkClicked()
{
	emit accepted();
	close();
}
