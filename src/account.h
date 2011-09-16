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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QSettings>
#include <QPointer>

#include "onlinestatus.h"
#include "gui/onlinestatuseseditor.h"
#include "chatsmanager.h"
#include "protocol/mrim/mrimclient.h"
#include "contactlist.h"

class Account : public QObject
{
Q_OBJECT
	friend class MRIMClient;
public:
	Account(QByteArray email, QByteArray password, QObject* parent);
	~Account();
	
	void reset(QByteArray email, QByteArray password);
	ContactList* contactList() { return m_contactList; }
	QByteArray email() const { return m_email; }
	QByteArray password() const { return m_password; }

	void setTotalMessages(const QString& totalMessages);
	void setUnreadMessages(const QString& unreadMessages);
	void setNickname(QString nickname);
	void setStatusText(QString statusText);

	QString nickname() const { return m_nickname; }
	QString firstName() const { return m_info["FirstName"].first(); }
	QString path() const;
	QString avatarsPath() const;
	MRIMClient* client() { return m_client; }
	OnlineStatus onlineStatus() const { return m_onlineStatus; }
	ChatsManager* chatsManager() const { return m_chatsManager; }
	uint unreadLetters() const { return m_unreadMessages; }

	QSettings* settings() { return m_settings; }
	OnlineStatusesEditor* onlineStatusesEditor() { return m_onlineStatusesEditor; }

	void setAutoAway(bool on);
public Q_SLOTS:
	void setOnlineStatus(OnlineStatus status, qint32 id = -1);
	void saveOnlineStatus(OnlineStatus st);
	void savePassword(bool really);
	void showOnlineStatusesEditor();
	void extendedStatusChanged(qint32 id, OnlineStatus status);

Q_SIGNALS:
	void onlineStatusChanged(OnlineStatus status);
	void nicknameChanged();
	void totalMessagesChanged(const QString& totalMessages);
	void unreadLettersChanged(uint n);
	void statusChanged(QByteArray email, QString text, QDateTime dt);
	void extendedStatusesChanged();
	
private Q_SLOTS:
	void slotLoggedIn(OnlineStatus status);
	void slotLoginRejected(QString reason);
	void slotDisconnectedFromServer();

	void setUnreadLetters(quint32 n);
	void slotNewLetter();

	void checkAccountInfo(quint32 status, MRIMClient::ContactsInfo info, quint32 maxRows, quint32 serverTime);

private:
	QByteArray m_email;
	QByteArray m_password;
	
	uint m_totalMessages;
	uint m_unreadMessages;
	QString m_nickname;
	MRIMClient::ContactsInfo m_info;
	
	OnlineStatus m_onlineStatus;
	qint32 m_pointerOnlineStatus;
	
	QPointer<ContactList> m_contactList;
	QPointer<MRIMClient> m_client;
	QPointer<ChatsManager> m_chatsManager;
	QPointer<QSettings> m_settings;
	QPointer<OnlineStatusesEditor> m_onlineStatusesEditor;

	bool m_isInAutoAway;
	OnlineStatus m_statusBeforeAutoAway;
};

#endif
