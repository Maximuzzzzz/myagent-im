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

#include "mrimclientprivate.h"

#include <QDebug>

#include <QUrl>
#include <QTextCodec>

#include "account.h"
#include "message.h"
#include "filemessage.h"
#include "zlibbytearray.h"
#include "mrimclient.h"
#include "proto.h"
#include "datetime.h"
#include "audio.h"

MRIMClientPrivate::MRIMClientPrivate(Account* a, MRIMClient* parent)
	: QObject(parent), account(a)
{
	q = parent;
	init();
	codec = QTextCodec::codecForName("cp1251");
}

MRIMClientPrivate::~MRIMClientPrivate()
{
}

void MRIMClientPrivate::init()
{
	pingTimer = new QTimer(this);
	pingTime = 30000;

	currentStatus = STATUS_OFFLINE;

	sequence = 1;
	headerReceived = false;
	dataSize = 0;

	manualDisconnect = false;

	gettingAddress = false;

	connect(&socket, SIGNAL(connected()), this, SLOT(slotConnectedToServer()));
	connect(&socket, SIGNAL(disconnected()), this, SLOT(slotDisconnectedFromServer()));
	connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
	connect(&socket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(&socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));
	connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
}

quint32 MRIMClientPrivate::sendPacket(quint32 msgtype, QByteArray data)
{
	qDebug() << "Send packet";
	QByteArray headerBA;
	MRIMDataStream header(&headerBA, QIODevice::WriteOnly);

	if (sequence == 0) sequence = 1;

	header << quint32(CS_MAGIC);
	header << quint32(PROTO_VERSION);
	header << quint32(sequence);
	header << quint32(msgtype);
	header << quint32(data.size());
	header << quint32(0);
	header << quint32(0);

	header << quint32(0);
	header << quint32(0);
	header << quint32(0);
	header << quint32(0);

	socket.write(headerBA);
	socket.write(data);

	return sequence++;
}

void MRIMClientPrivate::slotConnectedToServer()
{
	//qDebug() << "MRIMClientPrivate::slotConnectedToServer() currentStatus = " << currentStatus << ", newStatus = " << newStatus << ", gettingAddress = " << gettingAddress;

	if (currentStatus == STATUS_OFFLINE && newStatus != STATUS_OFFLINE)
		if (!gettingAddress)
			sendPacket(MRIM_CS_HELLO);
}

void MRIMClientPrivate::slotDisconnectedFromServer()
{
	//qDebug() << "MRIMClientPrivate slotDisconnectedFromServer, currentStatus = " << currentStatus << ", manualDisconnect = " << manualDisconnect;
	if (currentStatus != STATUS_OFFLINE || !gettingAddress || manualDisconnect)
	{
		pingTimer->stop();
		currentStatus = STATUS_OFFLINE;
		gettingAddress = false;
		manualDisconnect = false;
		emit q->disconnectedFromServer();
	}
}

void MRIMClientPrivate::slotSocketStateChanged(QAbstractSocket::SocketState state)
{
	qDebug() << "MRIMClientPrivate::slotSocketStateChanged" << state;
}

void MRIMClientPrivate::slotSocketError(QAbstractSocket::SocketError error)
{
	/*TODO: reconnect on error*/
	qDebug() << "MRIMClientPrivate::slotSocketEror" << error;
}

void MRIMClientPrivate::ping()
{
	sendPacket(MRIM_CS_PING);
}

void MRIMClientPrivate::readData()
{
	qDebug() << "Read data " << socket.bytesAvailable();

	if (gettingAddress)
	{
		QString address = socket.readLine();
		address = address.trimmed();
		QString host = address.section(':', 0, 0);
		uint port = address.section(':', 1, 1).toUInt();
		socket.disconnectFromHost();
		gettingAddress = false;
		qDebug() << "connecting to " << host << ':' << port;
		socket.connectToHost(host, port);

		return;
	}

	for(;;)
	{
		if (!headerReceived)
		{
			//qDebug() << "sizeof(mrim_packet_header_t) = " << sizeof(mrim_packet_header_t);
			if (socket.bytesAvailable() < sizeof(mrim_packet_header_t))
				break;

			messageHeader = socket.read(sizeof(mrim_packet_header_t));
			mrim_packet_header_t* packetHeader =
				reinterpret_cast<mrim_packet_header_t*>(messageHeader.data());

			qDebug() << "Packet Header:";
			qDebug() << messageHeader.toHex();
			dataSize = packetHeader->dlen;
			if (dataSize == 0)
				qDebug() << "!!! dataSize = 0";

			qDebug() << "dataSize from header = " << dataSize;

			headerReceived = true;
		}

		if (socket.bytesAvailable() < dataSize)
		{
			qDebug() << "socket.bytesAvailable = " << socket.bytesAvailable();
			qDebug() << "low bytes";
			break;
		}

		QByteArray data = socket.read(dataSize);
		processPacket(messageHeader, data);
		headerReceived = false;
	}
}

void MRIMClientPrivate::processPacket(QByteArray header, QByteArray data)
{
	qDebug() << "processPacket:";
	qDebug() << header.toHex();

	mrim_packet_header_t* headerData =
		reinterpret_cast<mrim_packet_header_t*>(header.data());
	quint32 msgtype = headerData->msg;
	quint32 msgseq  = headerData->seq;

	qDebug() << "Message#" << msgtype;
	qDebug() << "Message#" << QString::number(msgtype, 16);
	qDebug() << "Data size = " << data.size();
	switch (msgtype)
	{
		case MRIM_CS_HELLO_ACK:
			processHelloAck(data);
			break;
		case MRIM_CS_LOGIN_ACK:
			processLoginAcknowledged(data);
			break;
		case MRIM_CS_LOGIN_REJ:
			processLoginRejected(data);
			break;
		case MRIM_CS_USER_INFO:
			processUserInfo(data);
			break;
		case MRIM_CS_CONTACT_LIST2:
			processContactList2(data);
			break;
		case MRIM_CS_OFFLINE_MESSAGE_ACK:
			processOfflineMessageAck(data);
			break;
		case MRIM_CS_ANKETA_INFO:
			processAnketaInfo(data, msgseq);
			break;
		case MRIM_CS_USER_STATUS:
			processUserStatus(data);
			break;
		case MRIM_CS_MESSAGE_ACK:
			processMessageAck(data);
			break;
		case MRIM_CS_MESSAGE_STATUS:
			processMessageStatus(data, msgseq);
			break;
		case MRIM_CS_LOGOUT:
			processLogout(data);
			break;
		case MRIM_CS_CONNECTION_PARAMS:
			processConnectionParams(data);
			break;
		case MRIM_CS_MAILBOX_STATUS:
			processMailBoxStatus(data);
			break;
		case MRIM_CS_NEW_MAIL:
			processNewMail(data);
			break;
		case MRIM_CS_MODIFY_CONTACT_ACK:
			processModifyContactAck(data, msgseq);
			break;
		case MRIM_CS_ADD_CONTACT_ACK:
			processAddContactAck(data, msgseq);
			break;
		case MRIM_CS_AUTHORIZE_ACK:
			processAuthorizeAck(data);
			break;
		case MRIM_CS_FILE_TRANSFER:
			processFileTransfer(data);
			break;
		case MRIM_CS_FILE_TRANSFER_ACK:
			processFileTransferAck(data);
			break;
		case MRIM_CS_MPOP_SESSION:
			processMPOPSession(data, msgseq);
			break;
		case MRIM_CS_SMS_ACK:
			processSmsAck(data, msgseq);
			break;
		case MRIM_CS_PROXY:
			processProxy(data, msgseq);
			break;
		case MRIM_CS_PROXY_ACK:
			processProxyAck(data, msgseq);
			break;
		default:
			qDebug() << "unknown message";
			/*uchar* chhdr = (uchar*)header.data();
			for (int i = 0; i < sizeof(mrim_packet_header_t); i++)
				qDebug() << QString::number(chhdr[i], 16);
			mrim_packet_header_t* hdr = (mrim_packet_header_t*)header.data();
			qDebug() << "magic = " << QString::number(hdr->magic, 16);
			qDebug() << "proto = " << QString::number(hdr->proto, 16);
			qDebug() << "seq = " << QString::number(hdr->seq, 16);
			qDebug() << "msg = " << QString::number(hdr->msg, 16);
			qDebug() << "dlen = " << QString::number(hdr->dlen, 16);*/
	}
}

QByteArray MRIMClientPrivate::packRtf(QByteArray rtf)
{
	QByteArray pack;
	MRIMDataStream packStream(&pack, QIODevice::WriteOnly);

	packStream << quint32(2);
	packStream << rtf;
	packStream << quint32(4);
	packStream << quint32(0x00FFFFFF);

	QString uncompressError;
	QByteArray zip = ZlibByteArray::compress(pack, ZlibByteArray::Zlib, &uncompressError);
	if (!uncompressError.isEmpty())
		qDebug() << uncompressError;

	return zip.toBase64();
}

bool MRIMClientPrivate::unpackRtf(const QByteArray& packedRtf, QByteArray* rtf, quint32* backgroundColor)
{
	QByteArray tmp = QByteArray::fromBase64(packedRtf);
	QString uncompressError;
	QByteArray unc = ZlibByteArray::uncompress(tmp, ZlibByteArray::Zlib, &uncompressError);
	if (!uncompressError.isEmpty())
		qDebug() << uncompressError;

	MRIMDataStream rtfStream(unc);

	quint32 nLines;
	quint32 bcLPSLen;
	rtfStream >> nLines;
	rtfStream >> *rtf;
	rtfStream >> bcLPSLen >> *backgroundColor;
	qDebug() << "messageAck: lines = " << nLines;
	qDebug() << "messageAck: rtfText = " << *rtf;
	qDebug() << "messageAck: bcLPSLen = " << bcLPSLen;
	qDebug() << "messageAck: backColor = " << QString::number(*backgroundColor, 16);

	return true;
}

QByteArray MRIMClientPrivate::packAuthorizationMessage(const QString & message)
{
	QByteArray msg = codec->fromUnicode(message);
	QString nickname = account->nickname();
	QByteArray nick = codec->fromUnicode(nickname);

	QByteArray packedMessage;
	MRIMDataStream pack(&packedMessage, QIODevice::WriteOnly);

	pack << quint32(2);
	pack << nick;
	pack << msg;

	return packedMessage.toBase64();
}

void MRIMClientPrivate::unpackAuthorizationMessage(const QByteArray& message, QString& nickname, QString& unpackedMessage)
{
	QByteArray data = QByteArray::fromBase64(message);
	MRIMDataStream in(data);

	quint32 sz;
	QByteArray nick;
	QByteArray msg;

	in >> sz;
	in >> nick;
	in >> msg;

	nickname = codec->toUnicode(nick);
	unpackedMessage = codec->toUnicode(msg);
}

void MRIMClientPrivate::processHelloAck(QByteArray data)
{
	qDebug() << "MRIM_CS_HELLO_ACK, bytes = " << socket.bytesAvailable();
	MRIMDataStream in(data);

	quint32 pingPeriod;
	in >> pingPeriod;
	qDebug() << "ping period = " << pingPeriod;
	pingTime = pingPeriod*1000;
	pingTimer->start(pingTime);

	QByteArray ba;
	MRIMDataStream out(&ba, QIODevice::WriteOnly);

	out << QByteArray(account->email());
	out << QByteArray(account->password());
	qDebug() << "MRIMClientPrivate: processHelloAck, status = " << currentStatus;
	out << quint32(newStatus);
	out << QByteArray("myagent-im");
	sendPacket(MRIM_CS_LOGIN2, ba);
}

void MRIMClientPrivate::processUserInfo(QByteArray data)
{
	qDebug() << "MRIM_CS_USER_INFO, bytes = " << data.size();
	MRIMDataStream in(data);

	QByteArray totalMessages, unreadMessages, nick;

	QByteArray param, descr;

	for (int i = 0; i < 3; i++)
	{
		in >> param >> descr;

		if (param == "MESSAGES.TOTAL")
			totalMessages = descr;
		else if (param == "MESSAGES.UNREAD")
			unreadMessages = descr;
		else if (param == "MRIM.NICKNAME")
			nick = descr;
		else
			qDebug() << "something strange with user info";

		qDebug() << param << " " << descr;
	}

	while (in.device()->bytesAvailable())
	{
		in >> param >> descr;
	}

	if (unreadMessages.toUInt() > 0)
		audio.play(STLetter);

	account->setInfo(totalMessages, unreadMessages, codec->toUnicode(nick));
}

void MRIMClientPrivate::processContactList2(QByteArray data)
{
	qDebug() << "MRIM_CS_CONTACT_LIST2, bytes = " << data.size();

	ContactList* contactList = account->contactList();

	MRIMDataStream in(data);

	quint32 status;
	in >> status;
	qDebug() << "MRIM_CS_CONTACT_LIST2 status = " << status;
	if (status != GET_CONTACTS_OK) return;
	quint32 nGroups;
	in >> nGroups;
	qDebug() << "MRIM_CS_CONTACT_LIST2 ngroups = " << nGroups;
	QByteArray groupMask;
	in >> groupMask;
	qDebug() << "group mask = " << groupMask;
	QByteArray contactsMask;
	in >> contactsMask;
	qDebug() << "contacts mask = " << contactsMask;

	if (!contactsMask.startsWith(ContactData::dataMask()))
	{
		qDebug() << "wrong mask, returning";
		return;
	}

	contactList->beginUpdating();
	for (quint32 groupId = 0; groupId < nGroups; groupId++)
	{
		quint32 groupFlags;
		QByteArray groupName;

		in >> groupFlags;
		in >> groupName;

		qDebug() << QString::number(groupFlags, 16) << " " << codec->toUnicode(groupName);

		int nFlags = groupMask.size();
		quint32 uTmpAttr;
		QByteArray sTmpAttr;

		for (int i = 2; i < nFlags; i++)
		{
			char currFlag = groupMask.at(i);
			if (currFlag == 'u')
			{
				in >> uTmpAttr;
				qDebug() << QString::number(uTmpAttr, 16);
			}
			else if (currFlag == 's')
			{
				in >> sTmpAttr;
				qDebug() << sTmpAttr;
			}
			else
				qDebug() << "!!! strange mask flag: " << currFlag;
		}

		if (!(groupFlags & CONTACT_FLAG_REMOVED))
			contactList->addGroup(groupId, groupFlags, codec->toUnicode(groupName));
	}

	quint32 id = 20;

	while (in.device()->bytesAvailable() != 0)
	{
		ContactData contactData(id, in, contactsMask);
		if (in.status() != QDataStream::Ok)
		{
			qDebug() << "constructing contact data error";
			contactList->clear();
			return;
		}
		id++;
		contactList->addContact(contactData);
	}

	contactList->endUpdating();
}

void MRIMClientPrivate::processAnketaInfo(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_ANKETA_INFO, bytes = " << data.size();
	MRIMDataStream in(data);

	MRIMClient::ContactsInfo info;

	quint32 status;
	in >> status;
	if (status != MRIM_ANKETA_INFO_STATUS_OK)
	{
		qDebug() << "Anketa status = " << status;
		emit q->contactInfoReceived(msgseq, status, info, 0, 0);
		return;
	}

	quint32 numFields, maxRows, serverTime;
	in >> numFields >> maxRows >> serverTime;

	qDebug() << "numFields = " << numFields << ", maxRows = " << maxRows << ", serverTime = " << serverTime;

	QVector<QString> fields;

	QByteArray str;
	for (uint i = 0; i < numFields; i++)
	{
		in >> str;
		fields.append(codec->toUnicode(str));
		qDebug() << codec->toUnicode(str);
	}

	while (!in.atEnd())
		for (uint i = 0; i < numFields; i++)
		{
			in >> str;
			info[fields[i]] << codec->toUnicode(str);
			qDebug() << codec->toUnicode(str);
		}

	emit q->contactInfoReceived(msgseq, status, info, maxRows, serverTime);
}

void MRIMClientPrivate::processLoginAcknowledged(QByteArray data)
{
	qDebug() << "MRIM_CS_LOGIN_ACK, bytes = " << data.size();
	currentStatus = newStatus;
	emit q->loginAcknowledged(currentStatus);
}

void MRIMClientPrivate::processLoginRejected(QByteArray data)
{
	qDebug() << "MRIM_CS_LOGIN_REJ, bytes = " << data.size();
	MRIMDataStream in(data);

	QByteArray ba;
	in >> ba;
	QString reason = codec->toUnicode(ba);
	emit q->loginRejected(reason);
}

void MRIMClientPrivate::processLogout(QByteArray data)
{
	qDebug() << "MRIM_CS_LOGOUT, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 reason;

	in >> reason;
	qDebug() << "logout reason = " << reason;

	emit q->loggedOut(reason);
}

void MRIMClientPrivate::processConnectionParams(QByteArray data)
{
	qDebug() << "MRIM_CS_CONNECTION_PARAMS, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 pingPeriod;

	in >> pingPeriod;

	qDebug() << "ping period = " << pingPeriod;
	pingTime = pingPeriod*1000;
	pingTimer->setInterval(pingTime);
}

void MRIMClientPrivate::processMailBoxStatus(QByteArray data)
{
	qDebug() << "MRIM_CS_MAILBOX_STATUS, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 unreadMessages;

	in >> unreadMessages;

	qDebug() << "mailbox status: unreadMessages = " << unreadMessages;

	emit q->newNumberOfUnreadLetters(unreadMessages);
}

void MRIMClientPrivate::processNewMail(QByteArray data)
{
	qDebug() << "MRIM_CS_NEW_MAIL, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 unreadMessages;

	in >> unreadMessages;
	qDebug() << "mailbox new mail: unreadMessages = " << unreadMessages;

	QByteArray baSender, baSubject;
	quint32 unixTime;
	in >> baSender >> baSubject >> unixTime;

	if (unreadMessages > 0)
		audio.play(STLetter);

	emit q->newNumberOfUnreadLetters(unreadMessages);
	emit q->newLetter(codec->toUnicode(baSender), codec->toUnicode(baSubject), QDateTime::fromTime_t(unixTime));
}

void MRIMClientPrivate::processUserStatus(QByteArray data)
{
	qDebug() << "MRIM_CS_USER_STATUS, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 status;
	in >> status;
	QByteArray email;
	in >> email;
	qDebug() << "email = " << email;
	emit q->contactStatusChanged(status, email);
}

void MRIMClientPrivate::processMessageAck(QByteArray data)
{
	qDebug() << "MRIM_CS_MESSAGE_ACK, bytes = " << data.size();

	MRIMDataStream in(data);

	quint32 msgId, flags;
	QByteArray from, text;
	in >> msgId >> flags >> from >> text;

	qDebug() << "flags = " << QString::number(flags, 16);

	if (!(flags & MESSAGE_FLAG_NORECV))
	{
		QByteArray replyData;
		MRIMDataStream out(&replyData, QIODevice::WriteOnly);

		if (flags & MESSAGE_FLAG_SMS)
			out << QByteArray("mrim_sms@mail.ru");
		else
			out << from;
		out << msgId;
		sendPacket(MRIM_CS_MESSAGE_RECV, replyData);
	}

	if (flags & MESSAGE_FLAG_NOTIFY)
	{
		qDebug() << "--- contact typing ---";
		emit q->contactTyping(from);
		return;
	}

	if (flags & MESSAGE_FLAG_AUTHORIZE)
	{
		QString nickname, message;
		unpackAuthorizationMessage(text, nickname, message);

		audio.play(STAuth);

		emit q->contactAsksAuthorization(from, nickname, message);

		return;
	}

	QString plainText;
	plainText = codec->toUnicode(text);

	if (flags & (MESSAGE_FLAG_SMS | MESSAGE_SMS_DELIVERY_REPORT))
	{
		emit q->messageReceived
		(
			from, new Message(Message::Incoming, flags, plainText, from, 0x00FFFFFF)
		);
		return;
	}

	QByteArray rtfText;
	quint32 backgroundColor = 0x00FFFFFF;

	if (flags & MESSAGE_FLAG_RTF)
	{
		QByteArray rtf;
		in >> rtf;
		unpackRtf(rtf, &rtfText, &backgroundColor);
	}

	emit q->messageReceived
	(
		from,
		new Message(Message::Incoming, flags, plainText, rtfText, backgroundColor)
	);
}

void MRIMClientPrivate::processMessageStatus(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_MESSAGE_STATUS, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 st;
	in >> st;
	emit q->messageStatus(msgseq, st);
}

void MRIMClientPrivate::processModifyContactAck(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_MODIFY_CONTACT_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 status;
	in >> status;
	qDebug() << "seq = " << msgseq << ", status = " << status;
	emit q->contactModified(msgseq, status);
}

void MRIMClientPrivate::processAddContactAck(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_ADD_CONTACT_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 status;
	quint32 contactId;
	in >> status;
	in >> contactId;

	qDebug() << "seq = " << msgseq << ", status = " << status << ", id = " << contactId;

	emit q->contactAdded(msgseq, status, contactId);
}

void MRIMClientPrivate::processAuthorizeAck(QByteArray data)
{
	qDebug() << "MRIM_CS_AUTHORIZE_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	QByteArray email;
	in >> email;

	emit q->contactAuthorizedMe(email);
}

void MRIMClientPrivate::processOfflineMessageAck(QByteArray data)
{
	qDebug() << "MRIM_CS_OFFLINE_MESSAGE_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	quint64 uidl;
	QByteArray msg;
	in >> uidl;
	in >> msg;

	QByteArray replyData;
	MRIMDataStream out(&replyData, QIODevice::WriteOnly);

	out << uidl;
	sendPacket(MRIM_CS_DELETE_OFFLINE_MESSAGE, replyData);

	quint32 flags;
	QByteArray email;
	QString plainText;
	QByteArray rtfText;
	quint32 backgroundColor;
	QDateTime dateTime;

	QByteArray boundary;

	QList<QByteArray> strings = msg.split('\n');
	qDebug() << strings;
	int i = 0;
	while (i < strings.size() && !strings[i].isEmpty())
	{
		int index = strings[i].indexOf(' ');
		if (index == -1)
			continue;

		QByteArray value = strings[i].right(strings[i].size() - index - 1);

		if (strings[i].startsWith("From: "))
			email = value;
		else if (strings[i].startsWith("X-MRIM-Flags: "))
			flags = value.toUInt(NULL, 16);
		else if (strings[i].startsWith("Boundary: "))
			boundary = "--" + value + "--";
		else if (strings[i].startsWith("Date: "))
		{
			qDebug() << "date = " << value;
			dateTime = parseRFCDate(value);
		}
		++i;
	}
	++i;

	if (email.isEmpty())
	{
		qDebug() << "offline message: unknown email";
		return;
	}

	qDebug() << "from = " << email;
	qDebug() << "flags = " << QString::number(flags, 16);
	qDebug() << "boundary = " << boundary;
	qDebug() << "dateTime = " << dateTime.toString();

	if (flags & MESSAGE_FLAG_AUTHORIZE && i < strings.size() && strings[i] != boundary)
	{
		QString nickname, message;
		unpackAuthorizationMessage(strings[i], nickname, message);

		emit q->contactAsksAuthorization(email, nickname, message);

		return;
	}

	if (i < strings.size() && strings[i] != boundary)
		plainText += codec->toUnicode(strings[i]);
	++i;

	while (i < strings.size() && strings[i] != boundary)
	{
		plainText += "\n" + codec->toUnicode(strings[i]);
		++i;
	}

	if ((flags & MESSAGE_FLAG_RTF) && (i+1) < strings.size())
		unpackRtf(strings[i+1], &rtfText, &backgroundColor);

	emit q->messageReceived
	(
		email,
		new Message(Message::Incoming, flags, plainText, rtfText, backgroundColor, dateTime)
	);
}

void MRIMClientPrivate::processFileTransfer(QByteArray data)
{
	qDebug() << "MRIM_CS_FILE_TRANSFER, bytes = " << data.size();
	MRIMDataStream in(data);

	QByteArray email;
	quint32 sessionId;
	quint32 totalSize;
	QByteArray lps1;

	in >> email >> sessionId >> totalSize;
	qDebug() << "email = " << email << ", sessionId = " << sessionId << ", totalSize = " << totalSize;
	in >> lps1;

	qDebug() << "lps1 = " << lps1;
	qDebug() << "lps1.toHex = " << lps1.toHex();

	MRIMDataStream in2(lps1);

	QByteArray filesAnsi;
	QByteArray lps2;
	QByteArray ips;

	in2 >> filesAnsi >> lps2 >> ips;

	MRIMDataStream in3(lps2);

	quint32 unk;
	QString filesUtf;

	in3 >> unk >> filesUtf;

	qDebug() << "filesAnsi = " << filesAnsi;
	qDebug() << "ips = " << ips;
	qDebug() << "unk = " << unk;
	qDebug() << "filesUtf = " << filesUtf;

	FileMessage* fmsg = new FileMessage(FileMessage::Incoming, q->account()->email(), email, totalSize, sessionId, filesAnsi, filesUtf, ips);
	connect(q, SIGNAL(proxy(QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)), fmsg, SLOT(slotProxy(QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)));
	connect(q, SIGNAL(fileTransferAck(quint32, QByteArray, quint32, QByteArray)), fmsg, SLOT(slotFileTransferStatus(quint32, QByteArray, quint32, QByteArray)));
	connect(q, SIGNAL(proxyAck(quint32, QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)), fmsg, SLOT(slotProxyAck(quint32, QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)));

	connect(fmsg, SIGNAL(fileAck(quint32, QByteArray, quint32, QByteArray)), q, SLOT(sendFileAck(quint32, QByteArray, quint32, QByteArray)));
	connect(fmsg, SIGNAL(proxyAck(FileMessage*, quint32, quint32, quint32, quint32, quint32, quint32)), q, SLOT(sendProxyAck(FileMessage*, quint32, quint32, quint32, quint32, quint32, quint32)));

	emit q->fileReceived(fmsg);
}

void MRIMClientPrivate::processFileTransferAck(QByteArray data)
{
	qDebug() << "MRIM_CS_FILE_TRANSFER_ACK, bytes = " << data.size();
	qDebug() << data.toHex();
	MRIMDataStream in(data);

	quint32 status;
	QByteArray email;
	quint32 sessionId;
	QByteArray mirrorIps;

	in >> status >> email >> sessionId >> mirrorIps;

	qDebug() << "status = " << status << ", email = " << email << ", sessionId = " << sessionId << ", mirrorIps = " << mirrorIps;

	emit q->fileTransferAck(status, email, sessionId, mirrorIps);
}

void MRIMClientPrivate::processMPOPSession(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_MPOP_SESSION, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 status;
	QByteArray session;
	in >> status >> session;

	emit q->receivedMPOPSession(msgseq, status, session);
}

void MRIMClientPrivate::processSmsAck(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_SMS_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	quint32 status;
	in >> status;

	qDebug() << "sms status = " << QString::number(status, 16);

	emit q->smsAck(msgseq, status);
}

void MRIMClientPrivate::processProxy(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_PROXY, bytes = " << data.size();
	MRIMDataStream in(data);

	qDebug() << data.toHex();

	QByteArray email;
	quint32 idRequest;
	quint32 dataType;

	QByteArray filesAnsi;
	QByteArray proxyIps;
	quint32 sessionId;
	quint32 unk1, unk2, unk3;

	in >> email >> idRequest >> dataType >> filesAnsi >> proxyIps >> sessionId;
	in >> unk1 >> unk2 >> unk3;
	qDebug() << "email =" << email;
	qDebug() << "idRequest =" << idRequest;
	qDebug() << "dataType =" << dataType;
	qDebug() << "userData =" << filesAnsi;

	qDebug() << "lpsIpPort =" << proxyIps;
	qDebug() << "sessionId =" << sessionId;

	qDebug() << unk1 << unk2 << unk3;

	emit q->proxy(email, idRequest, dataType, filesAnsi, proxyIps, sessionId, unk1, unk2, unk3);
}

void MRIMClientPrivate::processProxyAck(QByteArray data, quint32 msgseq)
{
	qDebug() << "MRIM_CS_PROXY_ACK, bytes = " << data.size();
	MRIMDataStream in(data);

	qDebug() << data.toHex();

	quint32 status;
	QByteArray email;
	quint32 idRequest;
	quint32 dataType;
	QByteArray filesAnsi;
	QByteArray ips;
	quint32 sessionId;

	quint32 unk1, unk2, unk3, unk4, unk5, unk6, unk7, unk8;

	in >> status >> email >> idRequest >> dataType >> filesAnsi >> ips >> sessionId;
	in >> unk1 >> unk2 >> unk3 >> unk4 >> unk5 >> unk6 >> unk7 >> unk8;

	qDebug() << status;

	emit q->proxyAck(status, email, idRequest, dataType, filesAnsi, ips, sessionId, unk1, unk2, unk3);
}
