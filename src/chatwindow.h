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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QPointer>

class QSplitter;
class QTimeLine;

class ChatSession;
class Message;
class MessageEditor;
class SmsEditor;

class Account;

#include "animatedtextedit.h"
#include "animatedtextbrowser.h"
#include "animatedtextdocument.h"
#include "filemessage.h"
#include "onlinestatus.h"

class ChatWindow : public QMainWindow
{
Q_OBJECT
public:
	ChatWindow(Account* account, ChatSession* s);
	~ChatWindow();
	
public slots:
	void messageDelivered(bool, Message*);
	void shake();

	void slotMakeRead();

	bool getIsNewMessage() { return isNewMessage; }

	void editorActivate();

signals:
	void messageEditorActivate();
	void smsEditorActivate();
	void setMainWindowIconAndTitle(QIcon icon, ChatWindow* sender);
	void newMessage(ChatWindow* sender);

private slots:
	void send();
	quint32 sendMessage();
	quint32 sendSms();
	void contactTyping();
	void appendMessageToView(const Message* msg, bool newIncoming = true);
	void checkContactStatus(OnlineStatus status);

	void appendSmsToView(QByteArray phoneNumber, QString text);
	void smsFailed();

	void wakeupContact();

	void shakeStep();
	void restorePosAfterShake();

	void saveTopAvatarBoxState(bool checked);
	void saveBottomAvatarBoxState(bool checked);
	
	void slotEditorActivate(int);

	void slotTimeout();
	void clearStatus();

	void fileTransferring(FileMessage* fmsg);
	void fileReceiving(FileMessage* fmsg);

	void slotAnchorClicked(QUrl url);
	void clenupCommandUrls();

	void transferStarted(quint32 sessId);

	void slotFileTransferred(FileMessage::Status action, QString filesInHtml, QString destination);

protected:
	QTimer* timer;

private:
	Account* m_account;
	ChatSession* session;
	
	QSplitter* splitter;
	AnimatedTextBrowser* chatView;
	MessageEditor* messageEditor;
	SmsEditor* smsEditor;
	
	QTextDocument doc;

	int savedX;
	int savedY;
	QPointer<QTimeLine> shakeTimeLine;
	bool isNewMessage;

	QTabWidget* editorsWidget;
};

#endif
