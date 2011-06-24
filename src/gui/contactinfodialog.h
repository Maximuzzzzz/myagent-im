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

#ifndef CONTACTINFODIALOG_H
#define CONTACTINFODIALOG_H

#include <QWidget>
#include <QList>

class QLabel;
class QPushButton;
class QLineEdit;
class QDialogButtonBox;

class ContactInfo;
class Account;

class ContactInfoDialog : public QWidget
{
Q_OBJECT
public:
	static ContactInfoDialog* create(Account* account, const ContactInfo& info);
	static ContactInfoDialog* create(Account* account, const QByteArray& email);

	QStringList editablePhones() const;

public slots:
	void show();

private slots:
	void showContactInfo(quint32 status, bool timeout);
	void checkPhones();
	void savePhones();
	
private:
	ContactInfoDialog(Account* account, const ContactInfo& info);
	ContactInfoDialog(Account* account, const QByteArray& email);
	~ContactInfoDialog();
	static QList<ContactInfoDialog*> windows;

	void initWindow();

	static ContactInfoDialog* searchWindow(const QByteArray& email);

	void createContent(const ContactInfo& info);
private:
	QIcon linkIcon(const QString& linkType);

	Account* m_account;
	QByteArray m_email;
	QLabel* requestLabel;
	QPushButton* cancelButton;

	QLineEdit* phoneEdit1;
	QLineEdit* phoneEdit2;
	QLineEdit* phoneEdit3;
	QDialogButtonBox* buttonBox;
};

#endif
