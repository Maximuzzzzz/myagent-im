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

#include "onlinestatus.h"
#include "resourcemanager.h"
#include "proto.h"
#include "centerwindow.h"

LoginDialog::LoginDialog(QWidget* parent)
 : QDialog(parent)
{
	extendedStatus = false;

	setWindowFlags(Qt::Dialog & !Qt::WindowContextHelpButtonHint);
	setupUi(this);

	currPass = "";
	isSavePass = false;

	QDir dir(theRM.basePath());
	QStringList filters;
	filters << "?*@mail.ru" << "?*@list.ru" << "?*inbox.ru" << "?*bk.ru" << "?*corp.mail.ru";
	emailBox->addItems(dir.entryList(filters, QDir::Dirs | QDir::CaseSensitive));
	emailBox->lineEdit()->clear();
	emailBox->lineEdit()->completer()->setCompletionMode(QCompleter::PopupCompletion);

	emailBox->lineEdit()->setValidator(new QRegExpValidator(QRegExp("([a-z]|[A-Z]|[0-9]|[_])+([a-z]|[A-Z]|[0-9]|[_\\-\\.])*@(mail.ru|list.ru|inbox.ru|bk.ru|corp.mail.ru)"), emailBox));
	passwordEdit->setValidator(new QRegExpValidator(QRegExp(".+"), passwordEdit));

	OnlineStatus onlineStatus;
	QList<QByteArray> statuses;
	statuses << "status_1" << "status_dating" << "status_chat" << "status_2" << "status_3" << "status_dnd";
	QList<QByteArray>::const_iterator it;
	for (it = statuses.begin(); it != statuses.end(); ++it)
	{
		QIcon currStatusIcon;
		onlineStatus.setIdStatus(*it);
		if (theRM.onlineStatuses()->getOnlineStatusInfo(onlineStatus.id())->available() == "1")
		{
			currStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo(onlineStatus.id())->icon(), QSize(), QIcon::Normal, QIcon::Off);
			onlineStatusBox->addItem(currStatusIcon, onlineStatus.statusDescr());
			statusList << (*it);
		}
	}

	connect(emailBox->lineEdit(), SIGNAL(textEdited(const QString&)), SLOT(checkEmail()));
	connect(emailBox, SIGNAL(editTextChanged(const QString&)), SLOT(slotEmailChanged()));
	connect(passwordEdit, SIGNAL(textEdited(const QString&)), SLOT(checkPassword()));

	okButton->setDisabled(true);

	setFixedSize(sizeHint());
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(settingsButton, SIGNAL(clicked()), this, SLOT(showSettingsWindow()));
	connect(savePass, SIGNAL(clicked()), this, SLOT(slotSavePassChecked()));

	centerWindow(this);
}

OnlineStatus LoginDialog::status() const
{
	if (onlineStatusBox->currentIndex() >= statusList.size())
		return OnlineStatus(statusId, statusDescr);
	else
	{
		OnlineStatus res;
		res.setIdStatus(statusList.at(onlineStatusBox->currentIndex()));
		return res;
	}
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
	currPass = passwordEdit->text().toLatin1();
}

void LoginDialog::slotEmailChanged()
{
	OnlineStatus st = theRM.loadOnlineStatus(email());
	QByteArray pass = theRM.loadPass(email());
	if (!pass.isEmpty())
	{
		savePass->setChecked(true);
		passwordEdit->setText(pass);
		okButton->setEnabled(passwordEdit->hasAcceptableInput());
	}
	else
	{
		passwordEdit->setText(currPass);
		okButton->setEnabled(passwordEdit->hasAcceptableInput());
		savePass->setChecked(isSavePass);
	}
	if (st.id() == "" || st == OnlineStatus::wrongData || st == OnlineStatus::unknown || st == OnlineStatus::offline || st == OnlineStatus::connecting)
	{
		onlineStatusBox->setCurrentIndex(0);
		return;
	}

	if (extendedStatus)
	{
		onlineStatusBox->removeItem(statusList.size() - 1);
		extendedStatus = false;
	}

	int i;
	for (i = 0; i < statusList.size(); i++)
	{
		OnlineStatus status;
		status.setIdStatus(statusList.at(i));
		if (st == status)
		{
			onlineStatusBox->setCurrentIndex(i);
			return;
		}
	}
	QIcon onlineStatusIcon;
	onlineStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo(st.id())->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(onlineStatusIcon, st.statusDescr());
	onlineStatusBox->setCurrentIndex(statusList.size());
	extendedStatus = true;
	statusId = st.id();
	statusDescr = st.statusDescr();
}

void LoginDialog::showSettingsWindow()
{
	if (settingsWindow)
		return;

	settingsWindow = new SettingsWindow(SHOW_CONNECTION_PAGE);
	centerWindow(settingsWindow);
	settingsWindow->show();
}

void LoginDialog::slotSavePassChecked()
{
	//TODO: Attention message needs
	isSavePass = savePass->isChecked();
}
