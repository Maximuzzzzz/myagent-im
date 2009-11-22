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

#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QDateTime>
#include "zodiac.h"
#include "onlinestatus.h"

namespace Tasks
{
	class RequestContactInfo;
	class SearchContacts;
}

class ContactInfo
{
	Q_DECLARE_TR_FUNCTIONS(ContactInfo)
	friend class Tasks::SearchContacts;
	friend class Tasks::RequestContactInfo;
public:
	QString username() const { return m_username; }
	QString domain() const { return m_domain; }
	QString email() const { return m_username + "@" + m_domain; }
	QString nickname() const { return m_nickname; }
	QString firstname() const { return m_firstname; }
	QString lastname() const { return m_lastname; }
	QString sex() const { return m_sex; }
	QDate birthday() const { return m_birthday; }
	QString location() const { return m_location; }
	Zodiac zodiac() const { return m_zodiac; }
	QString age() const { return m_age; }
	OnlineStatus onlineStatus() const { return m_onlineStatus; }
	QStringList formPhones() const { return m_formPhones; }
	
	static QString errorDescription(quint32 status);
	
private:
	void setServerTime(quint32 serverTime) { m_serverTime.setTime_t(serverTime); }
	void setSex(const QString& sex);
	void setBirthday(const QString& birthday);
	void setZodiac(const QString& zodiac);
	void setOnlineStatus(const QString& status);
	void setPhones(const QString& phones);
	
	QString m_username;
	QString m_domain;
	QString m_nickname;
	QString m_firstname;
	QString m_lastname;
	QString m_sex;
	QDate m_birthday;
	QString m_location;
	Zodiac m_zodiac;
	OnlineStatus m_onlineStatus;
	QDateTime m_serverTime;
	QString m_age;
	QStringList m_formPhones;
};

#endif
