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

#include "tasksearchcontacts.h"

#include "protocol/mrim/proto.h"

Tasks::SearchContacts::SearchContacts(const QByteArray& email, MRIMClient* client, QObject* parent)
	: SimpleBlockingTask<Tasks::SearchContacts>(client, parent), m_email(email)
{
}

Tasks::SearchContacts::SearchContacts(const MRIMClient::SearchParams& params, MRIMClient* client, QObject* parent)
	: SimpleBlockingTask<Tasks::SearchContacts>(client, parent), m_params(params)
{
}

bool Tasks::SearchContacts::exec()
{
	qDebug() << "SearchContacts::exec()";
	if (!block())
		return false;

	connect(mc, SIGNAL(contactInfoReceived(quint32, quint32, MRIMClient::ContactsInfo, quint32, quint32)), this, SLOT(checkResult(quint32, quint32, MRIMClient::ContactsInfo, quint32, quint32)));

	setTimer(mc->getPingTime());

	if (!m_email.isEmpty())
		return checkCall(mc->requestContactInfo(m_email));
	else
		return checkCall(mc->searchContacts(m_params));
}

void Tasks::SearchContacts::checkResult(quint32 msgseq, quint32 status, MRIMClient::ContactsInfo info, quint32 maxRows, quint32 serverTime)
{
	qDebug() << "SearchContacts::checkResult";
	if (isMyResponse(msgseq))
	{
		qDebug() << "contactinfos received";

		if (status == MRIM_ANKETA_INFO_STATUS_OK)
		{
			m_maxRows = maxRows;

			int n = info[info.keys().first()].size();

			qDebug() << "n = " << n;

			for (int i = 0; i < n; i++)
			{
				ContactInfo contactInfo;

				contactInfo.setServerTime(serverTime);
				contactInfo.m_username = info["Username"].at(i);
				contactInfo.m_domain = info["Domain"].at(i);
				contactInfo.m_nickname = info["Nickname"].at(i);
				contactInfo.m_firstname = info["FirstName"].at(i);
				contactInfo.m_lastname = info["LastName"].at(i);
				contactInfo.m_location= info["Location"].at(i);
				contactInfo.setSex(info["Sex"].at(i));
				contactInfo.setBirthday(info["Birthday"].at(i));
				contactInfo.setZodiac(info["Zodiac"].at(i));

				OnlineStatus onlineStatus;
				QString status = info["mrim_status"].at(i);
				if (status.isEmpty())
					onlineStatus = OnlineStatus::unknown;
				else
				{
					if (info.contains("status_uri") && !info["status_uri"].at(i).isEmpty())
					{
						QByteArray statusId = info["status_uri"].at(i).toAscii();
						QString statusDescr = info.value("status_title").value(i);
						qDebug() << Q_FUNC_INFO << "statusId =" << statusId << ", statusDescr =" << statusDescr;
						onlineStatus = OnlineStatus(statusId, statusDescr);
					}
					else
					{
						bool ok;
						quint32 protocolStatus = status.toUInt(&ok, 16);
						if (ok)
							onlineStatus = OnlineStatus::fromProtocolStatus(protocolStatus);
						else
							onlineStatus = OnlineStatus::unknown;
					}
				}

				contactInfo.setOnlineStatus(onlineStatus);

				contactsInfo.append(contactInfo);
			}
		}

		Q_EMIT done(status, false);
		delete this;
	}
	else
		qDebug() << "received some wrong contactinfos";
}
