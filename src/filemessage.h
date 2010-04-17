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

#define MAX_INT32	4294836225
#define MAX_INT		32767

#ifndef FILEMESSAGE_H
#define FILEMESSAGE_H

#include <QDateTime>
#include <QFileInfo>
#include <QTcpSocket>
#include <QTcpServer>

#include "mrimclient.h"

class FileMessage : public QObject
{
Q_OBJECT
public:
	enum Type { Incoming, Outgoing };
	
	enum Status {
		CONNECTING_TO_CLIENT,
		WAITING_FOR_CONNECTION,
		WAITING_FOR_HELLO,
		READY_TO_RECEIVE_FILE,
		RECEIVING_FILE,
		RECEIVING_COMPLETE,
		RECEIVE_ERROR,
		RECEIVE_CANCEL,
		TRANSFERRING_FILE,
		TRANSFERRING_READY,
		TRANSFERRING_COMPLETE,
		TRANSFER_ERROR,
		TRANSFER_CANCEL
	};

	FileMessage(Type type, QList<QFileInfo> & files, QByteArray accEmail = "", QByteArray contEmail = "");
	FileMessage(Type type, QByteArray accEmail, QByteArray contEmail, quint32 ts, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips, quint32 status = 0);
	
	~FileMessage();
	
	Type type() const { return fm_type; }

	quint32 getTotalSize() const { return fm_totalSize; }

	QByteArray getFilesAnsi() const { return fm_filesAnsi; }
	QList<QByteArray> getFileListAnsi() { return fm_fileListAnsi; }
	QString getFilesUtf() const { return fm_filesUtf; }
	QList<QString> getFileListUtf() { return fm_fileListUtf; }
	QString getFilesInHtml() { return fm_filesHtml; }
	static QString getSizeInString(quint32 size);

	QList<qint64> getSizes() { return fm_sizes; }
	QByteArray getIps() { return fm_ips; }
	quint32 getSessionId() { return fm_sessionId; }
	quint32 getStatus() { return fm_status; }
	QByteArray getAccEmail() { return fm_accEmail; }
	QByteArray getContEmail() { return fm_contEmail; }

	void setStatus(quint32 status);
	void setAccEmail(QByteArray email);
	void setContEmail(QByteArray email);
	void setDefDir(QString d);
	void receiveFiles(quint32 sessId);
	void sendFiles(MRIMClient* cl);

	quint32 error() { return fm_error; }

public slots:
	void slotFileTransferStatus(quint32 status, QByteArray email, quint32 sessionId, QByteArray mirrorIps);
	void slotProxy(QByteArray email, quint32 idRequest, quint32 dataType, QByteArray filesAnsi, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	void slotProxyAck(quint32 status, QByteArray email, quint32 idRequest, quint32 dataType, QByteArray filesAnsi, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	bool cancelTransferring(quint32 sessId);

signals:
	void progress(FileMessage::Status action, int percentage);
	void fileAck(quint32 status, QByteArray email, quint32 sessionId, QByteArray ips);
	void proxy(FileMessage* fmsg, quint32 dataType);
	void proxyAck(FileMessage* fmsg, quint32 status, quint32 dataType, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);
	void startTransferring(quint32 sessId);
	void fileTransferred(FileMessage::Status action, QString filesInHtml, QString destination);

private slots:
	void slotConnectedToPeer();
	void slotReadyRead();
	void slotDisconnected();
	void socketError(QAbstractSocket::SocketError err);
	void stateChanged(QAbstractSocket::SocketState state);
	void slotBytesWritten(qint64 bytes);
	bool tryToConnect();
	quint32 waitForIncomingConnect();

	void slotClientConnected();
	void sendFileAck(quint32 status);

	void sayHello();
	void getFile();

	void sendData(const QString& str);

	void setTransferStatus(Status, int p);

	void sendFile();
	void sendFilePart();

	int percentage();
	void getIpArraysFromString();
	void deleteServers();
	void createSocket();

	void proxyHello();
	void initProxy(quint32 dataType, QByteArray proxyIps, quint32 sessionId, quint32 unk1, quint32 unk2, quint32 unk3);

	QString getDefDir();

private:
	Type fm_type;

	quint32 fm_totalSize;

	QByteArray fm_filesAnsi;
	QString fm_filesUtf;
	QString fm_filesHtml;

	QString fm_defaultDir;

	QList<QByteArray> fm_fileListAnsi;
	QList<QString> fm_fileListUtf;
	QList<QString> fm_filePaths;

	QList<qint64> fm_sizes;
	QByteArray fm_ips;
	quint32 fm_sessionId;
	quint32 fm_status;

	QByteArray fm_accEmail;
	QByteArray fm_contEmail;

	quint32 fm_error;

	QTcpSocket* fm_socket;
	QTcpServer* currServer;
	QList<QTcpServer*> fm_servers;

	int ipForCheck;
	int currFile;

	QFile fm_currentFile;
	int fm_currentFileChunkSize;

	qint64 bytesTransferred;
	qint64 bytesTotalTransferred;

	Status transferStatus;

	QList<QByteArray> ipsCheck;
	QList<quint16> portsCheck;
	QList<QByteArray> listeningPorts;

	bool isMirror;
	bool useProxy;

	quint32 fm_proxySessId;
	quint32 fm_unk[3]; /*TODO: this is the SessionId. Have to make array of 4 elements*/
};

#endif
