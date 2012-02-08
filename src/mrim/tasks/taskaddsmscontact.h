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

#ifndef TASKSTASKADDSMSCONTACT_H
#define TASKSTASKADDSMSCONTACT_H

#include "simpleblockingtask.h"
#include "core/contactdata.h"

namespace Tasks
{

class AddSmsContact : public SimpleBlockingTask<Tasks::AddSmsContact>
{
Q_OBJECT
public:
	AddSmsContact(const QString& nickname, const QStringList& phones, MRIMClient* client, QObject* parent = 0);

	virtual bool exec();

	ContactData contactData() const;

private Q_SLOTS:
	void checkResult(quint32 msgseq, quint32 status, quint32 contactId);
private:
	quint32 m_contactId;
	QString m_nickname;
	QStringList m_phones;
};

}

#endif
