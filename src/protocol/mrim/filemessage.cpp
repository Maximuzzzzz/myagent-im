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

#include <QDir>
#include <QNetworkInterface>
#include <QTextCodec>
#include <QBoxLayout>

#include <limits>

#include "filemessage.h"
#include "protocol/mrim/proto.h"
#include "protocol/mrim/mrimdatastream.h"
#include "resourcemanager.h"

/*Conception of transferring file:

Sender - host, which will be transfer file(s) to Receiver
Receiver - host, which will be receive file(s) from Sender

Client - host, which will be send connect() to Server
Server - host, which will be listen for connect() from Client

It's important not to confuse Sender/Receiver with Server/Client, because Sender can be both Server and Client, depending of situation, such and Receiver

Steps:
1. Sender sends signal MRIM_CS_FILE_TRANSFER, after which create the FileMessage copy;
2. Receiver try to connect to Sender. Here Receiver will be Client, Sender will be Server
2a. If Receiver connected to Sender, file(s) will be transferred. See pp 8-10.
3. If Receiver can't connect to Sender, Receiver become the Server and Sender become the Client, than Sender try to connect to Receiver.
3a. If Sender connected to Receiver, file(s) will be transferred. See pp 8-10.
4. If Sender can't connect to Receiver, Sender send MRIM_CS_PROXY with ip-adresses and ports of proxy. If ip and ports are empty, than one of MRIM-Servers will be proxy.
5. Receiver gets MRIM_CS_PROXY with ip and port of proxy and answer with MRIM_CS_PROXY_ACK to Sender.
6. Both Receiver and Server independently of each other try to connect to proxy with MRIM_CS_PROXY_HELLO by sending this packet directly to proxy, not to MRIM-server.
7. If Receiver and Sender get MRIM_CS_PROXY_HELLO_ACK, Receiver become the Client and Sender become the Server, file(s) will be transferred. See pp 8-10.
8. Client sends string "MRA_FT_HELLO receiver@mail.ru", where receiver@mail.ru - Receiver's mail-address.
9. When Server gets "MRA_FT_HELLO receiver@mail.ru", it sends "MRA_FT_HELLO sender@mail.ru", where sender@mail.ru - Sender's mail-address.
10. When Receiver gets "MRA_FT_HELLO sender@mail.ru", it sends "MRA_FT_GET_FILE file[i]", where file[i] - each of files, which sends Sender.
*/

FileMessage::FileMessage(Type type/*, QList<QFileInfo> & files, QByteArray accEmail, QByteArray contEmail*/)
 : fm_type(type)
{
	clearParameters();

	if (fm_type == Outgoing)
	{
		int i;
		int randomPort = 1234; /*TODO: generate random port*/
		listeningPorts << "2041" << QByteArray::number(randomPort) << "443";
		QList<QHostAddress> IPs = QNetworkInterface::allAddresses();
		Q_FOREACH (QHostAddress ip, IPs)
		{
			if (!ip.isNull() && !ip.toString().contains(':') && ip != QHostAddress::LocalHost)
				for (i = 0; i < listeningPorts.count(); i++)
					fm_ips += ip.toString() + ":" + listeningPorts[i] + ";";
		}
	}
	fm_error = 0;

	fm_sessionId = ((double)qrand() / (double)RAND_MAX) * std::numeric_limits<quint32>::max();

}

void FileMessage::setParameters(quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips)
{
	qDebug() << Q_FUNC_INFO << sessionId;
	fm_filesAnsi = filesAnsi;
	fm_filesUtf = filesUtf;

	fm_totalSize = totalSize;

	fm_ips = ips;
	fm_sessionId = sessionId;
//	fm_status = status;

	fm_filesHtml = "";

	fm_fileListAnsi = filesAnsi.split(';');
	fm_fileListAnsi.takeLast();

	fm_fileListUtf = filesUtf.split(';');
	fm_fileListUtf.takeLast();

	fm_sizes.clear();

	int i;
	quint32 fullSize = 0;
	for (i = 1; i < fm_fileListAnsi.count(); i++)
	{
		int currSize = fm_fileListAnsi.takeAt(i).toLongLong();
		fm_fileListUtf.removeAt(i);
		fm_filesHtml += ((fm_filesHtml == "") ? "" : "<br>") + fm_fileListUtf.at(i - 1) + " (" + getSizeInString(currSize) + ")";
		fm_sizes.append(currSize);
		fullSize += currSize;
	}
	if (fullSize != totalSize)
		fm_error = 1;
}

FileMessage::~FileMessage()
{
}

void FileMessage::setStatus(quint32 status)
{
//	fm_status = status;
}

void FileMessage::setAccEmail(QByteArray email)
{
	fm_accEmail = email;
}

void FileMessage::setContEmail(QByteArray email)
{
	fm_contEmail = email;
}

void FileMessage::receiveFiles(quint32 sessId)
{
	qDebug() << Q_FUNC_INFO << sessId << fm_sessionId;

	if (sessId != fm_sessionId)
		return;

	fm_error = 0;

	qDebug() << fm_fileListUtf.size();
	QList<QString>::iterator it = fm_fileListUtf.begin();
	for (; it != fm_fileListUtf.end(); it++)
	{
		QString currFN = getReceivingDir() /*fm_defaultDir*/ + "/" + (*it);
		QFile file(currFN);
		qDebug() << file.fileName();
		if (file.exists())
		{
			FileExistsDialog* dialog = new FileExistsDialog(*it);
			if (dialog->exec() == QDialog::Accepted)
				(*it) = dialog->fileName();
			else
			{
				cancelTransferring(fm_sessionId);
				delete dialog;
				return;
			}
			delete dialog;
		}
	}

	createSocket();
	getIpArraysFromString();

	setTransferStatus(CONNECTING_TO_CLIENT, 0);
	ipForCheck = 0;
	bool st = tryToConnect();
	if (!st)
	{
		qDebug() << "Try to connect is failed";
		isMirror = true;
		listeningPorts << "2041" << "35261"; /*TODO: generate random port*/
		quint32 res = waitForIncomingConnect();
		sendFileAck(res);
	}
}

void FileMessage::slotConnectedToPeer()
{
	qDebug() << Q_FUNC_INFO;

	setTransferStatus(WAITING_FOR_HELLO, 0);
	if (!useProxy)
	{
		qDebug() << "Saying hello";
		sayHello();
	}
}

void FileMessage::slotReadyRead()
{
	qDebug() << Q_FUNC_INFO;

	QByteArray in(fm_socket->readAll());

	processIncomingMessage(in);
}

void FileMessage::processIncomingMessage(QByteArray in)
{
	qDebug() << Q_FUNC_INFO << in.length();// << in;
	if (in[0] == char(239) && transferStatus == WAITING_FOR_HELLO) /*TODO: interpretate packet fully*/
	{
		qDebug() << "Proxy said hello";
		sayHello();
//		Q_EMIT startTransferring(fm_sessionId);
	}
	else if (transferStatus == WAITING_FOR_HELLO && in.contains("MRA_FT_HELLO") && in.contains(fm_contEmail))
	{
		qDebug() << "Host said hello";

		currFile = 0;
		bytesTotalTransferred = 0;

		if (fm_type == Outgoing)
		{
			qDebug() << "Outgoing";
			setTransferStatus(TRANSFERRING_READY, 0);
			if (!isMirror)
			{
				qDebug() << "Not mirror";
				sayHello();
//				Q_EMIT startTransferring(fm_sessionId);
			}
		}
		else if (fm_type == Incoming)
		{
			qDebug() << "Incoming";
			if (isMirror && !useProxy)
			{
				qDebug() << "Mirror, not proxy";
				sayHello();
			}
			Q_EMIT startTransferring(fm_sessionId);
			getFile();
		}
		in = in.right(in.length() - in.indexOf(fm_contEmail) - fm_contEmail.length() - 1);
		//qDebug() << "in =" << in.length() << in.toHex() << in;
		if (in.length() > 0)
			processIncomingMessage(in);
	}
	else if (transferStatus == READY_TO_RECEIVE_FILE)
	{
		qDebug() << "Ready to receive file";

		getFile();
	}
	else if (transferStatus == RECEIVING_FILE)
	{
		qDebug() << "FileMessage::Receiving file" << currFile << fm_fileListAnsi[currFile] << fm_sizes[currFile];

		fm_currentFile.write(in);
		bytesTransferred += in.length();
		bytesTotalTransferred += in.length();
		setTransferStatus(RECEIVING_FILE, percentage());

		if (bytesTransferred >= fm_sizes[currFile])
		{
			fm_currentFile.close();
			qDebug() << "File closed";

			setTransferStatus(READY_TO_RECEIVE_FILE, 0);

			if (++currFile > fm_fileListAnsi.count() - 1)
			{
				setTransferStatus(RECEIVING_COMPLETE, 0);
				//Q_EMIT fileTransferred(transferStatus, fm_filesHtml, getReceivingDir());
				fm_socket->disconnectFromHost();
			}
			else
				getFile();
		}
	}
	else if (in.contains("MRA_FT_GET_FILE")/* && in.contains(fm_fileListAnsi[currFile])*/)
	{
		qDebug() << "Host waits to get the file";

		//Search requested file
		QByteArray requestedFile = in.right(in.length() - QByteArray("MRA_FT_GET_FILE ").length());
		requestedFile = requestedFile.left(requestedFile.length() - 1);
		qDebug() << "Requested file" << requestedFile;
		qDebug() << "Cycle from 0 to" << fm_fileListAnsi.size();
		for (currFile = 0; currFile < fm_fileListAnsi.size(); currFile++)
		{
			qDebug() << fm_fileListAnsi[currFile];
			if (fm_fileListAnsi[currFile] == requestedFile)
				break;
		}
		qDebug() << currFile;
		if (fm_fileListAnsi[currFile] != requestedFile)
		{
			qDebug() << "Error: file " << requestedFile << "not found";
			theRM.account()->client()->sendFileAck(FILE_TRANSFER_STATUS_ERROR, fm_contEmail, fm_sessionId, "");
			setTransferStatus(TRANSFER_ERROR, 0);
			return;
		}
		//
		if (transferStatus == TRANSFERRING_READY)
			sendFile();
		else if (transferStatus == TRANSFERRING_COMPLETE)
			fm_socket->disconnectFromHost();
	}
}

void FileMessage::slotDisconnected()
{
	qDebug() << Q_FUNC_INFO;

	if (transferStatus == RECEIVING_FILE)
	{
		qDebug() << "Reseiving file";
		fm_currentFile.close();
		fm_currentFile.remove();
		setTransferStatus(RECEIVE_ERROR, 0);
	}
	else if (transferStatus == TRANSFERRING_FILE)
	{
		qDebug() << "Transferring file";
		fm_currentFile.close();
		setTransferStatus(TRANSFER_ERROR, 0);
	}
	else
	{
		if (fm_type == Incoming && transferStatus != RECEIVING_COMPLETE)
		{
			qDebug() << "Incoming && transferStatus != RECEIVING_COMPLETE";
			fm_currentFile.close();
			fm_currentFile.remove();
			setTransferStatus(RECEIVE_ERROR, 0);
		}
		if (fm_type == Outgoing && transferStatus != TRANSFERRING_COMPLETE)
		{
			qDebug() << "Outgong && transferStatus != TRANSFERRING_COMPLETE";
			fm_currentFile.close();
			setTransferStatus(TRANSFER_ERROR, 0);
		}
	}

	clearParameters();
}

void FileMessage::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << Q_FUNC_INFO << err << ")";
}

void FileMessage::stateChanged(QAbstractSocket::SocketState state)
{
	qDebug() << Q_FUNC_INFO << state << ")";
}

bool FileMessage::tryToConnect()
{
	qDebug() << Q_FUNC_INFO;

	do
	{
		if (ipForCheck > ipsCheck.count() - 1)
			return false;
		qDebug() << "Try to connect to" << ipsCheck[ipForCheck] << portsCheck[ipForCheck];
		fm_socket->connectToHost(ipsCheck[ipForCheck], portsCheck[ipForCheck]);
		ipForCheck++;
	}
	while (!fm_socket->waitForConnected(1000));

	return true;
}

quint32 FileMessage::waitForIncomingConnect()
{
	qDebug() << Q_FUNC_INFO;

	if (fm_type == Incoming)
		fm_socket->deleteLater();

	setTransferStatus(WAITING_FOR_CONNECTION, 0);

	int i;
	for (i = 0; i < listeningPorts.count(); i++)
	{
		QTcpServer* s = new QTcpServer();
		connect(s, SIGNAL(newConnection()), this, SLOT(slotClientConnected()));
		qDebug() << "MRIM: FT: Starting" << i << "server @ All:" << listeningPorts[i].toLong();
		//qDebug() << "I'm kidding :p Server listening nothing";
		s->listen(QHostAddress::Any, listeningPorts[i].toLong());
		fm_servers.append(s);
	}

	return FILE_TRANSFER_MIRROR;
}

void FileMessage::slotClientConnected()
{
	qDebug() << Q_FUNC_INFO;

	currServer = qobject_cast<QTcpServer*>(sender());

	deleteServers();
	fm_socket = currServer->nextPendingConnection();
	connect(fm_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(fm_socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	connect(fm_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(slotBytesWritten(qint64)));
	currServer->close();

	setTransferStatus(WAITING_FOR_HELLO, 0);
}

void FileMessage::sendFileAck(quint32 status)
{
	qDebug() << Q_FUNC_INFO << status;

	QByteArray ips;
	if (status == FILE_TRANSFER_STATUS_DECLINE || status == FILE_TRANSFER_STATUS_ERROR)
		ips = "";
	else
	{
		QList<QHostAddress> localIps = QNetworkInterface::allAddresses();
		int i;
		Q_FOREACH (QHostAddress ip, localIps)
		{
			for (i = 0; i < listeningPorts.count(); i++)
				if (!ip.isNull() && !ip.toString().contains(':') && ip != QHostAddress::LocalHost)
					ips += ip.toString() + ":" + listeningPorts[i] + ";";
		}
	}
	theRM.account()->client()->sendFileAck(status, fm_contEmail, fm_sessionId, ips);
/*	Q_EMIT fileAck(status, fm_contEmail, fm_sessionId, ips);*/
}

void FileMessage::sayHello()
{
	qDebug() << Q_FUNC_INFO;
	sendData("MRA_FT_HELLO " + fm_accEmail);
}

void FileMessage::getFile()
{
	qDebug() << Q_FUNC_INFO;
	setTransferStatus(RECEIVING_FILE, percentage());
	bytesTransferred = 0;

	fm_currentFile.setFileName(getReceivingDir() + "/" + fm_fileListUtf[currFile]);
	//qDebug() << fm_defaultDir + "/" + fm_fileListUtf[currFile];

	fm_currentFile.open(QIODevice::WriteOnly);

	sendData("MRA_FT_GET_FILE " + fm_fileListAnsi[currFile]);

	if (fm_sizes[currFile] == 0)
	{
		qDebug() << "File is null sized";
		slotReadyRead();
	}
}

void FileMessage::sendData(const QString& str)
{
	qDebug() << Q_FUNC_INFO << str;

	QByteArray data;
	fm_socket->write(str.toLatin1() + "\0", str.toLatin1().length() + 1);
	fm_socket->flush();
}

void FileMessage::setTransferStatus(Status s, int p)
{
	qDebug() << "FileMessage::setTransferStatus" << s;

	transferStatus = s;

	/*if (s == TRANSFERRING_COMPLETE || s == RECEIVING_COMPLETE || s == TRANSFER_ERROR || s == RECEIVE_ERROR)
		Q_EMIT fileTransferred(s, fm_filesHtml, fm_defaultDir);*/
	Q_EMIT progress(s, p);
	Q_EMIT fileTransferred(transferStatus, fm_filesHtml, getReceivingDir());
}

void FileMessage::sendFiles(/*MRIMClient* cl*/)
{
	qDebug() << "FileMessage::sendFiles()";

	theRM.account()->client()->sendFile(this);
	waitForIncomingConnect();
}

void FileMessage::sendFile()
{
	qDebug() << Q_FUNC_INFO;

	bytesTransferred = 0;

	fm_currentFile.setFileName(fm_filePaths[currFile]);
	qDebug() << "Opening " << fm_filePaths[currFile];

	fm_currentFile.open(QIODevice::ReadOnly);
	fm_currentFileChunkSize = fm_currentFile.size() < 1360 ? fm_currentFile.size() : 1360;
	setTransferStatus(TRANSFERRING_FILE, 0);
	sendFilePart();
}

void FileMessage::slotBytesWritten(qint64 bytes)
{
	qDebug() << Q_FUNC_INFO;

	if (transferStatus != TRANSFERRING_FILE)
		return;

	bytesTransferred += bytes;
	bytesTotalTransferred += bytes;
	setTransferStatus(TRANSFERRING_FILE, percentage());
	qDebug() << percentage();

	if (bytesTransferred < fm_sizes[currFile])
		sendFilePart();
	else
	{
		setTransferStatus(TRANSFERRING_READY, percentage());
		fm_currentFile.close();

		if (++currFile >= fm_fileListAnsi.count())
			setTransferStatus(TRANSFERRING_COMPLETE, 0);
	}
}

void FileMessage::sendFilePart()
{
	qDebug() << Q_FUNC_INFO;

	fm_socket->write(fm_currentFile.read(fm_currentFileChunkSize));
}

int FileMessage::percentage()
{
	return int(double(bytesTotalTransferred) / double(fm_totalSize) * double(MAX_INT));
}


void FileMessage::slotFileTransferStatus(quint32 status, QByteArray email, quint32 sessionId, QByteArray mirrorIps)
{
	qDebug() << "FileMessage::slotFileTransferStatus";

	if (email != fm_contEmail || sessionId != fm_sessionId)
		return;

	qDebug() << status;
	if (status == FILE_TRANSFER_MIRROR)
	{
		if (transferStatus == WAITING_FOR_CONNECTION)
		{
			qDebug() << "Mirror connection";
			deleteServers();
			fm_ips = mirrorIps;
			isMirror = true;
			getIpArraysFromString();
			ipForCheck = 0;
			createSocket();
			if (!tryToConnect())
			{
				qDebug() << "I can't to connect to you. Let's try proxy?";
				useProxy = true;
				//Q_EMIT proxy(this, MRIM_PROXY_TYPE_FILES);
				theRM.account()->client()->sendProxy(this, MRIM_PROXY_TYPE_FILES);
			}
		}
		else
		{
			qDebug() << "Other transfer status" << transferStatus;
			return;
		}
	}
	else if (status == FILE_TRANSFER_STATUS_DECLINE)
		cancelTransferring(fm_sessionId, false);
}

void FileMessage::getIpArraysFromString()
{
	qDebug() << Q_FUNC_INFO;

	ipsCheck.clear();
	portsCheck.clear();
	ipForCheck = 0;

	ipsCheck = fm_ips.split(';');
	qDebug() << fm_ips;
	if (ipsCheck.last() == "")
		ipsCheck.takeLast();

	int i;
	for (i = 0; i < ipsCheck.count(); i++)
	{
		bool ok;
		portsCheck.append(ipsCheck[i].mid(ipsCheck[i].indexOf(":") + 1, ipsCheck[i].length() - ipsCheck[i].indexOf(":") - 1).toLong(&ok));
		if (!ok)
			fm_error = 1;
		ipsCheck[i] = ipsCheck.at(i).left(ipsCheck.at(i).indexOf(":"));
		if (ipsCheck[i] == "")
			fm_error = 1;

		qDebug() << ipsCheck[i];
		qDebug() << portsCheck[i];
	}
}

void FileMessage::deleteServers()
{
/*	qDebug() << "FileMessage::deleteServers()";

	int i;
	for (i = 0; i < listeningPorts.count(); i++)
	{
		if (fm_servers[i] != currServer)
		{
			qDebug() << "deleting" << i << "server";
			fm_servers[i]->deleteLater();
		}
	}*/
}

void FileMessage::createSocket()
{
	qDebug() << Q_FUNC_INFO;

	fm_socket = new QTcpSocket;
    connect(fm_socket, SIGNAL(connected()), this, SLOT(slotConnectedToPeer()));
    connect(fm_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(fm_socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	connect(fm_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(fm_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect(fm_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(slotBytesWritten(qint64)));
}

void FileMessage::slotProxy(QByteArray email, quint32 idRequest, quint32 dataType, QByteArray filesAnsi, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3)
{
	qDebug() << Q_FUNC_INFO;
	qDebug() << email << fm_contEmail;
	qDebug() << idRequest << fm_sessionId;

	if (email != fm_contEmail || idRequest != fm_sessionId)
		return;

	useProxy = true;
	initProxy(dataType, proxyIps, sessionId, unk1, unk2, unk3);
}

void FileMessage::slotProxyAck(quint32 status, QByteArray email, quint32 idRequest, quint32 dataType, QByteArray filesAnsi, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3)
{
	qDebug() << Q_FUNC_INFO;

	qDebug() << "email = " << email;
	qDebug() << "fm_contEmail" << fm_contEmail;
	qDebug() << "idRequest" << idRequest;
	qDebug() << "fm_proxySessId" << fm_proxySessId;

	if (email != fm_contEmail)
		return;

	if (status == PROXY_STATUS_OK)
	{
		qDebug() << "Proxy status is OK";
		initProxy(dataType, proxyIps, sessionId, unk1, unk2, unk3);
		return;
	}
	qDebug() << "Proxy status is" << status;

	if (idRequest != fm_proxySessId)
		return;

/*	if (status == PROXY_STATUS_DECLINE)
		cancelTransferring(sessionId);*/
}

void FileMessage::proxyHello()
{
	qDebug() << Q_FUNC_INFO;

	QByteArray headerBA;
	MRIMDataStream header(&headerBA, QIODevice::WriteOnly);

	QByteArray data;
	MRIMDataStream out(&data, QIODevice::WriteOnly);

	out << fm_proxySessId << fm_unk[0] << fm_unk[1] << fm_unk[2];

	header << quint32(CS_MAGIC);
	header << quint32(PROTO_VERSION);
	header << quint32(1);
	header << quint32(MRIM_CS_PROXY_HELLO);
	header << quint32(data.size());
	header << quint32(0);
	header << quint32(0);

	header << quint32(0);
	header << quint32(0);
	header << quint32(0);
	header << quint32(0);

	fm_socket->write(headerBA);
	fm_socket->write(data);

	fm_socket->flush();
}

void FileMessage::initProxy(quint32 dataType, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3)
{
	qDebug() << Q_FUNC_INFO;

	fm_unk[0] = unk1;
	fm_unk[1] = unk2;
	fm_unk[2] = unk3;
	fm_ips = proxyIps;
	fm_proxySessId = sessionId;
	getIpArraysFromString();

	createSocket();
	setTransferStatus(CONNECTING_TO_CLIENT, 0);

	if (tryToConnect())
	{
		qDebug() << "Connected to proxy";
		theRM.account()->client()->sendProxyAck(this, PROXY_STATUS_OK, dataType, sessionId, fm_unk[0], fm_unk[1], fm_unk[2]);
/*		Q_EMIT proxyAck(this, PROXY_STATUS_OK, dataType, sessionId, fm_unk[0], fm_unk[1], fm_unk[2]);*/
		proxyHello();
	}
	else
	{
		qDebug() << "Error! Could not connect to proxy";
		if (fm_type == Incoming)
			setTransferStatus(RECEIVE_ERROR, 0);
		else
			setTransferStatus(TRANSFER_ERROR, 0);
	}
}

QString FileMessage::getSizeInString(quint32 size)
{
	static const double KB = 1024;
	static const double MB = 1024*KB;
	static const double GB = 1024*MB;

	if (size > GB)
		return tr("%1 GB").arg(double(size)/GB, 0, 'f', 2);
	else if (size > MB)
		return tr("%1 MB").arg(double(size)/MB, 0, 'f', 2);
	else if (size > KB)
		return tr("%1 KB").arg(double(size)/KB, 0, 'f', 2);
	else
		return tr("%1 B").arg(size);
}

bool FileMessage::cancelTransferring(quint32 sessId, bool sendCancelPackage)
{
	qDebug() << "FileMessage::cancelTransferring";

	if (sessId != fm_sessionId && sessId != 0)
		return false;

	/*if (sessId == 0)
		sessId = fm_sessionId;*/

	if (transferStatus == RECEIVING_FILE || transferStatus == TRANSFERRING_FILE)
	{
		fm_currentFile.close();
		if (transferStatus == RECEIVING_FILE)
			fm_currentFile.remove();
	}

/*	if (transferStatus != WAITING_FOR_CONNECTION)
		fm_socket->disconnectFromHost();*/

	if (fm_type == Incoming)
	{
		//Q_EMIT fileTransferred(RECEIVE_CANCEL, fm_filesHtml, getReceivingDir());
		setTransferStatus(RECEIVE_CANCEL, 0);
	}
	else if (fm_type == Outgoing)
	{
		//Q_EMIT fileTransferred(TRANSFER_CANCEL, fm_filesHtml, getReceivingDir());
		setTransferStatus(TRANSFER_CANCEL, 0);
	}

	if (sendCancelPackage)
	{
		if (!useProxy)
			theRM.account()->client()->sendFileAck(FILE_TRANSFER_STATUS_DECLINE, fm_contEmail, fm_sessionId, "");
		else
			theRM.account()->client()->sendProxyAck(this, PROXY_STATUS_DECLINE, MRIM_PROXY_TYPE_FILES, fm_proxySessId, fm_unk[0], fm_unk[1], fm_unk[2]);
			//Q_EMIT proxyAck(this, PROXY_STATUS_DECLINE, MRIM_PROXY_TYPE_FILES, fm_proxySessId, fm_unk[0], fm_unk[1], fm_unk[2]);
	}

	clearParameters();

	return true;
}

QString FileMessage::getDefDir()
{
	QDir dir = QDir::home();
	if (!dir.exists(tr("Myagent-im files")))
		dir.mkdir(tr("Myagent-im files"));
	dir.cd(tr("Myagent-im files"));

	if (!dir.exists(fm_accEmail))
		dir.mkdir(fm_accEmail);
	dir.cd(fm_accEmail);

	if (!dir.exists(fm_contEmail))
		dir.mkdir(fm_contEmail);
	dir.cd(fm_contEmail);

	return dir.absolutePath();
}

void FileMessage::setFileList(QList<QFileInfo> & files)
{
	qDebug() << Q_FUNC_INFO;
	QByteArray asciiName;
	int filesInUnicode = 0;

	fm_filesAnsi = "";
	fm_filesUtf = "";
	fm_filesHtml = "";
	fm_fileListAnsi.clear();
	fm_fileListUtf.clear();
	fm_sizes.clear();
	fm_filePaths.clear();
	fm_totalSize = 0;
	QList<QFileInfo>::iterator it = files.begin();
	for (; it != files.end(); it++)
	{
		QTextCodec* codec = QTextCodec::codecForName("cp1251");
		if (codec->canEncode(it->fileName()))
			asciiName = it->fileName().toAscii();
		else
			asciiName = "unicode_file_name_" + QByteArray::number(filesInUnicode++) + ".";

		fm_fileListAnsi.append(asciiName);
		fm_fileListUtf.append(it->fileName());

		fm_filesAnsi += asciiName + ";" + QByteArray::number(it->size()) + ";";
		fm_filesUtf += it->fileName() + ";" + QByteArray::number(it->size()) + ";";
		fm_filesHtml += ((fm_filesHtml == "") ? "" : "<br>") + it->absoluteFilePath() + " (" + getSizeInString(it->size()) + ")";

		fm_sizes.append(it->size());
		fm_filePaths.append(it->absoluteFilePath());
		fm_totalSize += it->size();
	}
}

QString FileMessage::getReceivingDir()
{
	return theRM.account()->settings()->value("FilesTransfer/defaultDir", getDefDir()).toString();
}


void FileMessage::clearParameters()
{
	currServer = NULL;
	isMirror = false;
	useProxy = false;

	fm_sessionId = 0;

	if (fm_type == Incoming)
		fm_ips = "";
}

FileExistsDialog::FileExistsDialog(QString fileName, QWidget* parent, Qt::WindowFlags f)
 : QDialog(parent, f), m_oldFileName(fileName)
{
	label = new QLabel(tr("File %1 is already exists. What should we do?").arg(fileName));
	quint32 i;
	QFile file;
	for (i = 1; i <= sizeof(quint32); i++)
	{
		file.setFileName(fileName + QString("-") + QString::number(i));
		if (!file.exists())
			break;
	}
	m_newFileName = file.fileName();
	lineEdit = new QLineEdit(m_newFileName);
	rewriteButton = new QPushButton(tr("Rewrite file"));
	renameButton = new QPushButton(tr("Rename file"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(rewriteButton, SIGNAL(clicked()), this, SLOT(rewriteFile()));
	connect(renameButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle(tr("File exists"));
	QVBoxLayout* layout = new QVBoxLayout;
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(rewriteButton);
	buttonsLayout->addWidget(renameButton);
	buttonsLayout->addWidget(cancelButton);
	layout->addWidget(label);
	layout->addWidget(lineEdit);
	layout->addLayout(buttonsLayout);
	setLayout(layout);
}

void FileExistsDialog::rewriteFile()
{
	m_newFileName = m_oldFileName;
	accept();
}
