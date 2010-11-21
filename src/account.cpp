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

#include "account.h"
#include "contactlist.h"

#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QTextCodec>

#include "proto.h"
#include "mrimclient.h"
#include "resourcemanager.h"
#include "chatsmanager.h"
#include "historymanager.h"
#include "audio.h"

Account::Account(QByteArray email, QByteArray password, QObject* parent)
	: QObject(parent)
{
	m_email = email;
	m_nickname = m_email;
	m_password = password;

	m_onlineStatus = OnlineStatus::offline;
	m_isInAutoAway = false;
	m_statusBeforeAutoAway = OnlineStatus::offline;

	m_contactList = new ContactList(this);
	m_client = new MRIMClient(this);
	m_chatsManager = new ChatsManager(this);

	QString accountPath = path();
	qDebug() << "accountPath =" << accountPath;
	if (!accountPath.isEmpty())
		m_settings = new QSettings(accountPath + "/settings.txt", QSettings::IniFormat, this);

	HistoryManager* hm = new HistoryManager(this);
	connect(m_chatsManager, SIGNAL(sessionCreated(ChatSession*)), hm, SLOT(createLogger(ChatSession*)));

	connect(m_client, SIGNAL(loginAcknowledged(OnlineStatus)), this, SLOT(slotLoggedIn(OnlineStatus)));
	connect(m_client, SIGNAL(loginRejected(QString)), this, SLOT(slotLoginRejected(QString)));
	connect(m_client, SIGNAL(disconnectedFromServer()), this, SLOT(slotDisconnectedFromServer()));
	
	connect(m_client, SIGNAL(contactStatusChanged(OnlineStatus, QByteArray)), m_contactList, SLOT(changeContactStatus(OnlineStatus, QByteArray)));
	connect(m_client, SIGNAL(contactTyping(QByteArray)), m_contactList, SLOT(contactTyping(QByteArray)));
	connect(m_client, SIGNAL(contactAuthorizedMe(const QByteArray&)), m_contactList, SLOT(slotContactAuthorized(const QByteArray&)));
	connect(m_client, SIGNAL(newNumberOfUnreadLetters(quint32)), this, SLOT(setUnreadLetters(quint32)));
}

void Account::reset(QByteArray email, QByteArray password)
{
	m_password = password;
	
	if (m_email != email)
	{
		m_email = email;
		
		m_contactList->clear();
		
		delete m_settings;
		m_settings = new QSettings(path() + "/settings.txt", QSettings::IniFormat, this);
	}
}

Account::~Account()
{
	qDebug() << "Account::~Account()";
	delete m_chatsManager;
	delete m_client;
	delete m_contactList;
}

QString Account::path() const
{
	QString basePath = theRM.basePath();
	if (basePath.isEmpty()) return "";

	QDir dir(basePath);

	if (!dir.exists(basePath))
		return "";

	if (!dir.exists(email()))
		dir.mkdir(email());

	dir.cd(email());

	return dir.absolutePath();
}

QString Account::avatarsPath() const
{
	static const QString avatarsDirname = "avatars";
	
	QString basePath = path();
	if (basePath.isEmpty()) return "";
	
	QDir dir(basePath);
	
	if (!dir.exists(avatarsDirname))
		if (!dir.mkdir(avatarsDirname))
		{
			qDebug() << "Can\'t create avatars subdir";
			return "";
		}
	dir.cd(avatarsDirname);
	
	return dir.absolutePath();
}

void Account::setOnlineStatus(OnlineStatus newStatus)
{
	qDebug() << "Account::setOnlineStatus: onlineStatus = " << m_onlineStatus.statusDescr() << ", newStatus = " << newStatus.statusDescr();
	
	if (m_onlineStatus == newStatus || newStatus == OnlineStatus::connecting)
		return;
	
	if (newStatus == OnlineStatus::offline)
	{
		qDebug() << "newStatus = offline";
		m_client->disconnectFromServer();
	}
	else if (m_onlineStatus == OnlineStatus::offline)
	{
		m_onlineStatus = OnlineStatus::connecting;
		m_contactList->load();
		emit onlineStatusChanged(m_onlineStatus);
		m_client->connectToServer(newStatus);
	}
	else if (m_onlineStatus == OnlineStatus::connecting)
	{
		m_client->disconnectFromServer();
		emit onlineStatusChanged(m_onlineStatus);
		m_client->connectToServer(newStatus);
	}
	else
	{
		m_onlineStatus = newStatus;
		if (m_isInAutoAway)
			m_isInAutoAway = false;
		m_client->changeStatus(m_onlineStatus);
		emit onlineStatusChanged(m_onlineStatus);
	}
	if (m_onlineStatus.connected())
		theRM.saveOnlineStatus(m_email, m_onlineStatus);
}

void Account::slotDisconnectedFromServer()
{
	qDebug() << "Account.slotDisconnectedFromServer";
	if (m_onlineStatus == OnlineStatus::offline)
		return;
	
	m_onlineStatus = OnlineStatus::offline;
	m_isInAutoAway = false;
	emit onlineStatusChanged(m_onlineStatus);
}

void Account::slotLoggedIn(OnlineStatus status)
{
	QString basePath = theRM.basePath();
	if (!basePath.isEmpty())
	{
		QDir dir(basePath);
	
		if (!dir.exists(email()))
		{
			if (!dir.mkdir(email()))
				qDebug() << "Can\'t create " << email() << " subdir";
			else if (m_settings.isNull())
				m_settings = new QSettings(path() + "/settings.txt", QSettings::IniFormat, this);
		}
	}

	qDebug() << "status" << status.id() << "(" << status.statusDescr() << ")";
	if (m_onlineStatus != status)
	{
		m_onlineStatus = status;
		emit onlineStatusChanged(m_onlineStatus);
	}
}

void Account::slotLoginRejected(QString reason)
{
	qDebug() << "login rejected: " << reason;
	m_client->disconnectFromServer();
}

void Account::setUnreadLetters(quint32 n)
{
	m_unreadMessages = n;
	emit unreadLettersChanged(m_unreadMessages);
}

void Account::setAutoAway(bool on)
{
	if (on)
	{
		if (m_isInAutoAway || m_onlineStatus != OnlineStatus::online)
			return;
		
		m_statusBeforeAutoAway = m_onlineStatus;
		setOnlineStatus(OnlineStatus::away);
		m_isInAutoAway = true;
	}
	else
	{
		if (!m_isInAutoAway || m_onlineStatus != OnlineStatus::away)
			return;

		setOnlineStatus(m_statusBeforeAutoAway);
		m_isInAutoAway = false;
	}
}

void Account::setTotalMessages(const QString& totalMessages)
{
	qDebug() << "Account::setTotalMessages = " << totalMessages;
	m_totalMessages  = totalMessages.toUInt();

	emit totalMessagesChanged(totalMessages);
}

void Account::setUnreadMessages(const QString& unreadMessages)
{
	qDebug() << "Account::setUnreadMessages" << unreadMessages;
	m_unreadMessages = unreadMessages.toUInt();

	if (unreadMessages.toUInt() > 0)
		theRM.getAudio()->play(STLetter);

	emit unreadLettersChanged(m_unreadMessages);
}

void Account::setNickName(QString nickname)
{
	qDebug() << "Account::setNickName" << nickname;

	m_nickname = nickname;

	emit nicknameChanged();
}

void Account::setStatusText(QString statusText)
{
	qDebug() << "Account::setStatusText" << statusText;
	emit statusChanged(statusText);
}
