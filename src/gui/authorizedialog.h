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

#ifndef AUTHORIZEDIALOG_H
#define AUTHORIZEDIALOG_H

#include "ui_authorize.h"

class Account;

class AuthorizeDialog : public QWidget, private Ui::AuthorizeDialog
{
Q_OBJECT
public:
	AuthorizeDialog(Account* account, const QByteArray& email, const QString& nickname, const QString& message);
	~AuthorizeDialog();

	QByteArray email() const { return m_email; }
	bool addContact() const;
	int group() const;

Q_SIGNALS:
	void accepted();

private Q_SLOTS:
	void showContactInfo();
	void slotOkClicked();
private:
	Account* m_account;
	QByteArray m_email;
};

#endif
