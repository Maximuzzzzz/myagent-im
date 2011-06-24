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

#ifndef TASKSTASKCHANGEPHONES_H
#define TASKSTASKCHANGEPHONES_H

#include "simpleblockingtask.h"

#include <QStringList>

class Contact;

namespace Tasks
{

class ChangePhones : public SimpleBlockingTask<Tasks::ChangePhones>
{
Q_OBJECT
public:
	ChangePhones(Contact* contact, const QStringList& phones, MRIMClient* client, QObject* parent = 0);
	
	bool exec();
	
	QStringList phones() const { return m_phones; }

private slots:
	void checkResult(quint32 msgseq, quint32 status);

private:
	Contact* m_contact;
	QStringList m_phones;
};

}

#endif
