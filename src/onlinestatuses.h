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

#ifndef ONLINESTATUSES_H
#define ONLINESTATUSES_H

#include <QCoreApplication>
#include <QIcon>
#include <QVariant>
#include <QMetaType>

class OnlineStatusInfo
{
	friend class OnlineStatuses;

	QString id_;
	QString icon_;
	QString builtin_;
	QString available_;

	void clear() { id_ = icon_ = builtin_ = available_ = ""; }
public:
	const QString id() const { return id_; }
	const QString icon() const { return icon_; }
	const QString builtIn() const { return builtin_; }
	const QString available() const { return available_; }
};

class OnlineStatusSet
{
	friend class OnlineStatuses;

	QString title_;
	QList<OnlineStatusInfo*> onlineStatusInfos;

	typedef QList<OnlineStatusInfo*>::const_iterator const_iterator;
	const_iterator begin() const { return onlineStatusInfos.begin(); }
	const_iterator end() const { return onlineStatusInfos.end(); }

public:
	~OnlineStatusSet() { qDeleteAll(onlineStatusInfos); }
	QString title() const { return title_; }
	int size();
	void addStatusesSet(QStringList & list);
};

class OnlineStatuses
{
public:
	OnlineStatuses() {};
	~OnlineStatuses() { qDeleteAll(onlineStatusSets); }
	
	void load(QString filename);
	const OnlineStatusInfo* getOnlineStatusInfo(QString id) const
		{ return idToOnlineStatusMap.value(id, NULL); }

	typedef QList<OnlineStatusSet*>::const_iterator const_iterator;
	const_iterator begin() const { return onlineStatusSets.begin(); }
	const_iterator end() const { return onlineStatusSets.end(); }

	const QStringList statusesList() const { return m_onlineStatusIds; }

	int numberOfSets() const { return onlineStatusSets.size(); }
	int statusesCount();

private:
	QMap<QString, OnlineStatusInfo*> idToOnlineStatusMap;
	QList<OnlineStatusSet*> onlineStatusSets;
	QStringList m_onlineStatusIds;
};

#endif
