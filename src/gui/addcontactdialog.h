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

#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QWidget>
#include <QList>

#include "ui_addcontact.h"
#include "core/contactinfo.h"

class Account;

class AddContactDialog : public QWidget, private Ui::AddContactDialog
{
Q_OBJECT
	AddContactDialog(Account* account, const ContactInfo& info);
	~AddContactDialog();
	static QList<AddContactDialog*> windows;

Q_SIGNALS:
	void accepted();

public:
	static AddContactDialog* create(Account* account, const ContactInfo& info, QObject* obj, const char* method);
	
	int group() const { return groupComboBox->currentIndex(); }
	QString nickname() const { return nicknameEdit->text(); }
	QString email() const { return m_info.email(); }
	
	bool askAuthorization() const { return authorizationCheckBox->isChecked(); }
	QString authorizationMessage() const { return authorizationMessageEdit->toPlainText(); }
	
private Q_SLOTS:
	void showContactInfo();
	void accept();
private:
	ContactInfo m_info;
	Account* m_account;
};

#endif
