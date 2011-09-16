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

#include "taskrequestcontactinfo.h"

#include "protocol/mrim/proto.h"

namespace Tasks
{

RequestContactInfo::RequestContactInfo(QByteArray email, MRIMClient* client, QObject *parent)
	: SimpleBlockingTask<Tasks::RequestContactInfo>(client, parent), m_email(email)
{
}

bool RequestContactInfo::exec()
{
	qDebug() << "RequestContactInfo::exec()";
	if (!block())
		return false;
	
	connect(mc, SIGNAL(contactInfoReceived(quint32, quint32, MRIMClient::ContactsInfo, quint32, quint32)), this, SLOT(checkResult(quint32, quint32, MRIMClient::ContactsInfo, quint32, quint32)));
	
	return checkCall(mc->requestContactInfo(m_email));
}

void RequestContactInfo::checkResult(quint32 msgseq, quint32 status, MRIMClient::ContactsInfo info, quint32, quint32 serverTime)
{
	qDebug() << "RequestContactInfo::checkResult";
	if (isMyResponse(msgseq))
	{
//		qDebug() << "contactinfo received, status = " << status;
		
		if (status == MRIM_ANKETA_INFO_STATUS_OK)
		{
			m_info.setServerTime(serverTime);
			m_info.m_username = info["Username"].first();
			m_info.m_domain = info["Domain"].first();
			m_info.m_nickname = info["Nickname"].first();
			m_info.m_firstname = info["FirstName"].first();
			m_info.m_lastname = info["LastName"].first();
			m_info.m_location= info["Location"].first();
			m_info.setSex(info["Sex"].first());
			m_info.setBirthday(info["Birthday"].first());
			m_info.setZodiac(info["Zodiac"].first());
			m_info.setOnlineStatus(info["mrim_status"].first());
			m_info.setPhones(info["Phone"].first());
		}
		
		Q_EMIT done(status, false);
		delete this;
	}
	else
		qDebug() << "received some wrong contactinfo";
}

}
