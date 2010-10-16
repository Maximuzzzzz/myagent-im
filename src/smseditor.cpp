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

#include "smseditor.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>

#include "smsedit.h"
#include "contact.h"
#include "account.h"
#include "contactinfodialog.h"
#include "resourcemanager.h"

SmsEditor::SmsEditor(Account* account, Contact* contact, QWidget* parent)
	: QWidget(parent), m_contact(contact), m_account(account)
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	
	smsEdit = new SmsEdit(account->email().size() + 2);
	smsEdit->installEventFilter(this);
	smsEdit->setFocus(Qt::OtherFocusReason);
	createToolBar(); // after smsEdit only

	layout->addWidget(toolBar);
	layout->addWidget(smsEdit);
	setLayout(layout);

	connect(m_contact, SIGNAL(phonesChanged()), this, SLOT(updatePhones()));
	connect(account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(checkOnlineStatus(OnlineStatus)));
	checkOnlineStatus(account->onlineStatus());
}

SmsEditor::~SmsEditor()
{
	//qDebug() << "SmsEditor::~SmsEditor()";
}

bool SmsEditor::eventFilter(QObject * obj, QEvent * ev)
{
	if (obj == smsEdit && ev->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
		qDebug() << "SmsEditor::eventFilter, key =" << QString::number(keyEvent->key(), 16) << ", mods =" << QString::number(keyEvent->modifiers(), 16);
		if (keyEvent->key() == Qt::Key_Return)
		{
			QString enterVariant = m_account->settings()->value("Messages/sendOnEnter", "Ctrl+Enter").toString();
			if (enterVariant == "Enter")
			{
				if (!keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::NoModifier)
				{
					emit sendPressed();
					return true;
				}
			}
			else if (enterVariant == "Enter+Enter")
			{
				if (keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::NoModifier)
				{
					emit sendPressed();
					return true;
				}
			}
			else if (enterVariant == "Ctrl+Enter")
			{
				if (!keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::ControlModifier)
				{
					emit sendPressed();
					return true;
				}
			}
		}
		
		if ((keyEvent->key() == Qt::Key_S || keyEvent->key() == 0x42b) && keyEvent->modifiers() == Qt::AltModifier)
		{
			emit sendPressed();
			return true;
		}
	}
	
	return QWidget::eventFilter(obj, ev);
}

void SmsEditor::createToolBar()
{
	toolBar = new QToolBar;
	//toolBar->setStyleSheet("QToolBar { border: 0px; }");

	translitCheckBox = new QCheckBox(tr("Auto transliteration"));
	connect(translitCheckBox, SIGNAL(clicked(bool)), this, SLOT(setAutoTranslit(bool)));
	toolBar->addWidget(translitCheckBox);
	toolBar->addSeparator();

	phonesBox = new QComboBox;
	updatePhones();
	connect(phonesBox, SIGNAL(activated(int)), SLOT(checkCurrentPhoneIndex(int)));
	toolBar->addWidget(phonesBox);
	toolBar->addSeparator();
	
	QLabel* txtLabel = new QLabel(tr(" symbols remaining: "));
	toolBar->addWidget(txtLabel);
	
	remainingSymbolsLabel = new QLabel;
	connect(smsEdit, SIGNAL(freeSpaceChanged(int)), this, SLOT(updateRemainingSymbolsNumber(int)));
	updateRemainingSymbolsNumber(smsEdit->freeSpace());
	toolBar->addWidget(remainingSymbolsLabel);
}

void SmsEditor::updateRemainingSymbolsNumber(int n)
{
	remainingSymbolsLabel->setText(QString::number(n, 10));
}

QString SmsEditor::text() const
{
	return smsEdit->text();
}

QByteArray SmsEditor::phoneNumber() const
{
	return phonesBox->currentText().toLatin1();
}

void SmsEditor::clear()
{
	smsEdit->clear();
}

void SmsEditor::blockInput()
{
	smsEdit->setReadOnly(true);
}

void SmsEditor::unblockInput()
{
	smsEdit->setReadOnly(false);
}

bool SmsEditor::isBlocked()
{
	return smsEdit->isReadOnly();
}

void SmsEditor::setAutoTranslit(bool b)
{
	if (!smsEdit->setAutoTranslit(b))
		translitCheckBox->toggle();

	smsEdit->setFocus(Qt::OtherFocusReason);
}

void SmsEditor::updatePhones()
{
	phonesBox->clear();
	phonesBox->addItems(m_contact->phones());
	phonesBox->addItem(tr("Edit..."));
}

void SmsEditor::checkCurrentPhoneIndex(int index)
{
	if (index == (phonesBox->count() - 1))
	{
		ContactInfoDialog::create(m_account, m_contact->email());
		phonesBox->setCurrentIndex(0);
		smsEdit->setFocus(Qt::OtherFocusReason);
	}
}

void SmsEditor::checkOnlineStatus(OnlineStatus status)
{
	phonesBox->setEnabled(status.connected());
}

void SmsEditor::smsEditorActivate()
{
	smsEdit->setFocus();
qDebug() << "SMSEditor::smsEditorActivate";
}

bool SmsEditor::event(QEvent* event)
{
	if (event->type() == 17 || event->type() == 24) //If event type == QEvent::Show or QEvent::WindowActivate
		smsEdit->setFocus();
	return true;
}
