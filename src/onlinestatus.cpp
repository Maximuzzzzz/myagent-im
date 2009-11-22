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

#include "onlinestatus.h"

#include <QDebug>

#include "proto.h"

const OnlineStatus OnlineStatus::unknown = OnlineStatus(Unknown);
const OnlineStatus OnlineStatus::offline = OnlineStatus(Offline);
const OnlineStatus OnlineStatus::invisible = OnlineStatus(Invisible);
const OnlineStatus OnlineStatus::away = OnlineStatus(Away);
const OnlineStatus OnlineStatus::online = OnlineStatus(Online);
const OnlineStatus OnlineStatus::connecting = OnlineStatus(Connecting);
const OnlineStatus OnlineStatus::unauthorized = OnlineStatus(Unauthorized);

OnlineStatus::OnlineStatus(StatusType type)
	: m_type(type)
{
}

QString OnlineStatus::description() const
{
	switch (m_type)
	{
		case Offline:
			return tr("Offline");
		case Invisible:
			return tr("Invisible");
		case Away:
			return tr("Away");
		case Online:
			return tr("Online");
		case Connecting:
			return tr("Connecting");
		default:
			return tr("Unknown");
	}
}

QIcon OnlineStatus::contactListIcon() const
{
	switch (m_type)
	{
		case Unknown:
			return QIcon(":/icons/status/status_gray.png");
		case Offline:
			return QIcon(":/icons/status/status_offline.png");
		case Invisible:
			return QIcon(":/icons/status/status_invisible.png");
		case Away:
			return QIcon(":/icons/status/status_away.png");
		case Online:
			return QIcon(":/icons/status/status_online.png");
		case Connecting:
			return QIcon(":/icons/status/status_connecting.png");
		case Unauthorized:
			return QIcon(":/icons/status/status_gray.png");
		default:
			return QIcon();
	}
}

QIcon OnlineStatus::chatWindowIcon() const
{
	switch (m_type)
	{
		case Unknown:
			return QIcon(":/icons/status/msg_stat_unauth.png");
		case Offline:
			return QIcon(":/icons/status/msg_stat_offline.png");
		case Invisible:
			return QIcon(":/icons/status/msg_stat_private.png");
		case Away:
			return QIcon(":/icons/status/msg_stat_away.png");
		case Online:
			return QIcon(":/icons/status/msg_stat_online.png");
		case Connecting:
			return QIcon(":/icons/status/msg_stat_connect.png");
		case Unauthorized:
			return QIcon(":/icons/status/msg_stat_unauth.png");
		default:
			return QIcon();
	}
}

quint32 OnlineStatus::protocolStatus() const
{
	switch (m_type)
	{
		case Offline:
			return STATUS_OFFLINE;
		case Invisible:
			return STATUS_FLAG_INVISIBLE | STATUS_ONLINE;
		case Away:
			return STATUS_AWAY;
		case Online:
			return STATUS_ONLINE;
		case Connecting:
			return STATUS_UNDETERMINATED;
		default:
			return STATUS_UNDETERMINATED;
	}
}

OnlineStatus OnlineStatus::fromProtocolStatus(quint32 st)
{
	if (st & STATUS_FLAG_INVISIBLE)
		return invisible;
	else if (st == STATUS_OFFLINE)
		return offline;
	else if (st == STATUS_AWAY)
		return away;
	else if (st == STATUS_ONLINE)
		return online;
	else
	{
		qDebug() << "unknown proto status: " << st;
		return unknown;
	}
}

bool OnlineStatus::connected() const
{
	switch (m_type)
	{
	case Online:
	case Invisible:
	case Away:
		return true;
	default:
		return false;
	}
}
