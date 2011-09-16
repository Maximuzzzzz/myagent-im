/***************************************************************************
 *   Copyright (C) 2008 by Alexander Volkov                                *
 *   volkov0aa@gmail.com                                                   *
 *                                                                         *
 *   This file is part of instant messenger MyAgent-IM                       *
 *                                                                         *
 *   MyAgent-IM is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   MyAgent-IM is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TASKCHANGEGROUP_H
#define TASKCHANGEGROUP_H

#include "simpleblockingtask.h"

class Contact;

namespace Tasks
{

class ChangeGroup : public SimpleBlockingTask<Tasks::ChangeGroup>
{
Q_OBJECT
public:
	ChangeGroup(Contact* c, quint32 g, MRIMClient* client, QObject *parent = 0);
	
	bool exec();
	
	quint32 getGroup() const { return group; }

private Q_SLOTS:
	void checkResult(quint32 msgseq, quint32 status);

private:
	Contact* contact;
	quint32 group;
};

}
#endif
