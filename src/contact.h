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

#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QString>

#include "onlinestatus.h"
#include "contactdata.h"
#include "contactgroup.h"

class Account;
class QDataStream;

class Contact : public QObject
{
Q_OBJECT
public:
	Contact(const ContactData& contactData, ContactGroup* group, Account* account);
	Contact(Account* account);
	~Contact();
	
	void update(const ContactData& contactData, ContactGroup* group);

	quint32 id() const { return data.id; }
	quint32 flags() const { return data.flags; }
	QString nickname() const { return data.nick; }
	OnlineStatus status() const;
	QStringList phones() const { return data.phones; }
	QByteArray email() const { return data.email; }
	ContactGroup* group() const { return m_group; }
	bool changeGroup(quint32 group);
	void setAuthorized();
	quint32 setMyVisibility(bool alwaysVisible, bool alwaysInvisible);
	void rename(const QString& nickname);
	void changePhones(const QStringList& phones);
	QString path() const;
	const Account* account() const { return m_account; }
	bool isPhone() const;
	bool isTemporary() const;
	bool isHidden() const;
	bool hasPhone(QByteArray phoneNumber) const;
	
	ContactData contactData() const { return data; }
	
	void load(QDataStream& stream);
	void save(QDataStream& stream) const;
	
	void updateStatus();
signals:
	void updated();
	void statusChanged(OnlineStatus status);
	void typing();
	void groupChanged(bool result);
	void visibilityChanged();
	void renamed(QString newNickname);
	void phonesChanged();

	void destroyed(Contact* obj = 0);
	
public slots:
	void changeStatus(quint32 status);
	void changeStatus(OnlineStatus status);
	void contactTyping();

private slots:
	void slotChangeGroupResult(quint32 status, bool timeout);
	void changePhonesEnd(quint32 status, bool timeout);
	void slotSetVisibilityResult(quint32 status, bool timeout);
	void slotRenameResult(quint32 status, bool timeout);
	void updateStatusEnd(quint32 status, bool timeout);

private:
	void setGroup(quint32 id);

private:
	ContactData data;
	ContactGroup* m_group;
	Account* m_account;
};

#endif
