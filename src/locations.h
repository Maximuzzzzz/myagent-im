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

#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <QString>
#include <QList>

class Locations
{
public:
	class Country
	{
	friend class Locations;
	public:
		struct Region
		{
			public:
				Region(int id, const QString& name) : m_id(id), m_name(name) {}
				
				int id() const { return m_id; }
				QString name() const { return m_name; }
			private:
				int m_id;
				QString m_name;
		};
		
		Country(int id, const QString& name) : m_id(id), m_name(name) {}
		
		int id() const { return m_id; }
		QString name() const { return m_name; }
		
		typedef QList<Region>::const_iterator regionsIterator;
		regionsIterator regionsBegin() const { return regions.begin(); }
		regionsIterator regionsEnd() const { return regions.end(); }
		regionsIterator region(int id) const;
		
	private:
		int m_id;
		QString m_name;
		QList<Region> regions;
	};
	
	Locations();

	typedef QList<Country>::const_iterator countriesIterator;
	countriesIterator countriesBegin() const { return countries.begin(); }
	countriesIterator countriesEnd() const { return countries.end(); }
	countriesIterator country(int id) const;
	
	void load(const QString& filename);
	
private:
	typedef QList<Country>::iterator iterator;
	iterator countriesBegin() { return countries.begin(); }
	iterator countriesEnd() { return countries.end(); }
	iterator country(int id);
	
	QList<Country> countries;
};

#endif
