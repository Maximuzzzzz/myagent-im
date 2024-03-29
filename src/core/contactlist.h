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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include <QList>

#include "contactdata.h"

class Contact;
class Account;
class ContactListModel;
class ContactGroup;

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

	ContactListModel* model() { return m_model; }

	void clear();
	void load();
	void save() const;
	void update();

	bool addTemporaryContactToGroup(Contact* contact, quint32 group);

	bool removeContactOnServer(Contact* contact);
	bool removeGroupOnServer(ContactGroup* group);
	bool ignoreContactOnServer(Contact* contact, bool ignore);
	bool addContactOnServer(quint32 group, const QByteArray& email, const QString& nickname, const QString& authorizationMessage);
	bool addSmsContactOnServer(const QString& nickname, const QStringList& phones);
	bool addGroupOnServer(const QString& groupName);
	bool renameGroup(ContactGroup* group, const QString& newName);

	ContactGroup* phones() { return m_phones; }
	ContactGroup* conferences() { return m_conferences; }
	ContactGroup* temporary() { return m_temporary; }
	ContactGroup* notInGroup() { return m_notInGroup; }
	ContactGroup* notAuthorized() { return m_notAuthorized; }

Q_SIGNALS:
	void groupAdded(ContactGroup* group);
	void groupRenamed(ContactGroup* group);
	void groupRemoved(ContactGroup* group);
	void groupsCleared();
	void contactAdded(Contact* contact);
	void contactRemoved(Contact*);
	void contactIgnored(bool ignored);
	void updated();

	void removeContactOnServerError(QString error);
	void removeGroupOnServerError(QString error);
	void addContactOnServerError(QString error);
	void newConferenceOnServerError(QString error);
	void addSmsContactOnServerError(QString error);
	void addGroupOnServerError(QString error);
	void renameGroupError(QString error);

public Q_SLOTS:
	void changeContactStatus(OnlineStatus status, QByteArray email);
	void contactTyping(QByteArray email);
	void slotContactAuthorized(const QByteArray& email);
	bool newConferenceOnServer(QString confName, QByteArray owner, QList<QByteArray> members = QList<QByteArray>());
	bool addConferenceOnServer(const QByteArray & chat, const QString & confName);
	void ignoreContactOnServerEnd(quint32 status, bool timeout);
	void setLastSmsFrom(const QByteArray& number, Contact* c);
	void useModel(ContactListModel* model) { m_model = model; }

private Q_SLOTS:
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
	QList<Contact*> m_receivedContacts;
	QList<ContactGroup*> m_receivedGroups;

	QMap<QByteArray, Contact*> m_lastSmsFrom;

	ContactListModel* m_model;

	ContactGroup* m_phones;
	ContactGroup* m_conferences;
	ContactGroup* m_temporary;
	ContactGroup* m_notInGroup;
	ContactGroup* m_notAuthorized;

	Account* m_account;

	bool m_updatingFromServer;
};

#endif
