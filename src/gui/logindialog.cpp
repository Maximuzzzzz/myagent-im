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
#include "gui/centerwindow.h"

LoginDialog::LoginDialog(QWidget* parent)
	: QDialog(parent), isExtendedStatus(false), isSavePassword(false)
{
	setWindowFlags(Qt::Dialog & !Qt::WindowContextHelpButtonHint);
	setupUi(this);

	QDir dir(theRM.basePath());
	QStringList filters;
	filters << "?*@mail.ru" << "?*@list.ru" << "?*inbox.ru" << "?*bk.ru" << "?*corp.mail.ru";
	loginBox->addItems(dir.entryList(filters, QDir::Dirs | QDir::CaseSensitive));
	loginBox->lineEdit()->clear();
	loginBox->lineEdit()->completer()->setCompletionMode(QCompleter::PopupCompletion);

	loginBox->lineEdit()->setValidator(new QRegExpValidator(QRegExp("([a-z]|[A-Z]|[0-9]|[_])+([a-z]|[A-Z]|[0-9]|[_\\-\\.])*@(mail.ru|list.ru|inbox.ru|bk.ru|corp.mail.ru)"), loginBox));
	passwordEdit->setValidator(new QRegExpValidator(QRegExp(".+"), passwordEdit));

	QList<QByteArray> statuses;
	statuses << "status_1" << "status_dating" << "status_chat" << "status_2" << "status_3" << "status_dnd";

	Q_FOREACH (const QByteArray& status, statuses)
	{
		OnlineStatus onlineStatus;
		onlineStatus.setIdStatus(status);
		if (theRM.onlineStatuses()->getOnlineStatusInfo(onlineStatus.id())->available() == "1")
		{
			QIcon currStatusIcon;
			currStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo(onlineStatus.id())->icon(), QSize(), QIcon::Normal, QIcon::Off);
			onlineStatusBox->addItem(currStatusIcon, onlineStatus.statusDescr());
			onlineStatusBox->setItemData(onlineStatusBox->count() - 1, status, Qt::UserRole + 1);
		}
	}

	connect(loginBox->lineEdit(), SIGNAL(textEdited(const QString&)), SLOT(checkEmail()));
	connect(loginBox, SIGNAL(editTextChanged(const QString&)), SLOT(slotEmailChanged()));
	connect(domainBox, SIGNAL(currentIndexChanged(QString)), SLOT(slotEmailChanged()));
	connect(passwordEdit, SIGNAL(textEdited(const QString&)), SLOT(checkPassword()));

	buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

	sizeWithHint = sizeHint();
	passwordHint->setVisible(false);

	sizeWithoutHint = sizeHint();
	setFixedSize(sizeWithoutHint);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(settingsButton, SIGNAL(clicked()), this, SLOT(showSettingsWindow()));
	connect(savePass, SIGNAL(clicked()), this, SLOT(slotSavePassChecked()));

	centerWindow(this);
}

OnlineStatus LoginDialog::status() const
{
	OnlineStatus res;
	res.setIdStatus(onlineStatusBox->itemData(onlineStatusBox->currentIndex(), Qt::UserRole + 1).toByteArray());
	return res;
}

void LoginDialog::setEmail(const QString & email)
{
	loginBox->lineEdit()->setText(email);
	passwordEdit->setFocus(Qt::OtherFocusReason);
}

void LoginDialog::checkEmail()
{
	if (loginBox->lineEdit()->text().contains('@'))
		domainBox->setVisible(false);
	else
		domainBox->setVisible(true);
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(loginBox->lineEdit()->hasAcceptableInput());
}

void LoginDialog::checkPassword()
{
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(passwordEdit->hasAcceptableInput());
	currentPassword = passwordEdit->text().toLatin1();
}

void LoginDialog::slotEmailChanged()
{
	if (domainBox->isVisible() && loginBox->currentText().contains('@'))
	{
		QString domain = "@" + loginBox->currentText().section('@', 1);
		int i = domainBox->findText(domain);
		if (i != -1)
		{
			domainBox->blockSignals(true);
			domainBox->setCurrentIndex(i);
			domainBox->blockSignals(false);

			loginBox->blockSignals(true);
			loginBox->setEditText(loginBox->currentText().section('@', 0, 0));
			loginBox->blockSignals(false);
		}

		if (loginBox->currentText().contains('@'))
			domainBox->setVisible(false);
	}

	QByteArray savedPassword = theRM.loadPass(email());
	if (!savedPassword.isEmpty())
	{
		passwordEdit->setText(savedPassword);
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(passwordEdit->hasAcceptableInput());
		savePass->setChecked(true);
		slotSavePassChecked();
	}
	else
	{
		passwordEdit->setText(currentPassword);
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(passwordEdit->hasAcceptableInput());
		savePass->setChecked(isSavePassword);
		slotSavePassChecked();
	}

	onlineStatusBox->setUpdatesEnabled(false);
	if (isExtendedStatus)
	{
		onlineStatusBox->removeItem(onlineStatusBox->count() - 1);
		isExtendedStatus = false;
	}

	OnlineStatus lastStatus = theRM.loadOnlineStatus(email());
	if (lastStatus.id().isEmpty()
	    || lastStatus == OnlineStatus::wrongData
	    || lastStatus == OnlineStatus::unknown
	    || lastStatus == OnlineStatus::offline
	    || lastStatus == OnlineStatus::connecting)
	{
		onlineStatusBox->setCurrentIndex(0);
		onlineStatusBox->setUpdatesEnabled(true);
		return;
	}

	int i = onlineStatusBox->findData(lastStatus.id(), Qt::UserRole + 1);
	if (i != -1)
	{
		onlineStatusBox->setCurrentIndex(i);
		onlineStatusBox->setUpdatesEnabled(true);
		return;
	}

	QIcon onlineStatusIcon;
	onlineStatusIcon.addFile(theRM.statusesResourcePrefix() + ":" + theRM.onlineStatuses()->getOnlineStatusInfo(lastStatus.id())->icon(), QSize(), QIcon::Normal, QIcon::Off);
	onlineStatusBox->addItem(onlineStatusIcon, lastStatus.statusDescr());
	onlineStatusBox->setCurrentIndex(onlineStatusBox->count() - 1);
	onlineStatusBox->setUpdatesEnabled(true);
	isExtendedStatus = true;
}

void LoginDialog::showSettingsWindow()
{
	if (!settingsWindow.isNull())
		return;

	settingsWindow = new SettingsWindow(SHOW_CONNECTION_PAGE);
	centerWindow(settingsWindow.data());
	settingsWindow.data()->show();
}

void LoginDialog::slotSavePassChecked()
{
	passwordHint->setVisible(savePass->isChecked());
	isSavePassword = savePass->isChecked();

	if (savePass->isChecked())
		setFixedSize(sizeWithHint);
	else
		setFixedSize(sizeWithoutHint);
}

QByteArray LoginDialog::email() const
{
	if (!loginBox->lineEdit()->text().contains("@"))
		return loginBox->currentText().append(domainBox->currentText()).toLatin1();
	else
		return loginBox->currentText().toLatin1();
}
