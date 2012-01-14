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

#include <QWidget>
#include <QPointer>
#include <QTextDocument>
#include <QDateTime>

#include "protocol/mrim/filemessage.h"
#include "onlinestatus.h"

class QSplitter;
class QTimeLine;
class QTabWidget;
class QStatusBar;
class QUrl;
class QPushButton;

class ChatSession;
class Message;
class MessageEditor;
class SmsEditor;
class MultSelector;
class MultInfo;
class AnimatedTextBrowser;
class ContactListBroadcast;
class ContactListConferenceWithHandle;
class EmoticonSelector;
class Account;
class IFlashPlayer;

class ChatWindow : public QWidget
{
Q_OBJECT
public:
	enum TransferStatus
	{
		FilesReceiving,
		FilesTransferring,
		None
	};
	struct Msg
	{
		QByteArray from;
		QDateTime dateTime;
	};

	ChatWindow(Account* account, ChatSession* s, EmoticonSelector* emoticonSelector, MultSelector* multSelector);
	~ChatWindow();

public Q_SLOTS:
	void messageDelivered(bool, Message*);
	void shake();

	void slotMakeRead();

	bool getIsNewMessage() { return isNewMessage; }

	void editorActivate();

Q_SIGNALS:
	void messageEditorActivate();
	void smsEditorActivate();
	void setMainWindowIconAndTitle(QIcon icon, ChatWindow* sender);
	void newMessage(ChatWindow* sender);
	void setIgnore(bool ignore);
	void ignoreSet(bool ignore);
	void setSignalCheckSpelling(bool b);
	void signalCheckSpellingSet(bool b);
	void contactIgnored(bool b);

private Q_SLOTS:
	void send();
	bool sendMessage();
	bool sendSms();
	void sendMult(const QString& id);
	void appendMessageToView(const Message* msg, bool newIncoming = true);
/*	void appendMessageToView(QTextDocumentFragment fragment);*/
/*	void appendMultToView(QString multId);*/
	void contactTyping();
	void putHeader(const QString& nick, Msg currMessage, QString* prompt);
	void checkContactStatus(OnlineStatus status);
	void microblogChanged(QString text, QDateTime mbDateTime);

	void appendSmsToView(QByteArray phoneNumber, QString text);
	void smsFailed();

	void wakeupContact();

	void shakeStep();
	void restorePosAfterShake();

	void saveTopAvatarBoxState(bool checked);
	void saveBottomAvatarBoxState(bool checked);
	void saveConferenceListState(bool checked);
	
	void slotEditorActivate(int tab);

	void slotTimeout();
	void clearStatus();

	void fileTransferring(QList<QFileInfo> files);
	void fileReceiving(quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips);
	void transferringCancelled();

	void slotAnchorClicked(const QUrl& url);
	void cleanupCommandUrls(/*QString str = ""*/);

	void transferStarted(quint32 sessId);

	void slotFileTransferred(FileMessage::Status action, QString filesInHtml, QString destination);
	void setContactStatusIcon(QString type = "");
	void contactUpdated();
	void sendButtonEnabledProcess();

	void showBroadcastPanel(bool visible);
	void showGameMenu(bool triggered);

	void contactIgnored();

	void showMult(const MultInfo* multInfo);

protected:
	QTimer* timer;

private:
	Account* m_account;
	ChatSession* session;
	
	QSplitter* splitter;
	AnimatedTextBrowser* chatView;
	MessageEditor* messageEditor;
	SmsEditor* smsEditor;
	ContactListBroadcast* broadcastPanel;
	ContactListConferenceWithHandle* conferenceListPanel;

	QTextDocument doc;

	int savedX;
	int savedY;
	QPointer<QTimeLine> shakeTimeLine;
	bool isNewMessage;

	QTabWidget* editorsWidget;
	QStatusBar* statusBar;
	QPushButton* sendButton;

	Msg lastMessage;

	FileMessage* fileMessageOut;
	FileMessage* fileMessageIn;

	TransferStatus transferStatus;

	IFlashPlayer* flashPlayer;
};

#endif
