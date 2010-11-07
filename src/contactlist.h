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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include <QList>

#include "contactgroup.h"
#include "mrimclient.h"
#include "onlinestatus.h"
#include "contactdata.h"

class Contact;
class Account;

class ContactList : public QObject
{
Q_OBJECT
public:
	ContactList(Account* account);
	~ContactList();

	void beginUpdating();
	void endUpdating();

	void addGroup(quint32 id, quint32 flags, const QString& name);
	Contact* addContact(const ContactData& data);
	void addContact(Contact* contact);

	typedef QList<ContactGroup*>::const_iterator GroupsIterator;
	GroupsIterator groupsBegin() const { return m_groups.begin(); }
	GroupsIterator groupsEnd() const { return m_groups.end(); }
	uint nGroups() const { return m_groups.size(); }

	ContactGroup* group(quint32 id) const;

	typedef QList<Contact*>::const_iterator ConstContactsIterator;
	ConstContactsIterator contactsBegin() const { return m_contacts.begin(); }
	ConstContactsIterator contactsEnd() const { return m_contacts.end(); }

	typedef QList<Contact*>::iterator ContactsIterator;
	ContactsIterator contactsBegin() { return m_contacts.begin(); }
	ContactsIterator contactsEnd() { return m_contacts.end(); }

	Contact* findContact(const QByteArray& email);
	Contact* findContact(const QByteArray& email, QList<Contact*> & list);
	Contact* findSmsContact(const QString& nickname);
	Contact* getContact(const QByteArray& email);
	Contact* findContactWithPhone(const QByteArray& phoneNumber);

	void clear();
	void load();
	void save() const;

	bool addTemporaryContactToGroup(Contact* contact, quint32 group);

	bool removeContactOnServer(Contact* contact);
	bool removeGroupOnServer(ContactGroup* group);
	bool addContactOnServer(quint32 group, const QByteArray& email, const QString& nickname, const QString& authorizationMessage);
	bool addSmsContactOnServer(const QString& nickname, const QStringList& phones);
	bool addGroupOnServer(const QString& groupName);
	bool renameGroup(ContactGroup* group, const QString& newName);

signals:
	void groupAdded(ContactGroup* group);
	void groupRemoved(ContactGroup* group);
	void groupsCleared();
	void contactAdded(Contact* contact);
	void contactRemoved(Contact*);
	void updated();

	void removeContactOnServerError(QString error);
	void removeGroupOnServerError(QString error);
	void addContactOnServerError(QString error);
	void newConferenceOnServerError(QString error);
	void addSmsContactOnServerError(QString error);
	void addGroupOnServerError(QString error);
	void renameGroupError(QString error);

public slots:
	void changeContactStatus(quint32 status, QByteArray email);
	void contactTyping(QByteArray email);
	void slotContactAuthorized(const QByteArray& email);
	bool newConferenceOnServer(QString confName, QByteArray owner, QList<QByteArray> members = QList<QByteArray>());
	bool addConferenceOnServer(const QByteArray & chat, const QString & confName);

private slots:
	void checkOnlineStatus(OnlineStatus status);

	void removeContactOnServerEnd(quint32 status, bool timeout);
	void removeGroupOnServerEnd(quint32 status, bool timeout);
	void addContactOnServerEnd(quint32 status, bool timeout);
	void newConferenceOnServerEnd(quint32 status, bool timeout);
	void addSmsContactOnServerEnd(quint32 status, bool timeout);
	void addGroupOnServerEnd(quint32 status, bool timeout);
	void renameGroupEnd(quint32 status, bool timeout);

private:
	QList<ContactGroup*> m_groups;
	QList<ContactGroup*> m_hiddenGroups;
	QList<Contact*> m_contacts;
	QList<Contact*> tmpContacts;

	bool constructing;

	Account* m_account;
};

#endif
