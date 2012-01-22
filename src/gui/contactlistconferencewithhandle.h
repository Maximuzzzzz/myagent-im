/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#ifndef CONTACTLISTCONFERENCEWITHHANDLE_H
#define CONTACTLISTCONFERENCEWITHHANDLE_H

#include <QWidget>

class QToolButton;
class QLabel;

class Contact;
class Account;
class ContactListConference;

class ContactListConferenceWithHandle : public QWidget
{
	Q_OBJECT
public:
	ContactListConferenceWithHandle(Contact* conference, Account* acc, QWidget* parent = 0);

	void toggle(bool visible);

Q_SIGNALS:
	void toggled(bool);

private Q_SLOTS:
	void toggleConferenceList(bool checked);
	void setMembersCount(quint32 cnt);

private:
	ContactListConference* contactListConference;
	QToolButton* handleButton;
	QLabel* members;
	QLabel* iconLabel;
	QLabel* addMembersLabel;
};

#endif // CONTACTLISTCONFERENCEWITHHANDLE_H
