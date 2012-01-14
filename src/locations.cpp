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

#include "locations.h"

#include <QDebug>

#include <QFile>
#include <QTextStream>

Locations::Locations()
{
}

Locations::countriesIterator Locations::country(int id) const
{
	countriesIterator it = countriesBegin();
	
	for (; it != countriesEnd(); ++it)
		if (it->id() == id)
			break;
		
	return it;
}

Locations::iterator Locations::country(int id)
{
	iterator it = countriesBegin();
	
	for (; it != countriesEnd(); ++it)
		if (it->id() == id)
			break;
	
	return it;
}

Locations::Country::regionsIterator Locations::Country::region(int id) const
{
	regionsIterator it = regionsBegin();
	
	for (; it != regionsEnd(); ++it)
		if (it->id() == id)
			break;
	
	return it;
}

void Locations::load(const QString& filename)
{
	qDebug() << "Locations::load(" << filename << ")";
	
	QFile data(filename);
	if (!data.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "can't open file";
		return;
	}
	
	QTextStream textStream(&data);
	textStream.setCodec("cp1251");
	
	textStream.readLine();
	
	int id, regionId, countryId;
	QString name;
	
	while (!textStream.atEnd())
	{
		textStream >> id >> regionId >> countryId;
		name = textStream.readLine().trimmed();
		
		if (id == countryId)
		{
			countries.append(Country(id, name));
		}
		else if (id == regionId)
		{
			iterator it = country(countryId);
			if (it != countriesEnd())
			{
				QString regionName = name.mid(it->name().size()+1);
				it->regions.append(Country::Region(regionId, regionName));
			}
			else
				qDebug() << "! country doesn't exist";
		}
	}
	
	data.close();
}
