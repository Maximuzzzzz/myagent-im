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

#include "zodiac.h"

Zodiac::Zodiac(int z)
{
	if (0 < z && z < 13)
		zodiacNum = z;
	else
		zodiacNum = 0;
}

QPixmap Zodiac::pixmap() const
{
	if (zodiacNum)
		return QPixmap(":/icons/horo/horo_" + QString::number(zodiacNum) + ".png");
	
	return QPixmap();
}

static const char* zodiac[] =
{
	QT_TRANSLATE_NOOP("Zodiac", "Aries"),
	QT_TRANSLATE_NOOP("Zodiac", "Taurus"),
	QT_TRANSLATE_NOOP("Zodiac", "Gemini"),
	QT_TRANSLATE_NOOP("Zodiac", "Cancer"),
	QT_TRANSLATE_NOOP("Zodiac", "Leo"),
	QT_TRANSLATE_NOOP("Zodiac", "Virgo"),
	QT_TRANSLATE_NOOP("Zodiac", "Libra"),
	QT_TRANSLATE_NOOP("Zodiac", "Scorpio"),
	QT_TRANSLATE_NOOP("Zodiac", "Sagittarius"),
	QT_TRANSLATE_NOOP("Zodiac", "Capricorn"),
	QT_TRANSLATE_NOOP("Zodiac", "Aquarius"),
	QT_TRANSLATE_NOOP("Zodiac", "Pisces")
};

QString Zodiac::name() const
{
	if (zodiacNum)
		return tr(zodiac[zodiacNum-1]);
	else
		return "";
}

Zodiac Zodiac::fromNumber(int z)
{
	return Zodiac(z);
}
