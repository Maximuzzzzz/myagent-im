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

#ifndef TASKNEWCONFERENCE_H
#define TASKNEWCONFERENCE_H

#include "simpleblockingtask.h"

#include "contactdata.h"

namespace Tasks
{

class NewConference : public SimpleBlockingTask<Tasks::NewConference>
{
Q_OBJECT
public:
	NewConference(QString confName, QByteArray owner, MRIMClient* client, QList<QByteArray> members, QObject* parent = 0);

	virtual bool exec();

	ContactData contactData() const;

private Q_SLOTS:
	void checkResult(quint32 msgseq, quint32 status, quint32 contactId, const QByteArray& chatAgent);

private:
	QString m_conferenceName;
	QList<QByteArray> m_members;
	quint32 m_contactId;
	QByteArray m_chatAgent;
	QByteArray m_owner;
};

}

#endif
