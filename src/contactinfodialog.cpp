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

#include "contactinfodialog.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QFocusFrame>
#include <QLabel>
#include <QIcon>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QStyle>

#include "account.h"
#include "contact.h"
#include "avatarbox.h"
#include "linkbutton.h"
#include "zodiac.h"
#include "contactinfo.h"
#include "taskrequestcontactinfo.h"
#include "proto.h"
#include "centerwindow.h"
#include "centeredmessagebox.h"

QList<ContactInfoDialog*> ContactInfoDialog::windows = QList<ContactInfoDialog*>();

void ContactInfoDialog::initWindow()
{
	setWindowIcon(QIcon(":icons/anketa.png"));
	setWindowTitle(tr("User info"));
	setAttribute(Qt::WA_DeleteOnClose);
}

ContactInfoDialog::ContactInfoDialog(Account* account, const ContactInfo& info)
	: m_account(account), m_email(info.email().toLatin1())
{
	initWindow();
	createContent(info);
	setFixedSize(sizeHint());
}

ContactInfoDialog::ContactInfoDialog(Account* account, const QByteArray& email)
	: m_account(account), m_email(email)
{
	initWindow();
	
	QWidget* helperWidget = new QWidget;
	helperWidget->setWindowIcon(QIcon(":icons/anketa.png"));
	helperWidget->setWindowTitle(tr("User info"));
	helperWidget->setAttribute(Qt::WA_DeleteOnClose);
	QVBoxLayout* layout = new QVBoxLayout;
	requestLabel = new QLabel(tr("Requesting info"));
	requestLabel->setAlignment(Qt::AlignCenter);
	cancelButton = new QPushButton(tr("Cancel"));
	connect(cancelButton, SIGNAL(clicked()), helperWidget, SLOT(close()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	layout->addWidget(requestLabel);
	layout->addWidget(cancelButton);
	helperWidget->setLayout(layout);
	helperWidget->setFixedSize(helperWidget->sizeHint());

	Task* task = new Tasks::RequestContactInfo(m_email, account->client(), this);
	connect(task, SIGNAL(done(quint32, bool)), helperWidget, SLOT(close()));
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(showContactInfo(quint32, bool)));

	task->exec();

	centerWindow(helperWidget);
	helperWidget->show();
}

QIcon ContactInfoDialog::linkIcon(const QString &linkType)
{
	QIcon icon;
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_d.png"), QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_p.png"), QIcon::Active, QIcon::On);
	
	return icon;
}

ContactInfoDialog::~ContactInfoDialog()
{
	qDebug() << "ContactInfoDialog::~ContactInfoDialog() " << m_email;
	windows.removeAll(this);
}

void ContactInfoDialog::showContactInfo(quint32 status, bool timeout)
{
	qDebug() << "ContactInfoDialog::showContactInfo";
	
	if (timeout)
	{
		CenteredMessageBox::warning(tr("User info"), tr("Time is out"));
		return;
	}
	else if (status != MRIM_ANKETA_INFO_STATUS_OK)
	{
		CenteredMessageBox::warning(tr("User info"), ContactInfo::errorDescription(status));
		return;
	}
	
	Tasks::RequestContactInfo* task = qobject_cast<Tasks::RequestContactInfo*>(sender());

	createContent(task->getContactInfo());
	setFixedSize(sizeHint());
	show();
}

void ContactInfoDialog::createContent(const ContactInfo & info)
{
	QString domain = info.domain().section('.', 0, 0);
	QString username = info.username();
	
	QVBoxLayout* layout = new QVBoxLayout;
	QHBoxLayout* topLayout = new QHBoxLayout;	
	
	QVBoxLayout* avatarBoxLayout = new QVBoxLayout;
	AvatarBox* avatarBox = new AvatarBox(m_account->avatarsPath(), info.email());
	avatarBoxLayout->addWidget(avatarBox);
	avatarBoxLayout->addStretch();
	
	QFormLayout* infoLayout = new QFormLayout;
	infoLayout->setVerticalSpacing(1);
	infoLayout->setHorizontalSpacing(30);
	
	QLabel* emailLabel = new QLabel("<a href=\"mailto:" + m_email + "\">" + m_email + "</a>");
	emailLabel->setOpenExternalLinks(true);
	infoLayout->addRow(tr("E-mail"), emailLabel);

	Contact* contact = m_account->contactList()->findContact(m_email);

	QHBoxLayout* statusLayout = new QHBoxLayout;
	QLabel* statusIconLabel = new QLabel;
	if (!contact)
	{
		statusIconLabel->setPixmap(info.onlineStatus().statusIcon().pixmap(16));
		statusLayout->addWidget(new QLabel(info.onlineStatus().description()));
	}
	else
	{
		statusIconLabel->setPixmap(contact->status().statusIcon().pixmap(16));
		statusLayout->addWidget(new QLabel(contact->status().statusDescr()));
	}
	statusLayout->addSpacing(4);
	statusLayout->addWidget(statusIconLabel);
	statusLayout->addStretch();
	infoLayout->addRow(tr("Status"), statusLayout);
	
	infoLayout->addRow(tr("Nickname"), new QLabel(info.nickname()));
	infoLayout->addRow(tr("LastName"), new QLabel(info.lastname()));
	infoLayout->addRow(tr("FirstName"), new QLabel(info.firstname()));
	infoLayout->addRow(tr("Sex"), new QLabel(info.sex()));
	infoLayout->addRow(tr("Age"), new QLabel(info.age()));
	infoLayout->addRow(tr("Birthday"), new QLabel(info.birthday().toString("d MMMM yyyy")));
	
	Zodiac zodiac = info.zodiac();
	QHBoxLayout* zodiacLayout = new QHBoxLayout;
	zodiacLayout->addWidget(new QLabel(zodiac.name()));
	QLabel* zodiacSign = new QLabel;
	zodiacSign->setPixmap(zodiac.pixmap());
	zodiacLayout->addSpacing(4);
	zodiacLayout->addWidget(zodiacSign);
	zodiacLayout->addStretch();
	infoLayout->addRow(tr("Zodiac"), zodiacLayout);
	
	infoLayout->addRow(tr("Location"), new QLabel(info.location()));

	topLayout->addLayout(avatarBoxLayout);
	topLayout->addLayout(infoLayout);

	QHBoxLayout* phonesLayout = new QHBoxLayout;

	QGroupBox* additionalPhonesBox = new QGroupBox(tr("Editable phones"));
	QFormLayout* additionalPhonesLayout = new QFormLayout;
	phoneEdit1 = new QLineEdit;
	phoneEdit2 = new QLineEdit;
	phoneEdit3 = new QLineEdit;
	
	int phoneWidth = phoneEdit1->fontMetrics().width("+000000000000") + 4*phoneEdit1->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	phoneEdit1->setFixedWidth(phoneWidth);
	phoneEdit2->setFixedWidth(phoneWidth);
	phoneEdit3->setFixedWidth(phoneWidth);

	phoneEdit1->setValidator(new QRegExpValidator(QRegExp("|\\+(\\d){11,12}"), phoneEdit1));
	phoneEdit2->setValidator(new QRegExpValidator(QRegExp("|\\+(\\d){11,12}"), phoneEdit2));
	phoneEdit3->setValidator(new QRegExpValidator(QRegExp("|\\+(\\d){11,12}"), phoneEdit3));

	connect(phoneEdit1, SIGNAL(textEdited(const QString&)), SLOT(checkPhones()));
	connect(phoneEdit2, SIGNAL(textEdited(const QString&)), SLOT(checkPhones()));
	connect(phoneEdit3, SIGNAL(textEdited(const QString&)), SLOT(checkPhones()));

	if (contact)
	{
		QStringList phones = contact->phones();
		phoneEdit1->setText(phones.value(0));
		phoneEdit2->setText(phones.value(1));
		phoneEdit3->setText(phones.value(2));
	}
	else
	{
		phoneEdit1->setDisabled(true);
		phoneEdit2->setDisabled(true);
		phoneEdit3->setDisabled(true);
	}
	additionalPhonesLayout->addRow(tr("Primary phone"), phoneEdit1);
	additionalPhonesLayout->addRow(tr("Additional phone 1"), phoneEdit2);
	additionalPhonesLayout->addRow(tr("Additional phone 2"), phoneEdit3);

	additionalPhonesBox->setLayout(additionalPhonesLayout);

	QStringList formPhones = info.formPhones();
	QGroupBox* formPhonesBox = new QGroupBox(tr("Form phones"));
	QFormLayout* formPhonesLayout = new QFormLayout;
	QLabel* label1 = new QLabel(formPhones.value(0));
	QLabel* label2 = new QLabel(formPhones.value(1));
	QLabel* label3 = new QLabel(formPhones.value(2));
	label1->setFixedSize(phoneEdit1->sizeHint());
	label2->setMinimumHeight(phoneEdit2->sizeHint().height());
	label3->setMinimumHeight(phoneEdit3->sizeHint().height());
	formPhonesLayout->addRow(tr("Primary phone"), label1);
	formPhonesLayout->addRow(tr("Additional phone 1"), label2);
	formPhonesLayout->addRow(tr("Additional phone 2"), label3);
	formPhonesBox->setLayout(formPhonesLayout);

	phonesLayout->addWidget(formPhonesBox);
	phonesLayout->addWidget(additionalPhonesBox);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(savePhones()));
	if (!contact)
		buttonBox->button(QDialogButtonBox::Save)->setDisabled(true);


	layout->addLayout(topLayout);
	layout->addLayout(phonesLayout);
	layout->addWidget(buttonBox);

	setLayout(layout);
}

ContactInfoDialog* ContactInfoDialog::create(Account * account, const ContactInfo & info)
{
	ContactInfoDialog* wnd = searchWindow(info.email().toLatin1());
	if (!wnd)
	{
		wnd = new ContactInfoDialog(account, info);
		windows.append(wnd);
		wnd->show();
	}
	else
	{
		wnd->activateWindow();
		wnd->raise();
	}
	
	return wnd;
}

ContactInfoDialog* ContactInfoDialog::create(Account * account, const QByteArray & email)
{
	ContactInfoDialog* wnd = searchWindow(email);
	if (!wnd)
	{
		wnd = new ContactInfoDialog(account, email);
		windows.append(wnd);
	}
	else
	{
		wnd->activateWindow();
		wnd->raise();
	}
	
	return wnd;
}

ContactInfoDialog* ContactInfoDialog::searchWindow(const QByteArray & email)
{
	int nWindows = windows.size();
	for (int i = 0; i < nWindows; i++)
	{
		ContactInfoDialog* wnd = windows.at(i);
		if (wnd->m_email == email)
			return wnd;
	}
	
	return 0;
}

void ContactInfoDialog::show()
{
	centerWindow(this);
	QWidget::show();
}

void ContactInfoDialog::checkPhones()
{
	QPushButton* saveButton = buttonBox->button(QDialogButtonBox::Save);
	
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

QStringList ContactInfoDialog::editablePhones() const
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

void ContactInfoDialog::savePhones()
{
	Contact* contact = m_account->contactList()->findContact(m_email);
	if (contact)
		contact->changePhones(editablePhones());
	close();
}
