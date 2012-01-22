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

#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QString>
#include <QDateTime>

#include "onlinestatus.h"
#include "contactdata.h"

class QDataStream;
class Account;
class ContactGroup;

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
	void setAuthorized();
	quint32 setMyVisibility(bool alwaysVisible, bool alwaysInvisible);
	void rename(const QString& nickname);
	void changePhones(const QStringList& phones);
	QString path() const;
	const Account* account() const { return m_account; }
	bool isPhone() const;
	bool isTemporary() const;
	bool isHidden() const;
	bool isConference() const;
	bool hasPhone(QByteArray phoneNumber) const;
	bool changeGroup(quint32 group);
	bool isIgnored() const;
	bool isNotAuthorized() const;
	QIcon chatWindowIcon(QString type = "");
	void setShowMicroblogText(bool really) { m_showMicroblogText = really; }
	bool showMicroblogText() { return m_showMicroblogText; }
	void setMicroblog(QString text, QDateTime mbDateTime) { m_microblogText = text; m_microblogDateTime = mbDateTime; }
	QString microblogText() { return m_microblogText; }
	QDateTime microblogDateTime() { return m_microblogDateTime; }
	QByteArray client() { return data.getClient(); }

	ContactData contactData() const { return data; }
	
	void load(QDataStream& stream);
	void save(QDataStream& stream) const;
	
	void updateStatus();
Q_SIGNALS:
	void updated();
	void statusChanged(OnlineStatus status);
	void typing();
	void groupChanged(bool result);
	void visibilityChanged();
	void ignoredChanged();
	void renamed(QString newNickname);
	void phonesChanged();

	void destroyed(Contact* obj = 0);
	
public Q_SLOTS:
	void changeStatus(OnlineStatus status);
	void contactTyping();
	void setFlags(quint32 f);

private Q_SLOTS:
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
	bool m_activeConference;
	bool m_showMicroblogText;
	QString m_microblogText;
	QDateTime m_microblogDateTime;
};

#endif
