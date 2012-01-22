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

#ifndef MULTS_H
#define MULTS_H

#include <QString>
#include <QStringList>
#include <QMap>

class MultInfo
{
	friend class Mults;

	QString id_;
	QString fileName_;
	QString frames_;
	QString alt_;

	void clear() { id_ = fileName_ = frames_ = alt_ = ""; }
public:
	const QString id() const { return id_; }
	const QString fileName() const { return fileName_; }
	const QString frames() const { return frames_; }
	const QString alt() const { return alt_; }
};

class MultSet
{
	friend class Mults;

/*	QString title_;*/
	QList<MultInfo*> multInfos;

	typedef QList<MultInfo*>::const_iterator const_iterator;
	const_iterator begin() const { return multInfos.begin(); }
	const_iterator end() const { return multInfos.end(); }

public:
	~MultSet() { qDeleteAll(multInfos); }
/*	QString title() const { return title_; }
	int size();*/
	void addMultSet(QStringList & list);
};

class Mults
{
public:
	Mults() {};
	~Mults() { qDeleteAll(multSets); }
	
	void load(/*QString filename*/);
	const MultInfo* getMultInfo(QString id) const
		{ return idToMultMap.value(id, NULL); }

	typedef QList<MultSet*>::const_iterator const_iterator;
	const_iterator begin() const { return multSets.begin(); }
	const_iterator end() const { return multSets.end(); }

	const QStringList multList() const { return m_multIds; }

	int numberOfSets() const { return multSets.size(); }
	/*int multsCount();*/

private:
	QMap<QString, MultInfo*> idToMultMap;
	QList<MultSet*> multSets;
	QStringList m_multIds;
};

#endif
