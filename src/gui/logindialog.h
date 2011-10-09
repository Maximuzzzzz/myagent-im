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
#include <QWeakPointer>

#include "ui_logindialog.h"
#include "onlinestatus.h"
#include "gui/settingswindow.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
Q_OBJECT
public:
	LoginDialog(QWidget* parent = 0);

	OnlineStatus status() const;
	QByteArray email() const;
	void setEmail(const QString& email);
	QByteArray password() const { return passwordEdit->text().toLatin1(); }

	bool savePassword() const { return savePass->isChecked(); }

private Q_SLOTS:
	void checkEmail();
	void checkPassword();
	void slotEmailChanged();
	void showSettingsWindow();
	void slotSavePassChecked();

private:
	bool isExtendedStatus;
	bool isSavePassword;
	QByteArray currentPassword;

	QSize sizeWithoutHint;
	QSize sizeWithHint;

	QWeakPointer<SettingsWindow> settingsWindow;
};

#endif
