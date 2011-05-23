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

#ifndef MRIMCLIENT_H
#define MRIMCLIENT_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDateTime>

#include "onlinestatus.h"

class Account;
class Contact;
class ContactGroup;
class ContactList;
class Message;
class FileMessage;
class QTextCodec;

class MRIMClientPrivate;

class MRIMClient : public QObject
{
Q_OBJECT
	friend class MRIMClientPrivate;
public:
	MRIMClient(Account* a);
	~MRIMClient();
	
	Account* account();
	uint getPingTime() const;
	
	void disconnectFromServer();
	
	typedef QMap<QString, QStringList> ContactsInfo;
	typedef QMap<quint32, QByteArray> SearchParams;
signals:
	void loginRejected(QString reason);
	void loginAcknowledged(OnlineStatus status);
	void disconnectedFromServer();
	void contactStatusChanged(OnlineStatus status, QByteArray email);
	void contactTyping(QByteArray email);
	void messageReceived(QByteArray from, Message* msg);
	//void fileReceived(FileMessage* fmsg);
	void fileReceived(QByteArray from, quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips);
	void messageStatus(quint32 id, quint32 status);
	void contactModified(quint32 id, quint32 status);
	void contactAdded(quint32 id, quint32 status, quint32 contactId);
	void conferenceBegan(quint32 seq, quint32 status, quint32 contactId, QByteArray & chatAgent);
	void contactInfoReceived(quint32 id, quint32 status, MRIMClient::ContactsInfo info, quint32 maxRows, quint32 serverTime);
	void accountInfoReceived(quint32 status, MRIMClient::ContactsInfo info, quint32 maxRows, quint32 serverTime);
	void contactAuthorizedMe(const QByteArray& email);
	void loggedOut(quint32 reason);
	void newNumberOfUnreadLetters(quint32 nLetters);
	void contactAsksAuthorization(const QByteArray& email, const QString& nickname, const QString& message);
	void receivedMPOPSession(quint32 seq, quint32 status, QByteArray session);
	void newLetter(QString sender, QString subject, QDateTime dateTime);
	void smsAck(quint32 seq, quint32 status);
	void fileTransferAck(quint32 status, QByteArray email, quint32 sessionId, QByteArray mirrorIps);
	void proxy(QByteArray email, quint32 idRequest, quint32 dataType, QByteArray filesAnsi, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	void proxyAck(quint32 status, QByteArray email, quint32 id_request, quint32 dataType, QByteArray filesAnsi, QByteArray ips, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	void microblogChanged(QByteArray, QString);
	void conferenceAsked(const QByteArray& confChat, const QString& confName);
	void connectError(QString errorMessage);
	void conferenceClAddContact(QByteArray& contact);
	
public slots:
	void connectToServer(OnlineStatus status);
	quint32 requestContactInfo(QByteArray email);
	quint32 searchContacts(const SearchParams& params);
	void changeStatus(OnlineStatus newStatus);
	quint32 sendMessage(QByteArray email, const Message* message);
	quint32 broadcastMessage(const Message* message, QList<QByteArray> receivers);
	quint32 sendRtfMessage(QByteArray email, QString text, QByteArray message);
	void sendTyping(QByteArray email);
	quint32 changeContactGroup(quint32 groupID, Contact* c);
	quint32 addContact(quint32 group, const QString& nickname, const QByteArray& email, const QString& authorizationMessage);
	quint32 addConference(QString confName, QByteArray owner, QList<QByteArray> members);
	quint32 addGroup(QString& name);
	quint32 addSmsContact(const QString& nickname, const QStringList& phones);
	quint32 removeContact(Contact* c);
	quint32 removeGroup(ContactGroup* group);
	quint32 askAuthorization(const QByteArray& email, const QString& message);
	quint32 setVisibility(bool alwaysVisible, bool alwaysInvisible, Contact* contact);
	quint32 renameContact(const QString& nickname, Contact* contact);
	quint32 ignoreContact(const quint32 flags, Contact* contact);
	quint32 renameGroup(ContactGroup* group, QString name);
	void authorizeContact(const QByteArray& email);
	quint32 getMPOPSession();
	quint32 sendSms(QByteArray number, const QString& text);
	quint32 changeContactPhones(Contact* contact, const QStringList& phones);
	quint32 sendFile(FileMessage* fileMessage);
	quint32 sendFileAck(quint32 status, QByteArray email, quint32 sessionId, QByteArray ips);
	void sendProxy(FileMessage* fmsg, quint32 dataType);
	void sendProxyAck(FileMessage* fmsg, quint32 status, quint32 dataType, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	quint32 sendMicrotext(const QString& text);
	quint32 conferenceClLoad(const QByteArray& conferenceAddress);
	
private:
	MRIMClientPrivate* p;
};

#endif
