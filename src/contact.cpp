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

#include "contact.h"
#include "account.h"

#include <QDir>
#include <QDebug>
#include <QUrl>

#include "mrimclient.h"
#include "proto.h"
#include "taskchangegroup.h"
#include "tasksetvisibility.h"
#include "taskrenamecontact.h"
#include "taskrequestcontactinfo.h"
#include "taskchangephones.h"
#include "contactlist.h"

Contact::Contact(const ContactData& contactData, ContactGroup* group, Account* account)
	: data(contactData), m_group(group), m_account(account)
{
}

Contact::Contact(Account* account)
	: m_account(account)
{
}

void Contact::update(const ContactData & contactData, ContactGroup * group)
{
	qDebug() << "Contact::update email = " << data.email << "contactData email = " << contactData.email;
	qDebug() << "Nickname = " << contactData.nick;
	data = contactData;
	qDebug() << "New nickname = " << data.nick;
	qDebug() << "New group" << m_group << group;
	m_group = group;

	if (isConference())
		m_activeConference = true;
	emit statusChanged(status());
	emit renamed(data.nick);
	emit phonesChanged();
}

QString Contact::path() const
{
	QDir dir(m_account->path());
	if (!dir.exists(email()))
		if (!dir.mkdir(email()))
		{
			qDebug() << "Can\'t create contact dir";
			return "";
		}
	dir.cd(email());
	return dir.absolutePath();
}

void Contact::changeStatus(OnlineStatus newStatus)
{
	if (data.status != newStatus)
	{
		data.status = newStatus;
		emit statusChanged(status());
	}
}

void Contact::contactTyping()
{
	emit typing();
}

bool Contact::changeGroup(quint32 group)
{
	qDebug() << "group = " << m_group << ", new group = " << group;
	
	if (isTemporary())
	{
		return m_account->contactList()->addTemporaryContactToGroup(this, group);
	}

	if (m_group->id() == group)
		return false;
	
	MRIMClient* mc = m_account->client();
	if (mc == NULL)
		return false;
	
	Task* task = new Tasks::ChangeGroup(this, group, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotChangeGroupResult(quint32, bool)));

	return task->exec();
}

void Contact::slotChangeGroupResult(quint32 status, bool timeout)
{
	if (timeout == true || status != CONTACT_OPER_SUCCESS)
	{
		emit groupChanged(false);
		return;
	}
	
	Tasks::ChangeGroup* task = qobject_cast<Tasks::ChangeGroup*>(sender());
	if (!task)
	{
		qDebug() << "slotChangeGroupResult strange sender";
		return;
	}
	
	setGroup(task->getGroup());
	emit groupChanged(true);
}

void Contact::setAuthorized()
{
	if (data.internalFlags & CONTACT_INTFLAG_NOT_AUTHORIZED)
	{
		data.internalFlags = data.internalFlags & ~CONTACT_INTFLAG_NOT_AUTHORIZED;
		emit statusChanged(status());
	}
}

OnlineStatus Contact::status() const
{
	if (data.internalFlags & CONTACT_INTFLAG_NOT_AUTHORIZED)
		return OnlineStatus::unauthorized;
	else
		return data.status;
}

void Contact::rename(const QString& newNickname)
{
	if (nickname() == newNickname)
		return;
	
	if (!m_group)
	{
		data.nick = newNickname;
		emit renamed(nickname());
		return;
	}

	MRIMClient* mc = m_account->client();
	if (mc == NULL)
		return;
	
	Task* task = new Tasks::RenameContact(this, newNickname, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotRenameResult(quint32, bool)));

	task->exec();
}

void Contact::slotRenameResult(quint32 status, bool timeout)
{
	if (timeout == true || status != CONTACT_OPER_SUCCESS)
	{
		qDebug() << "Contact: renaming error";
		return;
	}
	
	Tasks::RenameContact* task = qobject_cast<Tasks::RenameContact*>(sender());
	if (!task)
	{
		qDebug() << "slotRenameResult strange sender";
		return;
	}
	
	data.nick = task->getNickname();
	emit renamed(nickname());
}

quint32 Contact::setMyVisibility(bool alwaysVisible, bool alwaysInvisible)
{
	MRIMClient* mc = m_account->client();
	if (mc == NULL)
		return 0;
	
	Task* task = new Tasks::SetVisibility(this, alwaysVisible, alwaysInvisible, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(slotSetVisibilityResult(quint32, bool)));

	return task->exec();
}

void Contact::slotSetVisibilityResult(quint32 status, bool timeout)
{
	if (timeout == true || status != CONTACT_OPER_SUCCESS)
	{
		qDebug() << "Contact: set visibility error";
		return;
	}
	
	Tasks::SetVisibility* task = qobject_cast<Tasks::SetVisibility*>(sender());
	if (!task)
	{
		qDebug() << "slotSetVisibilityResult strange sender";
		return;
	}
	
	quint32 newFlags = task->flags();
	if (data.flags != newFlags)
	{
		data.flags = newFlags;
		emit visibilityChanged();
	}
}

Contact::~Contact()
{
	qDebug() << "Contact::~ Contact() email = " << email() << (void*)this;

	emit destroyed(this);
}

void Contact::setGroup(quint32 id)
{
	m_group = m_account->contactList()->group(id);
}

bool Contact::isPhone() const
{
	return (data.flags & CONTACT_FLAG_PHONE);
}

bool Contact::isIgnored() const
{
	return (data.flags & CONTACT_FLAG_IGNORE);
}

bool Contact::isTemporary() const
{
	qDebug() << m_group;
	return (m_group == 0) && !isPhone() && !(isConference() && m_activeConference);
}

bool Contact::isNotAuthorized() const
{
	return (data.internalFlags & CONTACT_INTFLAG_NOT_AUTHORIZED);
}

void Contact::load(QDataStream& stream)
{
	data.load(stream);
	setGroup(data.group);
}

void Contact::save(QDataStream & stream) const
{
	if (isTemporary())
		return;
	
	data.save(stream);
}

bool Contact::isHidden() const
{
	return data.flags & (CONTACT_FLAG_SHADOW | CONTACT_FLAG_REMOVED);
}

bool Contact::isConference() const
{
	return data.isConference();
}

void Contact::updateStatus()
{
	Task* task = new Tasks::RequestContactInfo(data.email, m_account->client(), this);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(showContactInfo(quint32, bool)));
	task->exec();
}

void Contact::updateStatusEnd(quint32 status, bool timeout)
{
	qDebug() << "Contact::updateStatusEnd";
	
	if (timeout || status != MRIM_ANKETA_INFO_STATUS_OK)
	{
		qDebug() << "can't get contact info for" << data.email;
		return;
	}
	
	Tasks::RequestContactInfo* task = qobject_cast<Tasks::RequestContactInfo*>(sender());
	changeStatus(task->getContactInfo().onlineStatus());
}

bool Contact::hasPhone(QByteArray phoneNumber) const
{
	if (!phoneNumber.startsWith('+'))
		phoneNumber.prepend('+');

	return data.phones.contains(phoneNumber);
}

void Contact::changePhones(const QStringList& phones)
{
	if (phones == data.phones)
		return;
	
	MRIMClient* mc = m_account->client();
	if (mc == NULL)
		return;
	
	Task* task = new Tasks::ChangePhones(this, phones, mc);
	connect(task, SIGNAL(done(quint32, bool)), this, SLOT(changePhonesEnd(quint32, bool)));

	task->exec();
}

void Contact::changePhonesEnd(quint32 status, bool timeout)
{
	if (timeout == true || status != CONTACT_OPER_SUCCESS)
	{
		qDebug() << "changing phones error: timeout = " << timeout << ", status = " << status;
		return;
	}
	
	Tasks::ChangePhones* task = qobject_cast<Tasks::ChangePhones*>(sender());
	if (!task)
	{
		qDebug() << "changePhonesEnd strange sender";
		return;
	}
	
	data.phones = task->phones();
	emit phonesChanged();
}

QIcon Contact::chatWindowIcon(QString type)
{
	qDebug() << "Contact" << nickname() << isPhone() << isConference();
	if (type == "")
	{
		if (isIgnored())
			return QIcon(":icons/cl_ignore_contact.png");
		if (isConference())
			return QIcon(":icons/msg_conference.png");
		else if (isPhone())
			return QIcon(":icons/phone.png");
		else
			return status().statusIcon();
	}
	else
	{
		QIcon currentIcon;
		currentIcon.addFile(":icons/" + type + "_32x32.png");
		currentIcon.addFile(":icons/" + type + "_16x16.png");
		return currentIcon;
	}
}

void Contact::setFlags(quint32 f)
{
	if (!(data.flags & CONTACT_FLAG_IGNORE & f))
	{
		data.flags = f;
		emit ignoredChanged();
		return;
	}
	data.flags = f;
}
