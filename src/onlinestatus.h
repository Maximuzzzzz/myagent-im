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
		Online = 1,		// Don't rearrange these strings!!!
		Away = 2,		//
		Invisible = 3,		//
		OtherOnline = 4,	//
		Offline = 5,		//
		Unauthorized = 6,	//
		Connecting = 7,		//
		Unknown = 8,		//
		Null = 9		//
	};

	static const OnlineStatus unknown;
	static const OnlineStatus offline;
	static const OnlineStatus invisible;
	static const OnlineStatus away;
	static const OnlineStatus online;
	static const OnlineStatus connecting;
	static const OnlineStatus unauthorized;
	static const OnlineStatus otherOnline;
	static const OnlineStatus wrongData;

	OnlineStatus();
	explicit OnlineStatus(QByteArray id, QString description = QString());
	static OnlineStatus fromProtocolStatus(quint32 st);

	StatusType type() const { return m_type; }
	QByteArray id() const;
	QString description() const;
	quint32 protocolStatus() const;

	QIcon statusIcon() const;
	bool connected() const;

	bool builtIn() const;
	bool available() const;

	bool operator== (OnlineStatus another) const;
	bool operator!= (OnlineStatus another) const;
	bool operator< (OnlineStatus another) const;

	static QByteArray getDefIdStatus(int at);
	static QString getDefDescrStatus(int at);

private:
	StatusType m_type;
	QByteArray m_id;
	QString m_statusDescr;
};

Q_DECLARE_METATYPE(OnlineStatus)

#endif
