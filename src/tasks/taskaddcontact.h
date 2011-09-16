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

#ifndef TASKADDCONTACT_H
#define TASKADDCONTACT_H

#include "simpleblockingtask.h"

#include "contactdata.h"

namespace Tasks
{

class AddContact : public SimpleBlockingTask<Tasks::AddContact>
{
Q_OBJECT
public:
	AddContact(quint32 group, const QByteArray& email, const QString& nickname, const QString& authorizationMessage, MRIMClient* client, QObject* parent = 0);

	virtual bool exec();

	quint32 group() const { return m_group; }
	QString nickname() const { return m_nickname; }
	QByteArray email() const { return m_email; }
	quint32 contactId() const { return m_contactId; }

	ContactData contactData() const;

private Q_SLOTS:
	void checkResult(quint32 msgseq, quint32 status, quint32 contactId);

private:
	quint32 m_group;
	QString m_nickname;
	QByteArray m_email;
	QString m_authorizationMessage;
	quint32 m_contactId;
};

}

#endif
