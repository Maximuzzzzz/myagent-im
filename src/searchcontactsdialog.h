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

#ifndef SEARCHCONTACTSDIALOG_H
#define SEARCHCONTACTSDIALOG_H

#include <QDialog>
#include <ui_searchcontacts.h>

#include "mrimclient.h"

class SearchContactsForm : public QDialog, private Ui::SearchContactsForm
{
Q_OBJECT
public:
	SearchContactsForm();
	~SearchContactsForm();

	bool isEmail() const { return emailGroupBox->isChecked(); }
	QByteArray email() const { return emailEdit->text().toLatin1(); }
	MRIMClient::SearchParams info() const;

signals:
	void searchButtonClicked();

private slots:
	void checkCountryBox(int index);
	void checkBirthMonth(int index);
	void checkBirthDay(int index);
	
private slots:
	void checkDate(bool b);
private:
	void showWarning(bool condition, const QString& msg);
	void warnWrongDate(bool b);
	
	static int days[12];
};

#endif
