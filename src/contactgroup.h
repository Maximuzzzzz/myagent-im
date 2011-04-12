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

#ifndef CONTACTGROUP_H
#define CONTACTGROUP_H

#include <QString>
#include <QDataStream>

class ContactGroup
{
public:
	enum GroupType
	{
		Simple,
		Conferences,
		Phones,
		Temporary,
		NotInGroup,
		NotAuthorized,
		None
	};

	ContactGroup(quint32 id, quint32 flags, QString name, GroupType t = Simple);
	ContactGroup(QDataStream& stream);
	
	quint32 id() const { return m_id; }
	quint32 flags() const { return m_flags; }
	QString name() const { return m_name; }

	GroupType groupType() const { return m_type; }
	bool isExpanded() const { return m_expanded; }

	void setName(const QString& name) { m_name = name; }
	void setExpanded(bool b) { m_expanded = b; }

private:
	quint32 typeToInt(GroupType t);
	GroupType intToType(quint32 n);

	quint32 m_id;
	quint32 m_flags;
	QString m_name;
	bool m_expanded;

	GroupType m_type;

	friend QDataStream& operator<<(QDataStream& stream, ContactGroup* group);	
	friend QDataStream& operator>>(QDataStream& stream, ContactGroup* group);
};

#endif
