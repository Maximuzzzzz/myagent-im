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
	
	QDir dir(theRM.basePath());
	QStringList filters;
	filters << "?*@mail.ru" << "?*@list.ru" << "?*inbox.ru" << "?*bk.ru" << "?*corp.mail.ru";
	emailBox->addItems(dir.entryList(filters, QDir::Dirs | QDir::CaseSensitive));
	emailBox->lineEdit()->clear();
	emailBox->lineEdit()->completer()->setCompletionMode(QCompleter::PopupCompletion);

	emailBox->lineEdit()->setValidator(new QRegExpValidator(QRegExp("([a-z]|[A-Z]|[0-9])+([a-z]|[A-Z]|[0-9]|[_\\-\\.])*@(mail.ru|list.ru|inbox.ru|bk.ru|corp.mail.ru)"), emailBox));
	passwordEdit->setValidator(new QRegExpValidator(QRegExp(".+"), passwordEdit));

	QIcon onlineStatusIcon;
	OnlineStatus onlineStatus;
	onlineStatus = OnlineStatus::online;
	onlineStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo("status_1")->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(onlineStatusIcon, onlineStatus.statusDescr());
	onlineStatus = OnlineStatus::chatOnline;
	QIcon chatStatusIcon;
	chatStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo("status_chat")->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(chatStatusIcon, onlineStatus.statusDescr());
	onlineStatus = OnlineStatus::away;	
	QIcon awayStatusIcon;
	awayStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo("status_2")->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(awayStatusIcon, onlineStatus.statusDescr());
	onlineStatus = OnlineStatus::invisible;	
	QIcon invisibleStatusIcon;
	invisibleStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo("status_3")->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(invisibleStatusIcon, onlineStatus.statusDescr());
	onlineStatus = OnlineStatus::dndOnline;
	QIcon dndStatusIcon;
	dndStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo("status_dnd")->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(dndStatusIcon, onlineStatus.statusDescr());

	connect(emailBox->lineEdit(), SIGNAL(textEdited(const QString&)), SLOT(checkEmail()));
	connect(emailBox, SIGNAL(editTextChanged(const QString&)), SLOT(checkOnlineStatus()));
	connect(passwordEdit, SIGNAL(textEdited(const QString&)), SLOT(checkPassword()));

	okButton->setDisabled(true);

	setFixedSize(sizeHint());
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	centerWindow(this);
}

OnlineStatus LoginDialog::status() const
{
	switch (onlineStatusBox->currentIndex())
	{
		case 0:
			return OnlineStatus::online;
		case 1:
			return OnlineStatus::chatOnline;
		case 2:
			return OnlineStatus::away;
		case 3:
			return OnlineStatus::invisible;
		case 4:
			return OnlineStatus::dndOnline;
		case 5:
			return OnlineStatus(statusId, statusDescr);
	}
	return OnlineStatus();
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

void LoginDialog::checkOnlineStatus()
{
	OnlineStatus st = theRM.loadOnlineStatus(email());
	if (extendedStatus)
	{
		onlineStatusBox->removeItem(5);
		extendedStatus = false;
	}
	if (st == OnlineStatus::online)
		onlineStatusBox->setCurrentIndex(0);
	else if (st == OnlineStatus::chatOnline)
		onlineStatusBox->setCurrentIndex(1);
	else if (st == OnlineStatus::away)
		onlineStatusBox->setCurrentIndex(2);
	else if (st == OnlineStatus::invisible)
		onlineStatusBox->setCurrentIndex(3);
	else if (st == OnlineStatus::dndOnline)
		onlineStatusBox->setCurrentIndex(4);
	else if (st.id() == "")
		onlineStatusBox->setCurrentIndex(0);
	else
	{
		QIcon onlineStatusIcon;
		onlineStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo(st.id())->icon(), QSize(), QIcon::Normal, QIcon::Off);
		onlineStatusBox->addItem(onlineStatusIcon, st.statusDescr());
		onlineStatusBox->setCurrentIndex(5);		
		extendedStatus = true;
		statusId = st.id();
		statusDescr = st.statusDescr();
	}
}
