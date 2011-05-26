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

#ifndef CONTACTDATA_H
#define CONTACTDATA_H

#include "mrimdatastream.h"
#include "onlinestatus.h"

#include <QStringList>
#include <QVariant>

class QTextCodec;

class Contact;
class ContactList;
class MRIMClient;

class ContactData
{
	friend class Contact;
	friend class ContactList;
	friend class MRIMClient;
public:
	ContactData();
	ContactData(const QByteArray& email); // for temporary contact
	ContactData(quint32 id, quint32 flags, quint32 group, QByteArray email, QString nick, QStringList phones = QStringList());
	ContactData(quint32 id, MRIMDataStream& stream, const QByteArray& mask);
	static const QByteArray dataMask();

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

	bool isConference() const;
	QByteArray getClient() const;
	//QIcon getClientIcon() const;

	void prepareForSending(MRIMDataStream& stream) const;
private:
	static QTextCodec* codec;

	quint32 id;
	quint32 flags;
	quint32 group;
	QByteArray email;
	QString nick;
	quint32 internalFlags;
	OnlineStatus status;
	QStringList phones;
	QByteArray client;
	
	QString tailMask;
	QList<QVariant> tailData;
};

#endif
