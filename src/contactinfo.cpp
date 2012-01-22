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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "contactinfo.h"

#include <QDebug>

#include "protocol/mrim/proto.h"

void ContactInfo::setSex(const QString & sex)
{
	if (sex == "1")
		m_sex = tr("Male");
	else if (sex == "2")
		m_sex = tr("Female");
}

void ContactInfo::setBirthday(const QString& birthday)
{
	m_birthday = QDate::fromString(birthday, "yyyy-MM-dd");

	if (m_serverTime.isValid() && m_birthday.isValid())
	{
		int age = m_serverTime.date().year() - m_birthday.year();
		QDateTime birthdayDT
		(
			QDate(m_serverTime.date().year(), m_birthday.month(), m_birthday.day())
		);

		if (m_serverTime < birthdayDT)
			--age;

		m_age = QString::number(age);
	}
}

void ContactInfo::setZodiac(const QString& zodiac)
{
	bool ok;
	int z = zodiac.toInt(&ok);
	if (ok && 1 <= z && z <= 12)
		m_zodiac = Zodiac(z);
}

void ContactInfo::setOnlineStatus(const OnlineStatus& status)
{
	qDebug() << "ContactInfo::setOnlineStatus status = " << status.description();
	m_onlineStatus = status;
}

QString ContactInfo::errorDescription(quint32 status)
{
	if (status == MRIM_ANKETA_INFO_STATUS_NOUSER)
		return tr("No contacts were found");
	else if (status == MRIM_ANKETA_INFO_STATUS_RATELIMERR)
		return tr("Search is temporarily unavailable");
	else if (status != MRIM_ANKETA_INFO_STATUS_OK)
		return tr("Unknown error");
	else return "";
}

void ContactInfo::setPhones(const QString & phones)
{
	m_formPhones = phones.split(',', QString::SkipEmptyParts);

	for (int i = 0; i < m_formPhones.size(); i++)
	{
		if (!m_formPhones.at(i).startsWith('+'))
			m_formPhones[i].prepend('+');
	}
}
