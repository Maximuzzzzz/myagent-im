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

#include "taskgetmpopsession.h"

#include <QDebug>

#include "protocol/mrim/mrimclient.h"
#include "protocol/mrim/proto.h"

namespace Tasks
{

GetMPOPSession::GetMPOPSession(const QString& url, MRIMClient* client, QObject* parent)
	: SimpleBlockingTask<Tasks::GetMPOPSession>(client, parent), m_url(url)
{
}

}

bool Tasks::GetMPOPSession::exec()
{
	if (!block())
		return false;
	
	connect(mc, SIGNAL(receivedMPOPSession(quint32,quint32,QByteArray)), this, SLOT(checkResult(quint32,quint32,QByteArray)));
	
	
	return checkCall(mc->getMPOPSession());
}

void Tasks::GetMPOPSession::checkResult(quint32 msgseq, quint32 status, QByteArray session)
{
	if (isMyResponse(msgseq))
	{
		if (status == MRIM_GET_SESSION_SUCCESS)
			m_session = session;
		emit done(status, false);
		delete this;
	}
}
