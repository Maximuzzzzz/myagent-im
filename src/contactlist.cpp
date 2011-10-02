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

#include "contactlist.h"

#include <QDebug>

#include <QFile>
#include <QDataStream>

#include "protocol/mrim/proto.h"
#include "account.h"
#include "contact.h"
#include "tasks/taskremovecontact.h"
#include "tasks/taskaddcontact.h"
#include "tasks/taskaddsmscontact.h"
#include "tasks/taskaddgroup.h"
#include "tasks/taskremovegroup.h"
#include "tasks/taskrenamegroup.h"
#include "tasks/taskignorecontact.h"
#include "tasks/tasknewconference.h"

ContactList::ContactList(Account* account)
	: QObject(account), m_account(account)
{
	constructing = false;
	connect(m_account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(checkOnlineStatus(OnlineStatus)));

	m_phones = new ContactGroup(0, 0, tr("Phone contacts"), ContactGroup::Phones);
	m_conferences = new ContactGroup(0, 0, tr("Conferences"), ContactGroup::Conferences);
	m_temporary = new ContactGroup(0, 0, tr("Temporary"), ContactGroup::Temporary);
	m_notInGroup = new ContactGroup(0, 0, tr("Not in group"), ContactGroup::NotInGroup);
	m_notAuthorized = new ContactGroup(0, 0, tr("Waiting for authorization"), ContactGroup::NotAuthorized);
}

ContactList::~ContactList()
{
	qDebug() << "ContactList::~ContactList()";
	save();
	clear();

	delete m_phones;
	m_phones = 0;

	delete m_conferences;
	m_conferences = 0;

	delete m_temporary;
	m_temporary = 0;

	delete m_notInGroup;
	m_notInGroup = 0;

	delete m_notAuthorized;
	m_notAuthorized = 0;
}

void ContactList::clear()
{
	qDebug() << "--- ContactList::clear() ---";
	qDeleteAll(m_groups);
	m_groups.clear();

	Q_EMIT groupsCleared();

	qDeleteAll(m_hiddenGroups);
	m_hiddenGroups.clear();

	qDeleteAll(m_contacts);
	m_contacts.clear();
	
	constructing = false;
	Q_EMIT updated();
}

void ContactList::addGroup(quint32 id, quint32 flags, const QString& name)
{
	qDebug() << Q_FUNC_INFO << constructing;
	ContactGroup* group = new ContactGroup(id, flags, name);

	QList<ContactGroup*>::iterator it = m_groups.begin();
	for (; it != m_groups.end(); ++it)
		if ((*it)->id() == group->id())
		{
			if (constructing)
			{
				(*it)->update((*it)->flags(), group->name());
				tmpGroups.append(*it);
				m_groups.removeAll(*it);
				delete group;
				Q_EMIT groupAdded(*it);
			}
			else
			{
				m_groups.removeAll(*it);
				m_groups.append(group);
				//delete (*it);
				Q_EMIT groupAdded(group);
			}
			return;
		}
	if (constructing)
		tmpGroups.append(group);
	else
	{
		m_groups.append(group);
		Q_EMIT updated();
	}

	Q_EMIT groupAdded(group);
}

Contact* ContactList::addContact(const ContactData& data)
{
	ContactGroup* group;
	uint id = data.group;

	QList<ContactGroup*>::const_iterator it = ((constructing) ? tmpGroups.begin() : m_groups.begin());
	QList<ContactGroup*>::const_iterator end_it = (constructing) ? tmpGroups.end() : m_groups.end();
	while (it != end_it && (*it)->id() != id) ++it;
	if (it != end_it)
		group = *it;
	else
	{
		group = new ContactGroup(data.group, 0, "");
		m_hiddenGroups.append(group);
	}

	Contact* contact = findContact(data.email, (constructing) ? (tmpContacts) : (m_contacts));
	if (contact)
	{
		qDebug() << "preserving contact" << contact->email();
		((constructing) ? (tmpContacts) : (m_contacts)).removeAll(contact);
		contact->update(data, group);
	}
	else
	{
		qDebug() << "creating new contact for contact" << data.email << data.nick;
		contact = new Contact(data, group, m_account);
	}
	
	if (constructing)
	{
		qDebug() << "adding contact to temporary contacts";
		tmpContacts.append(contact);
	}
	else
	{
		qDebug() << "adding contact to main contacts";
		addContact(contact);
	}

	return contact;
}

void ContactList::addContact(Contact* contact)
{
	m_contacts.append(contact);

	if (!constructing) Q_EMIT contactAdded(contact);
}

void ContactList::changeContactStatus(OnlineStatus status, QByteArray email)
{
	Contact* contact = findContact(email);
	if (contact)
		contact->changeStatus(status);
}

Contact* ContactList::findContact(const QByteArray & email)
{
	return findContact(email, (constructing) ? (tmpContacts) : (m_contacts));
}

Contact* ContactList::findContact(const QByteArray& email, QList<Contact*> & list)
{
	if (email == "phone")
		return 0;
	
	QList<Contact*>::iterator it = list.begin();
	
	while (it != list.end() && (*it)->email() != email)
		++it;

	if (it == list.end())
		return 0;
	else
		return *it;
}

Contact* ContactList::findSmsContact(const QString & nickname)
{
	QList<Contact*>::iterator it = m_contacts.begin();
	
	while (it != m_contacts.end())
	{
		bool phone = (*it)->isPhone();
		if (!phone || (*it)->nickname() != nickname)
			++it;
		else
			break;
	}
	
	if (it == m_contacts.end())
		return 0;
	else
		return *it;
}

Contact* ContactList::getContact(const QByteArray& email)
{
	Contact* contact = findContact(email);
	if (contact)
		return contact;
	
	qDebug() << "ContactList::getContact: can't find contact for email " << email;
	// creating temporary contact
	contact = new Contact(ContactData(email), 0, m_account);
	addContact(contact);
	return contact;
}

Contact* ContactList::findContactWithPhone(const QByteArray& phoneNumber)
{
	qDebug() << phoneNumber;

	Contact* c = lastSmsFrom.value(phoneNumber, NULL);
	if (c)
	{
		qDebug() << "Contact found" << c->nickname();
		return c;
	}
	qDebug() << "Contact didn't find";

	QList<Contact*>::iterator it = m_contacts.begin();
	
	while (it != m_contacts.end() && !(*it)->hasPhone(phoneNumber))
		++it;

	if (it != m_contacts.end())
		return *it;

	return 0;
}

void ContactList::contactTyping(QByteArray email)
{
	Contact* contact = findContact(email);
	if (contact)
		contact->contactTyping();
}

void ContactList::beginUpdating()
{
	qDebug() << "ContactList::beginUpdating()";

	Q_EMIT groupsCleared();
	
	qDeleteAll(m_hiddenGroups);
	m_hiddenGroups.clear();

	qDeleteAll(tmpContacts);
	tmpContacts.clear();

	qDeleteAll(tmpGroups);
	tmpGroups.clear();
	
	constructing = true;
}

void ContactList::endUpdating()
{
	qDebug() << "ContactList::endUpdating()";

	qDeleteAll(m_groups);
	m_groups = tmpGroups;

	QList<Contact*> contactsForDelete;
	QList<Contact*>::iterator it = m_contacts.begin();
	for (; it != m_contacts.end(); ++it)
	{
		QList<Contact*>::iterator tmp_it = tmpContacts.begin();
		bool found = false;
		for (; tmp_it != tmpContacts.end(); ++tmp_it)
		{
			if ((*it)->email() == (*tmp_it)->email())
			{
				found = true;
				//Update m_contacts contact, found in tmpContacts
				(*it)->update((*tmp_it)->contactData(), (*tmp_it)->group());
				//Remove item from tmpContacts;
				tmpContacts.removeAll(*tmp_it);
				break;
			}
		}
		if (!found)
			contactsForDelete.append(*it);
	}

	//Remove from m_contacts contact, which not found in tmpContacts
	for (it = contactsForDelete.begin(); it != contactsForDelete.end(); ++it)
		m_contacts.removeAll(*it);
	qDeleteAll(contactsForDelete);
	contactsForDelete.clear();

	//Add in m_contacts all contacts, residuary in tmpContacts
	for (it = tmpContacts.begin(); it != tmpContacts.end(); ++it)
		m_contacts.append(*it);

//	qDeleteAll(m_contacts);
//	m_contacts = tmpContacts;
	
	constructing = false;
	Q_EMIT updated();
}

bool ContactList::removeContactOnServer(Contact* contact)
{
	qDebug() << "ContactList::removeContact " << contact->email();
	if (!contact)
	{
		qDebug() << "removeContact: contact doesn't exist";
		return false;
	}
	
	if (contact->isTemporary() && !contact->isConference())
	{
		m_contacts.removeAll(contact);
		Q_EMIT contactRemoved(contact);

		return true;
	}

	Task* task = new Tasks::RemoveContact(contact, m_account->client(), this);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(removeContactOnServerEnd(quint32, bool)));
	
	return task->exec();
}

void ContactList::removeContactOnServerEnd(quint32 status, bool timeout)
{
	if (timeout || status != CONTACT_OPER_SUCCESS)
	{
		qDebug() << "checkRemoveContactResult: timeout = " << timeout << ", status = " << status;
		Q_EMIT removeContactOnServerError(tr("Removing contact failed"));
		return;
	}
	
	Tasks::RemoveContact* task = qobject_cast<Tasks::RemoveContact*>(sender());
	Contact* contact = task->contact();
	m_contacts.removeAll(contact);
	Q_EMIT contactRemoved(contact);
}

bool ContactList::ignoreContactOnServer(Contact* contact, bool ignore)
{
	qDebug() << "ContactList::removeContact " << contact->email();
	if (!contact)
	{
		qDebug() << "removeContact: contact doesn't exist";
		return false;
	}

	quint32 contFlags;
	if (ignore)
		contFlags = contact->flags() | CONTACT_FLAG_IGNORE;
	else
		contFlags = contact->flags() & ~CONTACT_FLAG_IGNORE;

	Task* task = new Tasks::IgnoreContact(contact, contFlags, m_account->client());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(ignoreContactOnServerEnd(quint32, bool)));

	return task->exec();
}

void ContactList::ignoreContactOnServerEnd(quint32 status, bool timeout)
{
	qDebug() << Q_FUNC_INFO;
	if (timeout == true || status != CONTACT_OPER_SUCCESS)
	{
		qDebug() << "Contact: ignoring contact error";
		return;
	}

	Tasks::IgnoreContact* task = qobject_cast<Tasks::IgnoreContact*>(sender());
	if (!task)
	{
		qDebug() << "slotIgnoreResult strange sender";
		return;
	}

	task->contact()->setFlags(task->getFlags());
	Q_EMIT contactIgnored(task->contact()->isIgnored());
}

void ContactList::slotContactAuthorized(const QByteArray& email)
{
	qDebug() << "ContactList::slotContactAuthorized";
	Contact* contact = getContact(email);
	if (contact)
		contact->setAuthorized();
	else
		qDebug() << "can't find authorized contact " << email;
}

void ContactList::load()
{
	qDebug() << "ContactList::load()";
	if (m_account->path().isEmpty())
		return;
	
	if (!m_groups.isEmpty() || !m_contacts.isEmpty())
		return;

	QFile file(m_account->path() + "/contactlist");
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "contactlist file doesn't exist";
		return;
	}
	
	clear();
	
	QDataStream in(&file);
	
	int nGroups;
	quint32 vers;
	in >> vers;
	if (vers != 2)
		return;

	in >> nGroups;
	if (in.status() != QDataStream::Ok)
	{
		file.close();
		Q_EMIT updated();
		return;
	}

	for (int i = 0; i < nGroups; i++)
	{
		ContactGroup* group = new ContactGroup(in);
		if (group->groupType() == ContactGroup::Simple)
		{
			m_groups.append(group);
			Q_EMIT groupAdded(group);
		}
		else
			delete group;
	}

	int i;
	for (i = 0; i < 5; i++)
	{
		bool isCurrGrp;
		in >> isCurrGrp;
		if (isCurrGrp)
		{
			ContactGroup* group = new ContactGroup(in);
			switch (i)
			{
				case 0:
					if (in.status() == QDataStream::Ok)
					{
						delete m_phones;
						m_phones = group;
					}
					break;
				case 1:
					if (in.status() == QDataStream::Ok)
					{
						delete m_conferences;
						m_conferences = group;
					}
					break;
				case 2:
					if (in.status() == QDataStream::Ok)
					{
						delete m_temporary;
						m_temporary = group;
					}
					break;
				case 3:
					if (in.status() == QDataStream::Ok)
					{
						delete m_notAuthorized;
						m_notAuthorized = group;
					}
					break;
				case 4:
					if (in.status() == QDataStream::Ok)
					{
						delete m_notInGroup;
						m_notInGroup = group;
					}
					break;
			}
		}
	}

	if (in.status() != QDataStream::Ok)
	{
		file.close();
		clear();
		return;
	}

	while (!in.atEnd())
	{
		Contact* contact = new Contact(m_account);
		contact->load(in);
		if (in.status() == QDataStream::Ok)
		{
			Contact* cont = findContact(contact->email());
			if (cont)
			{
				qDebug() << "Contact" << cont->email() << "already exists!";
				cont = contact;
				delete contact;
			}
			else
				m_contacts.append(contact);

		}
		else
		{
			delete contact;
			break;
		}
	}

	if (in.status() != QDataStream::Ok)
		clear();

	Q_EMIT updated();
	
	file.close();
}

void ContactList::save() const
{
	if (m_account->path().isEmpty())
		return;

	QFile file(m_account->path() + "/contactlist");
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "can't open file to write contactlist";
		return;
	}

	QDataStream out(&file);

	out << quint32(2); //version cl
	out << m_groups.size();
	for (int i = 0; i < m_groups.size(); i++)
		out << m_groups.at(i);

	if (m_model->hasPhones())
	{
		out << true;
		out << m_phones;
	}
	else
		out << false;
	if (m_model->hasConferences())
	{
		out << true;
		out << m_conferences;
	}
	else
		out << false;
	if (m_model->hasTemporary())
	{
		out << true;
		out << m_temporary;
	}
	else
		out << false;
	if (m_model->hasNotAuthorized())
	{
		out << true;
		out << m_notAuthorized;
	}
	else
		out << false;
	if (m_model->hasNotInGroup())
	{
		out << true;
		out << m_notInGroup;
	}
	else
		out << false;

	for (int i = 0; i < m_contacts.size(); ++i)
		m_contacts.at(i)->save(out);

	file.close();
}

void ContactList::checkOnlineStatus(OnlineStatus status)
{
	if (status == OnlineStatus::offline)
		for (int i = 0; i < m_contacts.size(); i++)
			m_contacts[i]->changeStatus(OnlineStatus::offline);
}

ContactGroup* ContactList::group(quint32 id) const
{
	for (int i = 0; i < m_groups.size(); i++)
		if (m_groups.at(i)->id() == id && m_groups.at(i)->groupType() == ContactGroup::Simple)
			return m_groups.at(i);
	return m_notInGroup;
}

bool ContactList::addTemporaryContactToGroup(Contact* contact, quint32 group)
{
	MRIMClient* mc = m_account->client();
	if (mc == NULL)
		return false;

	Task* task = new Tasks::AddContact(group, contact->email(), contact->nickname(), "", mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(addContactOnServerEnd(quint32, bool)));

	if (!task->exec())
	{
		qDebug() << "Contact::addTemporaryContactToGroup: add contact task returned 0";
		return false;
	}
	return true;
}

bool ContactList::addContactOnServer(quint32 group, const QByteArray & email, const QString & nickname, const QString & authorizationMessage)
{
	Contact* contact = findContact(email);
	if (contact && !contact->isHidden())
		return false;
	
	MRIMClient* mc = m_account->client();
	
	Task* task = new Tasks::AddContact(group, email, nickname, authorizationMessage, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(addContactOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		qDebug() << "add contact task returned 0";
		return false;
	}
	
	return true;
}

void ContactList::addContactOnServerEnd(quint32 status, bool timeout)
{
	if (timeout)
	{
		Q_EMIT addContactOnServerError(tr("Time is out"));
		return;
	}
	else if (status != CONTACT_OPER_SUCCESS)
	{
		Q_EMIT addContactOnServerError(tr("Error"));
		return;
	}

	Tasks::AddContact* task = qobject_cast<Tasks::AddContact*>(sender());

	Contact* contact = findContact(task->email());
	if (contact != NULL)
	{
		qDebug() << "Contact found in" << ((constructing) ? ("tmpContacts") : ("m_contacts"));
		if (contact->isTemporary())
		{
			qDebug() << "addContactOnServerEnd remove temporary contact";
			qDebug() << constructing;
			((constructing) ? (tmpContacts) : (m_contacts)).removeAll(contact);
			Q_EMIT contactRemoved(contact);
		}
		else
		{
			qDebug() << "ContactList::addContactOnServerEnd: strange error: contact added but it is already in contactlist";
			return;
		}
	}

	addContact(task->contactData());
}

bool ContactList::newConferenceOnServer(QString confName, QByteArray owner, QList<QByteArray> members)
{
	qDebug() << "ContactList::newConferenceOnServer" << confName;
	MRIMClient* mc = m_account->client();
	
	Task* task = new Tasks::NewConference(confName, owner, mc, members);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(newConferenceOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		qDebug() << "new conference task returned 0";
		return false;
	}
	
	return true;
}

bool ContactList::addConferenceOnServer(const QByteArray & chat, const QString & confName)
{
	Contact* contact = findContact(chat);
	if (contact)
		return false;

	qDebug() << "ContactList::addConferenceOnServer" << confName << chat;
	MRIMClient* mc = m_account->client();
	
	Task* task = new Tasks::NewConference(confName, chat, mc, QList<QByteArray>());
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(newConferenceOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		qDebug() << "new conference task returned 0";
		return false;
	}
	
	return true;
}

void ContactList::newConferenceOnServerEnd(quint32 status, bool timeout)
{
	qDebug() << "ContactList::newConferenceOnServerEnd";
	if (timeout)
	{
		Q_EMIT newConferenceOnServerError(tr("Time is out"));
		return;
	}
	else if (status != CONTACT_OPER_SUCCESS)
	{
		Q_EMIT newConferenceOnServerError(tr("Error"));
		return;
	}

	Tasks::NewConference* task = qobject_cast<Tasks::NewConference*>(sender());

	qDebug() << "Contact name:" << task->contactData().nick;
	Contact* contact = findContact(task->contactData().email);
	if (contact)
//	{
		contact->update(task->contactData(), contact->group());
/*		if (contact->isTemporary())
		{
			qDebug() << "addContactOnServerEnd remove temporary contact";
			m_contacts.removeAll(contact);
			Q_EMIT contactRemoved(contact);
		}
		else
		{
			qDebug() << "ContactList::addContactOnServerEnd: strange error: contact added but it is already in contactlist";
			return;
		}*/
//	}
	else
		contact = addContact(task->contactData());
}

bool ContactList::addSmsContactOnServer(const QString & nickname, const QStringList & phones)
{
	Contact* contact = findSmsContact(nickname);
	if (contact && !contact->isHidden())
	{
		Q_EMIT addSmsContactOnServerError(tr("Contact %1 already exists").arg(nickname));
		return false;
	}
	
	MRIMClient* mc = m_account->client();
	
	Task* task = new Tasks::AddSmsContact(nickname, phones, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(addSmsContactOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		qDebug() << "add sms contact task returned 0";
		return false;
	}
	
	return true;
}

void ContactList::addSmsContactOnServerEnd(quint32 status, bool timeout)
{
	if (timeout)
	{
		Q_EMIT addSmsContactOnServerError(tr("Time is out"));
		return;
	}
	else if (status != CONTACT_OPER_SUCCESS)
	{
		Q_EMIT addSmsContactOnServerError(tr("Error"));
		return;
	}

	Tasks::AddSmsContact* task = qobject_cast<Tasks::AddSmsContact*>(sender());
	addContact(task->contactData());
}

bool ContactList::addGroupOnServer(const QString& groupName)
{
	for (int i = 0; i < m_groups.size(); i++)
	{
		if (m_groups.at(i)->name() == groupName)
		{
			Q_EMIT addGroupOnServerError(tr("Group %1 already exists.").arg(groupName));
			return false;
		}
	}
	
	MRIMClient* mc = m_account->client();
	
	Task* task = new Tasks::AddGroup(groupName, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(addGroupOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		Q_EMIT addGroupOnServerError(tr("Adding group is currently impossible"));
		return false;
	}
	
	return true;
}

void ContactList::addGroupOnServerEnd(quint32 status, bool timeout)
{
	QString error;
	
	if (timeout)
		error = tr("Time is out");
	else if (status == CONTACT_OPER_ERROR)
		error = tr("Incorrect data");
	else if (status == CONTACT_OPER_INTERR)
		error = tr("Internal error");
	else if (status == CONTACT_OPER_INVALID_INFO)
		error = tr("Invalid group name");
	else if (status == CONTACT_OPER_GROUP_LIMIT)
		error = tr("Number of groups exceeded");
	else if (status != CONTACT_OPER_SUCCESS)
		error = tr("Unknown error");
	
	if (!error.isEmpty())
	{
		Q_EMIT addGroupOnServerError(error);
		return;
	}

	Tasks::AddGroup* task = qobject_cast<Tasks::AddGroup*>(sender());
	addGroup(task->groupId(), CONTACT_FLAG_GROUP, task->groupName());
}

bool ContactList::removeGroupOnServer(ContactGroup* group)
{
	if (!group)
	{
		Q_EMIT removeGroupOnServerError(tr("Unknown group"));
		return false;
	}
	
	qDebug() << "searching for group" << group->name() << (void*)group;
	ContactsIterator it = contactsBegin();
	for (; it != contactsEnd(); it++)
	{
		qDebug() << "current group = " << (*it)->group()->name() << (void*)((*it)->group());
		if ((*it)->group() == group)
			break;
	}

	if (it != contactsEnd())
	{
		Q_EMIT removeGroupOnServerError(tr("Group is not empty"));
		return false;
	}
	
	MRIMClient* mc = m_account->client();
	Tasks::RemoveGroup* task = new Tasks::RemoveGroup(group, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(removeGroupOnServerEnd(quint32, bool)));
	if (!task->exec())
	{
		Q_EMIT removeGroupOnServerError(tr("Removing group is currently impossible"));
		return false;
	}
	
	return true;
}

void ContactList::removeGroupOnServerEnd(quint32 status, bool timeout)
{
	QString error;
	
	if (timeout)
		error = tr("Time is out");
	else if (status == CONTACT_OPER_ERROR)
		error = tr("Incorrect data");
	else if (status == CONTACT_OPER_INTERR)
		error = tr("Internal error");
	else if (status == CONTACT_OPER_INVALID_INFO)
		error = tr("Invalid group name");
	else if (status == CONTACT_OPER_NO_SUCH_USER)
		error = tr("No such group");
	else if (status != CONTACT_OPER_SUCCESS)
		error = tr("Unknown error");
	
	if (!error.isEmpty())
	{
		Q_EMIT removeGroupOnServerError(error);
		return;
	}

	Tasks::RemoveGroup* task = qobject_cast<Tasks::RemoveGroup*>(sender());
	
	ContactGroup* group = task->group();
	m_groups.removeAll(group);
	delete group;

	Q_EMIT groupRemoved(group);
	Q_EMIT updated();
}

bool ContactList::renameGroup(ContactGroup * group, const QString & newName)
{
	if (!group)
	{
		Q_EMIT renameGroupError(tr("Unknown group"));
		return false;
	}
	
	MRIMClient* mc = m_account->client();
	Tasks::RenameGroup* task = new Tasks::RenameGroup(group, newName, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(renameGroupEnd(quint32, bool)));
	if (!task->exec())
	{
		Q_EMIT renameGroupError(tr("Renaming group is currently impossible"));
		return false;
	}
	
	return true;
}

void ContactList::renameGroupEnd(quint32 status, bool timeout)
{
	QString error;
	
	if (timeout)
		error = tr("Time is out");
	else if (status == CONTACT_OPER_ERROR)
		error = tr("Incorrect data");
	else if (status == CONTACT_OPER_INTERR)
		error = tr("Internal error");
	else if (status == CONTACT_OPER_INVALID_INFO)
		error = tr("Invalid group name");
	else if (status == CONTACT_OPER_NO_SUCH_USER)
		error = tr("No such group");
	else if (status != CONTACT_OPER_SUCCESS)
		error = tr("Unknown error");
	
	if (!error.isEmpty())
	{
		Q_EMIT renameGroupError(error);
		return;
	}

	Tasks::RenameGroup* task = qobject_cast<Tasks::RenameGroup*>(sender());
	
	ContactGroup* group = task->group();
	QString name = task->name();
	group->setName(name);

	Q_EMIT groupRenamed(group);
	Q_EMIT updated();
}

void ContactList::setLastSmsFrom(const QByteArray& number, Contact* c)
{
	qDebug() << "setLastSmsFrom" << number.right(11) << c->nickname();
	lastSmsFrom[number.right(11)] = c;
}

void ContactList::update()
{
	Q_EMIT updated();
}

