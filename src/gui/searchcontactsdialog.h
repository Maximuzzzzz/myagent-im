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

#ifndef SEARCHCONTACTSDIALOG_H
#define SEARCHCONTACTSDIALOG_H

#include <QDialog>
#include <ui_searchcontacts.h>

#include "mrim/mrimclient.h"

class SearchContactsForm : public QDialog, private Ui::SearchContactsForm
{
Q_OBJECT
public:
	SearchContactsForm();
	~SearchContactsForm();

	bool isEmail() const { return emailGroupBox->isChecked(); }
	QByteArray email() const { return emailEdit->text().toLatin1(); }
	MRIMClient::SearchParams info() const;

Q_SIGNALS:
	void searchButtonClicked();

private Q_SLOTS:
	void checkCountryBox(int index);
	void checkBirthMonth(int index);
	void checkBirthDay(int index);
	
private Q_SLOTS:
	void checkDate(bool b);
private:
	void showWarning(bool condition, const QString& msg);
	void warnWrongDate(bool b);
	
	static int days[12];
};

#endif
