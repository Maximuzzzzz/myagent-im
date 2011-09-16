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

#ifndef ONLINESTATUS_H
#define ONLINESTATUS_H

#include <QCoreApplication>
#include <QIcon>
#include <QVariant>
#include <QMetaType>

#include "onlinestatuses.h"

class OnlineStatus
{
	Q_DECLARE_TR_FUNCTIONS(OnlineStatus)
public:
	enum StatusType
	{
		Online = 1,			//Don't rearrange these strings!!!
		Away = 2,			//
		Invisible = 3,		//
		OtherOnline = 4,	//
		Offline = 5,		//
		Unauthorized = 6,	//
		Connecting = 7,		//
		Unknown = 8,		//
		Null = 9			//
	};
	
	static const OnlineStatus unknown;
	static const OnlineStatus offline;
	static const OnlineStatus invisible;
	static const OnlineStatus away;
	static const OnlineStatus online;
	static const OnlineStatus connecting;
	static const OnlineStatus unauthorized;
	static const OnlineStatus chatOnline;
	static const OnlineStatus seekFriends;
	static const OnlineStatus dndOnline;
	static const OnlineStatus otherOnline;
	static const OnlineStatus wrongData;

	void setExtendedStatus(QString status);

	StatusType type() const { return m_type; }
	QByteArray id() const { return m_idStatus; }
	QString statusDescr() { return m_statusDescr; }
	QString description() const;
	quint32 protocolStatus() const;
	static OnlineStatus fromProtocolStatus(quint32 st);
	QIcon statusIcon() const;
	bool connected() const;
	QByteArray getDefIdStatus(int at) { return (m_defaultIdStatuses.size() > at) ? m_defaultIdStatuses.at(at) : ""; }
	QString getDefDescrStatus(int at) { return (m_defaultDescrStatuses.size() > at) ? m_defaultDescrStatuses.at(at) : ""; }

	void setIdStatus(QByteArray status);
	void setDescr(QString descr);

	explicit OnlineStatus(QByteArray idStatus = "", QString statusDescr = "");
	
	bool operator==(OnlineStatus another) { return (m_idStatus == another.m_idStatus); }
	bool operator!=(OnlineStatus another) { return (m_idStatus != another.m_idStatus && m_statusDescr != another.m_statusDescr); }
	bool operator<=(OnlineStatus another) { return (m_type <= another.m_type);  }
	bool operator<(OnlineStatus another) { return (m_type < another.m_type); }

private Q_SLOTS:
	void setMType();

private:
	StatusType m_type;
	QByteArray m_idStatus;
	QString m_statusDescr;
	OnlineStatuses* m_onlineStatuses;
	QList<QByteArray> m_defaultIdStatuses;
	QStringList m_defaultDescrStatuses;
};

Q_DECLARE_METATYPE(OnlineStatus)

#endif
