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

#ifndef SMSEDITOR_H
#define SMSEDITOR_H

#include <QWidget>

#include "onlinestatus.h"

class QToolBar;
class SmsEdit;
class QLabel;
class QComboBox;
class QCheckBox;
class Contact;
class Account;

class SmsEditor : public QWidget
{
Q_OBJECT
public:
	SmsEditor(Account* account, Contact* contact, QWidget* parent = 0);
	~SmsEditor();

	QString text() const;
	QByteArray phoneNumber() const;

	void blockInput();
	void unblockInput();
	bool isBlocked();

signals:
	void sendPressed();

public slots:
	void clear();

protected:
	bool eventFilter(QObject * obj, QEvent * ev);

private slots:
	void updateRemainingSymbolsNumber(int n);
	void setAutoTranslit(bool b);
	void updatePhones();
	void checkCurrentPhoneIndex(int);
	void checkOnlineStatus(OnlineStatus);

private:
	void createToolBar();

private:
	QToolBar* toolBar;
	SmsEdit* smsEdit;
	QComboBox* phonesBox;
	QLabel* remainingSymbolsLabel;
	QCheckBox* translitCheckBox;
	Contact* m_contact;
	Account* m_account;
};

#endif
