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

#ifndef TASKIGNORECONTACT_H
#define TASKIGNORECONTACT_H

#include "simpleblockingtask.h"

class Contact;

namespace Tasks
{

class IgnoreContact : public Tasks::SimpleBlockingTask<Tasks::IgnoreContact>
{
Q_OBJECT
public:
	IgnoreContact(Contact* c, const quint32 flags, MRIMClient* client, QObject* parent = 0);

	virtual bool exec();

	quint32 getFlags() const { return m_flags; }
	Contact* contact() { return m_contact; }

private slots:
	void checkResult(quint32 msgseq, quint32 status);
private:
	Contact* m_contact;
	quint32 m_flags;
};

}

#endif
