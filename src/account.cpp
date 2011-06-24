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

#include "protocol/mrim/proto.h"
#include "protocol/mrim/mrimclient.h"
#include "resourcemanager.h"
#include "chatsmanager.h"
#include "historymanager.h"
#include "audio.h"
#include "gui/centerwindow.h"

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
	if (!accountPath.isEmpty())
		m_settings = new QSettings(accountPath + "/settings.txt", QSettings::IniFormat, this);

	m_pointerOnlineStatus = m_settings->value("Statuses/statusPointer", -1).toLongLong();
	if (m_pointerOnlineStatus > theRM.maxDefaultStatuses - 1)
		m_pointerOnlineStatus = theRM.maxDefaultStatuses - 1;

	HistoryManager* hm = new HistoryManager(this);
	connect(m_chatsManager, SIGNAL(sessionCreated(ChatSession*)), hm, SLOT(createLogger(ChatSession*)));

	connect(m_client, SIGNAL(loginAcknowledged(OnlineStatus)), this, SLOT(slotLoggedIn(OnlineStatus)));
	connect(m_client, SIGNAL(loginRejected(QString)), this, SLOT(slotLoginRejected(QString)));
	connect(m_client, SIGNAL(disconnectedFromServer()), this, SLOT(slotDisconnectedFromServer()));
	
	connect(m_client, SIGNAL(contactStatusChanged(OnlineStatus, QByteArray)), m_contactList, SLOT(changeContactStatus(OnlineStatus, QByteArray)));
	connect(m_client, SIGNAL(contactTyping(QByteArray)), m_contactList, SLOT(contactTyping(QByteArray)));
	connect(m_client, SIGNAL(contactAuthorizedMe(const QByteArray&)), m_contactList, SLOT(slotContactAuthorized(const QByteArray&)));
	connect(m_client, SIGNAL(newNumberOfUnreadLetters(quint32)), this, SLOT(setUnreadLetters(quint32)));
	connect(m_client, SIGNAL(newLetter(QString, QString, QDateTime)), this, SLOT(slotNewLetter()));

	connect(m_client, SIGNAL(accountInfoReceived(quint32, MRIMClient::ContactsInfo, quint32, quint32)), this, SLOT(checkAccountInfo(quint32, MRIMClient::ContactsInfo, quint32, quint32)));
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
	qDebug() << Q_FUNC_INFO;
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

void Account::setOnlineStatus(OnlineStatus newStatus, qint32 id)
{
	qDebug() << "Account::setOnlineStatus: onlineStatus = " << m_onlineStatus.statusDescr() << ", newStatus = " << newStatus.statusDescr();

	if (m_onlineStatus.connected())
		m_pointerOnlineStatus = id;

	if ((m_onlineStatus == newStatus && m_onlineStatus.statusDescr() == newStatus.statusDescr()) || newStatus == OnlineStatus::connecting)
		return;
	
	if (newStatus == OnlineStatus::offline)
	{
		qDebug() << "newStatus = offline";
		m_client->disconnectFromServer();
		emit onlineStatusChanged(newStatus);
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
		saveOnlineStatus(m_onlineStatus);
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

void Account::slotNewLetter()
{
	m_unreadMessages++;
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

	emit unreadLettersChanged(m_unreadMessages);
}

void Account::setNickname(QString nickname)
{
	qDebug() << "Account::setNickName" << nickname;

	m_nickname = nickname;

	emit nicknameChanged();
}

void Account::setStatusText(QString statusText)
{
	qDebug() << "Account::setStatusText" << statusText;
	emit statusChanged(email(), statusText, QDateTime::currentDateTime());
}

void Account::saveOnlineStatus(OnlineStatus st)
{
	m_settings->setValue("Statuses/lastOnlineStatus", st.id());
	m_settings->setValue("Statuses/lastOnlineStatusDescr", st.statusDescr());

	if (theRM.onlineStatuses()->getOnlineStatusInfo(st.id())->builtIn() == "1")
		m_settings->remove("Statuses/statusPointer");
	else
	{
		if (m_pointerOnlineStatus == -1)
		{
			OnlineStatus tmp;
			int i;
			for (i = 0; i <= m_settings->value("Statuses/count", theRM.maxDefaultStatuses - 1).toInt(); i++)
			{
				if (i == 10)
					break;
				if (!m_settings->value("Statuses/statuschecked" + QByteArray::number(i), (i < theRM.minDefaultStatuses) ? true : false).toBool())
				{
					i++;
					break;
				}
				if (m_settings->value("Statuses/statusid" + QByteArray::number(i), tmp.getDefIdStatus(i)).toByteArray() == st.id() &&
				 m_settings->value("Statuses/statusdescr" + QByteArray::number(i), tmp.getDefDescrStatus(i)).toString() == st.statusDescr())
				{
					m_settings->setValue("Statuses/lastOnlineStatus", st.id());
					i++;
					break;
				}
			}
			m_pointerOnlineStatus = i - 1;
		}
		if (m_settings->value("Statuses/count", theRM.minDefaultStatuses).toInt() < m_pointerOnlineStatus + 1)
			m_settings->setValue("Statuses/count", m_pointerOnlineStatus + 1);

		m_settings->setValue("Statuses/statuschecked" + QByteArray::number(m_pointerOnlineStatus), true);
		m_settings->setValue("Statuses/statusid" + QByteArray::number(m_pointerOnlineStatus), st.id());
		m_settings->setValue("Statuses/statusdescr" + QByteArray::number(m_pointerOnlineStatus), st.statusDescr());

		m_settings->setValue("Statuses/statusPointer", m_pointerOnlineStatus);
	}
}

void Account::showOnlineStatusesEditor()
{
	if (m_onlineStatusesEditor)
		return;
	qDebug() << "Statuses editor isn't exists";
	m_onlineStatusesEditor = new OnlineStatusesEditor(this);
	centerWindow(m_onlineStatusesEditor);

	connect(m_onlineStatusesEditor, SIGNAL(statusesChanged()), this, SIGNAL(extendedStatusesChanged()));
	connect(m_onlineStatusesEditor, SIGNAL(statusChanged(qint32, OnlineStatus)), this, SLOT(extendedStatusChanged(qint32, OnlineStatus)));

	m_onlineStatusesEditor->show();
}

void Account::extendedStatusChanged(qint32 id, OnlineStatus status)
{
	qDebug() << "Account::extendedStatusChanged" << id << m_pointerOnlineStatus;

	if (id == m_pointerOnlineStatus)
		setOnlineStatus(status, id);
}

void Account::checkAccountInfo(quint32 status, MRIMClient::ContactsInfo info, quint32 maxRows, quint32 serverTime)
{
	qDebug() << Q_FUNC_INFO;
	m_info = info;
}

void Account::savePassword(bool really)
{
	if (really)
		m_settings->setValue("Account/Password", m_password.toBase64());
	else
		m_settings->remove("Account/Password");
}
