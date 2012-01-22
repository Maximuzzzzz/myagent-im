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

#include "editphonesdialog.h"

#include <QDebug>

#include <QRegExpValidator>

#include "gui/centerwindow.h"

EditPhonesDialog::EditPhonesDialog(const QString& nickname, const QStringList& phones, QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);
	setFixedSize(sizeHint());

	usernameEdit->setText(nickname);
	if (nickname.isEmpty())
	{
		usernameEdit->setEnabled(true);
		saveButton->setText(tr("Add"));
	}
	else
		usernameEdit->setDisabled(true);

	phoneEdit1->setText(phones.value(0));
	phoneEdit2->setText(phones.value(1));
	phoneEdit3->setText(phones.value(2));

	usernameEdit->setValidator(new QRegExpValidator(QRegExp(".+"), phoneEdit1));
	phoneEdit1->setValidator(new QRegExpValidator(QRegExp("\\+(\\d){11,12}"), phoneEdit1));
	phoneEdit2->setValidator(new QRegExpValidator(QRegExp("|\\+(\\d){11,12}"), phoneEdit2));
	phoneEdit3->setValidator(new QRegExpValidator(QRegExp("|\\+(\\d){11,12}"), phoneEdit3));

	connect(usernameEdit, SIGNAL(textEdited(const QString&)), SLOT(checkData()));
	connect(phoneEdit1, SIGNAL(textEdited(const QString&)), SLOT(checkData()));
	connect(phoneEdit2, SIGNAL(textEdited(const QString&)), SLOT(checkData()));
	connect(phoneEdit3, SIGNAL(textEdited(const QString&)), SLOT(checkData()));

	centerWindow(this);
}

EditPhonesDialog::~EditPhonesDialog()
{
}

void EditPhonesDialog::checkData()
{
	qDebug() << "EditPhonesDialog::checkData";

	if (usernameEdit->text().isEmpty())
	{
		saveButton->setDisabled(true);
		return;
	}

	if (!phoneEdit1->hasAcceptableInput())
	{
		saveButton->setDisabled(true);
		return;
	}

	if (phoneEdit2->text().isEmpty() && !phoneEdit3->text().isEmpty())
	{
		saveButton->setDisabled(true);
		return;
	}

	if (!phoneEdit2->hasAcceptableInput() || !phoneEdit3->hasAcceptableInput())
	{
		saveButton->setDisabled(true);
		return;
	}

	saveButton->setEnabled(true);
}

QStringList EditPhonesDialog::phones() const
{
	QStringList result;
	QString phone;

	phone = phoneEdit1->text();
	if (!phone.isEmpty())
		result.append(phone);

	phone = phoneEdit2->text();
	if (!phone.isEmpty())
		result.append(phone);

	phone = phoneEdit3->text();
	if (!phone.isEmpty())
		result.append(phone);

	return result;
}

QString EditPhonesDialog::nickname() const
{
	return usernameEdit->text();
}
