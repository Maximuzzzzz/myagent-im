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

#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QString>
#include <QStringList>
#include <QMap>

class Emoticons;
class QSettings;

class EmoticonInfo
{
	friend class Emoticons;

	QString id_;
	QString alt_;
	QString tip_;
	QString iconPath_;

	void clear() { id_ = alt_ = tip_ = iconPath_ = ""; }
public:
	const QString id() const { return id_; }
	const QString alt() const { return alt_; }
	const QString tip() const { return tip_; }
	const QString path() const { return iconPath_; }
};

class EmoticonSet
{
	friend class Emoticons;

	QString title_;
	QList<EmoticonInfo*> emoticonInfos;
public:
	~EmoticonSet() { qDeleteAll(emoticonInfos); }
	QString title() const { return title_; }

	typedef QList<EmoticonInfo*>::const_iterator const_iterator;
	const_iterator begin() const { return emoticonInfos.begin(); }
	const_iterator end() const { return emoticonInfos.end(); }
	int size() const { return emoticonInfos.size(); }
};

class Emoticons
{
public:
	Emoticons() {};
	~Emoticons() { qDeleteAll(emoticonSets); }

	void load(QString filename, QSettings* settings);
	const EmoticonInfo* getEmoticonInfo(QString id) const
		{ return idToEmoticonMap.value(id); }

	typedef QList<EmoticonSet*>::const_iterator const_iterator;
	const_iterator begin() const { return emoticonSets.begin(); }
	const_iterator end() const { return emoticonSets.end(); }

	const QStringList favouriteEmoticons() const { return m_favouriteEmoticons; }
	void setFavouriteEmoticons(const QStringList& emoticonIds);

	int numberOfSets() const { return emoticonSets.size(); }
	int maxSetSize() const { return m_maxSetSize; }
private:
	QMap<QString, EmoticonInfo*> idToEmoticonMap;
	QList<EmoticonSet*> emoticonSets;
	int m_maxSetSize;
	QStringList m_favouriteEmoticons;
};

#endif
