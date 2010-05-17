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

#ifndef MRIMCLIENTPRIVATE_H
#define MRIMCLIENTPRIVATE_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class MRIMClient;
class Account;

class MRIMClientPrivate : public QObject
{
Q_OBJECT
	friend class MRIMClient;
public:
	MRIMClientPrivate(Account* a, MRIMClient* parent);
	~MRIMClientPrivate();
	
private slots:
	void readData();
	void slotDisconnectedFromServer();
	void slotConnectedToServer();
	void ping();

	void slotSocketStateChanged(QAbstractSocket::SocketState);
	void slotSocketError(QAbstractSocket::SocketError);
	
private:
	QByteArray getLogin(QByteArray email) const
	{
		int i = email.indexOf('@');
		return email.left(i);
	}
	
	QByteArray getDomain(QByteArray email) const
	{
		int i = email.indexOf('@');
		return email.right(email.length() - i - 1);
	}

	void init();
	quint32 sendPacket(quint32 msgtype, QByteArray data = QByteArray());
	void processPacket(QByteArray header, QByteArray data);
	
	QByteArray packRtf(QByteArray rtf);
	bool unpackRtf(const QByteArray& packedRtf, QByteArray* rtf, quint32* backgroundColor);
	
	QByteArray packAuthorizationMessage(const QString& message);
	void unpackAuthorizationMessage(const QByteArray& message, QString& nickname, QString& unpackedMessage);
	
	void processHelloAck(QByteArray data);
	void processLoginAcknowledged(QByteArray data);
	void processLoginRejected(QByteArray data);
	void processUserInfo(QByteArray data);
	void processContactList2(QByteArray data);
	void processAnketaInfo(QByteArray data, quint32 msgseq);
	void processUserStatus(QByteArray data);
	void processMessageAck(QByteArray data);
	void processMessageStatus(QByteArray data, quint32 msgseq);
	void processModifyContactAck(QByteArray data, quint32 msgseq);
	void processAddContactAck(QByteArray data, quint32 msgseq);
	void processAuthorizeAck(QByteArray data);
	void processOfflineMessageAck(QByteArray data);
	void processLogout(QByteArray data);
	void processConnectionParams(QByteArray data);
	void processMailBoxStatus(QByteArray data);
	void processNewMail(QByteArray data);
	void processFileTransfer(QByteArray data);
	void processFileTransferAck(QByteArray data);
	void processMPOPSession(QByteArray data, quint32 msgseq);
	void processSmsAck(QByteArray data, quint32 msgseq);
	
	void processProxy(QByteArray data, quint32 msgseq);
	void processProxyAck(QByteArray data, quint32 msgseq);

	void processStatusChanged(QByteArray data);

	MRIMClient* q;

	QTextCodec* codec;
	quint32 sequence;
	QTcpSocket socket;
	QByteArray messageHeader;
	QTimer* pingTimer;
	uint pingTime;
	bool headerReceived;
	uint dataSize;
	Account* account;
	quint32 currentStatus;
	quint32 newStatus;
	bool gettingAddress;
	bool manualDisconnect;
};

#endif
