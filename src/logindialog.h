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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "ui_logindialog.h"
#include "onlinestatus.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
Q_OBJECT
public:
	LoginDialog(QWidget* parent = 0);
	OnlineStatus status() const;
	QByteArray email() const { return emailBox->currentText().toLatin1(); }
	QByteArray password() const { return passwordEdit->text().toLatin1(); }
	void setEmail(const QString& email);

private slots:
	void checkEmail();
	void checkPassword();
	void checkOnlineStatus();

private:
	bool extendedStatus;
	QByteArray statusId;
	QString statusDescr;

	QList<QByteArray> statusList;
};

#endif
