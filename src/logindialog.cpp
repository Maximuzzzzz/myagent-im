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

#include "logindialog.h"

#include <QDir>
#include <QCompleter>

#include "resourcemanager.h"
#include "proto.h"
#include "centerwindow.h"

LoginDialog::LoginDialog(QWidget* parent)
 : QDialog(parent)
{
	setWindowFlags(Qt::Dialog & !Qt::WindowContextHelpButtonHint);
	setupUi(this);
	
	QDir dir(theRM.basePath());
	QStringList filters;
	filters << "?*@mail.ru" << "?*@list.ru" << "?*inbox.ru" << "?*bk.ru" << "?*corp.mail.ru";
	emailBox->addItems(dir.entryList(filters, QDir::Dirs | QDir::CaseSensitive));
	emailBox->lineEdit()->clear();
	emailBox->lineEdit()->completer()->setCompletionMode(QCompleter::PopupCompletion);

	emailBox->lineEdit()->setValidator(new QRegExpValidator(QRegExp("([a-z]|[A-Z]|[0-9])+([a-z]|[A-Z]|[0-9]|[_\\-\\.])*@(mail.ru|list.ru|inbox.ru|bk.ru|corp.mail.ru)"), emailBox));
	passwordEdit->setValidator(new QRegExpValidator(QRegExp(".+"), passwordEdit));

	connect(emailBox->lineEdit(), SIGNAL(textEdited(const QString&)), SLOT(checkEmail()));
	connect(passwordEdit, SIGNAL(textEdited(const QString&)), SLOT(checkPassword()));

	okButton->setDisabled(true);

	setFixedSize(sizeHint());
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	centerWindow(this);
}

OnlineStatus LoginDialog::status() const
{
	if (onlineStatusBox->currentText() == tr("Online"))
		return OnlineStatus::online;
	else if (onlineStatusBox->currentText() == tr("Away"))
		return OnlineStatus::away;
	else
		return OnlineStatus::invisible;
}

void LoginDialog::setEmail(const QString & email)
{
	emailBox->lineEdit()->setText(email);
	passwordEdit->setFocus(Qt::OtherFocusReason);
}

void LoginDialog::checkEmail()
{
	okButton->setEnabled(emailBox->lineEdit()->hasAcceptableInput());
}

void LoginDialog::checkPassword()
{
	okButton->setEnabled(passwordEdit->hasAcceptableInput());
}
