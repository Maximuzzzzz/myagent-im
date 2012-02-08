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

#include "contactgroup.h"

#include <QDataStream>

ContactGroup::ContactGroup(quint32 id, quint32 flags, QString name, GroupType t)
	: m_id(id), m_flags(flags), m_name(name), m_type(t)
{
	m_expanded = false;
}

ContactGroup::ContactGroup(QDataStream& stream)
{
	m_expanded = false;
	quint32 typeInt;
	stream >> m_id >> m_flags >> m_name >> typeInt >> m_expanded;
	m_type = intToType(typeInt);
}

void ContactGroup::update(quint32 flags, QString name)
{
	m_flags = flags;
	m_name = name;
}

quint32 ContactGroup::typeToInt(GroupType t)
{
	switch (t)
	{
		case Simple:
			return 0;
		case Conferences:
			return 1;
		case Phones:
			return 2;
		case Temporary:
			return 3;
		case NotInGroup:
			return 4;
		case NotAuthorized:
			return 5;
		default:
			return -1;
	}
}

ContactGroup::GroupType ContactGroup::intToType(quint32 n) const
{
	switch (n)
	{
		case 0:
			return Simple;
		case 1:
			return Conferences;
		case 2:
			return Phones;
		case 3:
			return Temporary;
		case 4:
			return NotInGroup;
		case 5:
			return NotAuthorized;
		default:
			return None;
	}
}

QDataStream & operator <<(QDataStream & stream, ContactGroup* group)
{
	stream << group->id() << group->flags() << group->name() << group->typeToInt(group->m_type) << group->isExpanded();
	return stream;
}

QDataStream & operator >>(QDataStream & stream, ContactGroup* group)
{
	quint32 typeInt;
	stream >> group->m_id >> group->m_flags >> group->m_name >> typeInt >> group->m_expanded;
	group->m_type = group->intToType(typeInt);
	return stream;
}
