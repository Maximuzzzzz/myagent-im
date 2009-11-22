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

#include "datetime.h"

#include <QDebug>

#include <QRegExp>
#include <QStringList>

QDateTime parseRFCDate(const QString& rfcDate)
{
	static const char* shortDay[] = {
		"Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat",
		"Sun"
	};
	
	static const char* shortMonth[] = {
		"Jan", "Feb", "Mar", "Apr",
		"May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec"
	};
	
	qDebug() << "parsing RFC date";
	
	QString str = rfcDate.trimmed();
	if (str.isEmpty())
	{
		qDebug() << "empty date";
		return QDateTime();
	}
	
	QRegExp rx("^([A-Z][a-z]+),\\s+(\\d+)\\s+([A-Z][a-z]+)\\s+(\\d\\d\\d\\d)\\s(\\d\\d):(\\d\\d):(\\d\\d)");
	
	if (str.indexOf(rx) != 0)
	{
		qDebug() << "wrong date format";
		return QDateTime();
	}
	
	QStringList parts = rx.capturedTexts();
	int nyear  = 4;
	int nmonth = 3;
	int nday   = 2;
	int nwday  = 1;
	int nhour  = 5;
	int nmin   = 6;
	int nsec   = 7;
	
	bool ok[5];
	int day    = parts[nday].toInt(&ok[0]);
	int year   = parts[nyear].toInt(&ok[1]);
	int hour   = parts[nhour].toInt(&ok[2]);
	int minute = parts[nmin].toInt(&ok[3]);
	int second = parts[nsec].toInt(&ok[4]);
	if (!ok[0] || !ok[1] || !ok[2] || !ok[3] || !ok[4])
	{
		qDebug() << "day or year or hour or minute or second wrong";
		return QDateTime();
	}
	
	int month = 0;
	for (; month < 12  &&  parts[nmonth] != shortMonth[month]; ++month) ;
	if (month == 12)
	{
		qDebug() << "wrong month";
		return QDateTime();
	}
	
	int dayOfWeek = -1;
	while (++dayOfWeek < 7  &&  shortDay[dayOfWeek] != parts[nwday]) ;
	if (dayOfWeek >= 7)
	{
		qDebug() << "wrong day of week";
		return QDateTime();
	}
	
	qDebug() << year << month+1 << day;
	
	Qt::TimeSpec timeSpec= Qt::LocalTime;
	
	int pos = rx.matchedLength();
	qDebug() << "matchedLength = " << rx.matchedLength();
	rx = QRegExp("\\s+([A-Z]+)");
	pos = str.indexOf(rx, pos);
	qDebug() << "pos = " << pos << ", rx.cap(1) = " << rx.cap(1);
	if (pos != -1 && rx.cap(1) == "GMT")
		timeSpec = Qt::UTC;
	
	QDateTime result(QDate(year, month+1, day), QTime(hour, minute, second), timeSpec);
	if (!result.isValid() || result.date().dayOfWeek() != (dayOfWeek+1))
		return QDateTime();
	
	return result.toLocalTime();
}
