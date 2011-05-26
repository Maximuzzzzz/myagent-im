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

#include "chatwindow.h"

#include <QDebug>

#include <QSplitter>
#include <QTextEdit>
#include <QTextDocumentFragment>
#include <QStatusBar>
#include <QPushButton>
#include <QApplication>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include <QScrollBar>
#include <QTimeLine>
#include <QTabWidget>
#include <QStackedLayout>
#include <QFont>

#include "proto.h"
#include "account.h"
#include "contact.h"
#include "mrimclient.h"
#include "message.h"
#include "chatsession.h"
#include "messageeditor.h"
#include "smseditor.h"
#include "rtfexporter.h"
#include "rtfparser.h"
#include "plaintextexporter.h"
#include "plaintextparser.h"
#include "avatarboxwithhandle.h"
#include "audio.h"

ChatWindow::ChatWindow(Account* account, ChatSession* s, EmoticonSelector* emoticonSelector)
	: QWidget(),
	m_account(account), session(s), messageEditor(0), smsEditor(0)
{
	qDebug() << Q_FUNC_INFO << "{";

	transferStatus = None;

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);

	isNewMessage = false;

	connect(s->contact(), SIGNAL(renamed(QString)), this, SLOT(contactUpdated()));
	connect(s->contact(), SIGNAL(ignoredChanged()), this, SLOT(contactIgnored()));

	timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

	QHBoxLayout* mainLayout = new QHBoxLayout;
	splitter = new QSplitter(Qt::Vertical, this);

	QWidget* chatWidget = new QWidget;
	QHBoxLayout* chatWidgetLayout = new QHBoxLayout;
	chatWidgetLayout->setContentsMargins(1, 1, 1, 1);
	chatWidgetLayout->setSpacing(1);
	chatView = new AnimatedTextBrowser;
	chatWidgetLayout->addWidget(chatView);
	connect(chatView, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotAnchorClicked(QUrl)));
	if (!s->contact()->isPhone() && !session->contact()->isConference())
	{
		AvatarBoxWithHandle* avatarBoxWithHandle = new AvatarBoxWithHandle(m_account->avatarsPath(), session->contact()->email());

		avatarBoxWithHandle->toggle(m_account->settings()->value("ChatWindow/TopAvatarBoxState", false).toBool());

		connect(avatarBoxWithHandle, SIGNAL(toggled(bool)), SLOT(saveTopAvatarBoxState(bool)));
		chatWidgetLayout->addWidget(avatarBoxWithHandle);
	}
	chatWidget->setLayout(chatWidgetLayout);
	splitter->addWidget(chatWidget);

	if (!s->contact()->isPhone())
	{
		QWidget* bottomWidget = new QWidget;
		QHBoxLayout* bottomWidgetLayout = new QHBoxLayout;
		bottomWidgetLayout->setContentsMargins(1, 1, 1, 1);
		bottomWidgetLayout->setSpacing(1);

		editorsWidget = new QTabWidget;
		editorsWidget->setStyleSheet("QTabWidget::pane { border: 0px; } QTabWidget::tab-bar { left: 1px; } QTabBar::tab { padding: 0px 32px 0px 32px; font: bold 9px; }");
		editorsWidget->setTabPosition(QTabWidget::South);
		editorsWidget->setTabShape(QTabWidget::Triangular);

		messageEditor = new MessageEditor(m_account, session->contact(), emoticonSelector);
		connect(messageEditor, SIGNAL(sendPressed()), this, SLOT(sendMessage()));
		connect(messageEditor, SIGNAL(textChanged()), session, SLOT(sendTyping()));
//		connect(messageEditor, SIGNAL(textChanged()), this, SLOT(sendButtonEnabledProcess()));
		connect(messageEditor, SIGNAL(wakeupPressed()), this, SLOT(wakeupContact()));
		connect(this, SIGNAL(messageEditorActivate()), messageEditor, SLOT(messageEditorActivate()));

		connect(messageEditor, SIGNAL(setIgnore(bool)), this, SIGNAL(setIgnore(bool)));
		connect(this, SIGNAL(ignoreSet(bool)), messageEditor, SLOT(slotIgnoreSet(bool)));

		connect(messageEditor, SIGNAL(setSignalCheckSpelling(bool)), this, SIGNAL(setSignalCheckSpelling(bool)));
		connect(this, SIGNAL(signalCheckSpellingSet(bool)), messageEditor, SLOT(setCheckSpelling(bool)));

		fileMessageOut = new FileMessage(FileMessage::Outgoing);
		fileMessageIn = new FileMessage(FileMessage::Incoming);
		fileMessageIn->setAccEmail(account->email());
		fileMessageIn->setContEmail(session->contact()->email());
		fileMessageOut->setAccEmail(account->email());
		fileMessageOut->setContEmail(session->contact()->email());

		//connect(fileMessageOut, SIGNAL(startTransferring(quint32)), this, SLOT(transferStarted(quint32)));
		connect(fileMessageOut, SIGNAL(fileTransferred(FileMessage::Status, QString, QString)), this, SLOT(slotFileTransferred(FileMessage::Status, QString, QString)));
		connect(fileMessageOut, SIGNAL(progress(FileMessage::Status, int)), messageEditor, SLOT(slotProgress(FileMessage::Status, int)));
		connect(fileMessageIn, SIGNAL(startTransferring(quint32)), this, SLOT(transferStarted(quint32)));
		connect(fileMessageIn, SIGNAL(fileTransferred(FileMessage::Status, QString, QString)), this, SLOT(slotFileTransferred(FileMessage::Status, QString, QString)));
		connect(fileMessageIn, SIGNAL(progress(FileMessage::Status, int)), messageEditor, SLOT(slotProgress(FileMessage::Status, int)));
		connect(m_account->client(), SIGNAL(fileTransferAck(quint32, QByteArray, quint32, QByteArray)), fileMessageOut, SLOT(slotFileTransferStatus(quint32, QByteArray, quint32, QByteArray)));
		connect(m_account->client(), SIGNAL(proxyAck(quint32, QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)), fileMessageOut, SLOT(slotProxyAck(quint32, QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)));
		connect(m_account->client(), SIGNAL(fileTransferAck(quint32, QByteArray, quint32, QByteArray)), fileMessageIn, SLOT(slotFileTransferStatus(quint32, QByteArray, quint32, QByteArray)));
		connect(m_account->client(), SIGNAL(proxy(QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)), fileMessageIn, SLOT(slotProxy(QByteArray, quint32, quint32, QByteArray, QByteArray, quint32, quint32, quint32, quint32)));

		connect(messageEditor, SIGNAL(transferringCancel()), this, SLOT(transferringCancelled()));

		editorsWidget->addTab(messageEditor, tr("Text"));
		if (!session->contact()->isConference())
		{
			smsEditor = new SmsEditor(m_account, session->contact());
			connect(smsEditor, SIGNAL(sendPressed()), this, SLOT(sendSms()));

			editorsWidget->addTab(smsEditor, tr("SMS"));

			connect(editorsWidget, SIGNAL(currentChanged(int)), this, SLOT(slotEditorActivate(int)));
		}

		bottomWidgetLayout->addWidget(editorsWidget);
		if (!session->contact()->isConference())
		{
			AvatarBoxWithHandle* avatarBoxWithHandle = new AvatarBoxWithHandle(m_account->avatarsPath(), m_account->email());
			avatarBoxWithHandle->toggle(m_account->settings()->value("ChatWindow/BottomAvatarBoxState", false).toBool());
			connect(avatarBoxWithHandle, SIGNAL(toggled(bool)), SLOT(saveBottomAvatarBoxState(bool)));

			bottomWidgetLayout->addWidget(avatarBoxWithHandle);
		}

		bottomWidget->setLayout(bottomWidgetLayout);

		splitter->addWidget(bottomWidget);

		if (!session->contact()->isConference())
		{
			connect(session->contact(), SIGNAL(statusChanged(OnlineStatus)), this, SLOT(checkContactStatus(OnlineStatus)));
			connect(this, SIGNAL(smsEditorActivate()), smsEditor, SLOT(smsEditorActivate()));
			connect(session->contact(), SIGNAL(typing()), this, SLOT(contactTyping()));
		}

		//connect(messageEditor, SIGNAL(filesTransfer(FileMessage*)), session, SLOT(fileTransfer(FileMessage*)));
		//connect(messageEditor, SIGNAL(filesTransfer(FileMessage*)), this, SLOT(fileTransferring(FileMessage*)));
		connect(messageEditor, SIGNAL(filesTransfer(QList<QFileInfo>)), this, SLOT(fileTransferring(QList<QFileInfo>)));
		//connect(session, SIGNAL(signalFileReceived(FileMessage*)), messageEditor, SLOT(fileReceived(FileMessage*)));
		connect(session, SIGNAL(signalFileReceived(quint32, quint32, QByteArray, QString, QByteArray)), this, SLOT(fileReceiving(quint32, quint32, QByteArray, QString, QByteArray)));
	}
	else
	{
		//setWindowTitle(session->contact()->nickname().append(" - ").append(session->contact()->client()));

		smsEditor = new SmsEditor(s->account(), s->contact());
		connect(smsEditor, SIGNAL(sendPressed()), this, SLOT(sendSms()));
		splitter->addWidget(smsEditor);
	}

	checkContactStatus(session->contact()->status());

	splitter->setStretchFactor(0, 7);
	splitter->setStretchFactor(1, 3);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	splitter->setSizePolicy(sizePolicy);

	mainLayout->addWidget(splitter);

	if (!s->contact()->isPhone() && !s->contact()->isConference())
	{
		broadcastPanel = new ContactListBroadcast(s->contact()->email(), m_account->contactList());
		connect(messageEditor, SIGNAL(showBroadcastPanel(bool)), this, SLOT(showBroadcastPanel(bool)));
		mainLayout->addWidget(broadcastPanel);
		broadcastPanel->setVisible(false);
	}
	else if (s->contact()->isConference())
	{
		conferenceListPanel = new ContactListConferenceWithHandle(session->contact(), session->account());
		conferenceListPanel->toggle(m_account->settings()->value("ChatWindow/ConferenceListVisible", false).toBool());
		connect(conferenceListPanel, SIGNAL(toggled(bool)), SLOT(saveConferenceListState(bool)));
		mainLayout->addWidget(conferenceListPanel);
	}

	layout->addLayout(mainLayout);

	statusBar = new QStatusBar();

	layout->addWidget(statusBar);

	sendButton = new QPushButton(tr("Send"), this);
	connect(sendButton, SIGNAL(clicked(bool)), this, SLOT(send()));
	connect(m_account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(sendButtonEnabledProcess()));

	connect(session, SIGNAL(messageDelivered(bool, Message*)), this, SLOT(messageDelivered(bool, Message*)));
	connect(session, SIGNAL(messageAppended(const Message*)), this, SLOT(appendMessageToView(const Message*)));

	connect(session, SIGNAL(microblogChanged(QString)), this, SLOT(microblogChanged(QString)));

	connect(session, SIGNAL(smsDelivered(QByteArray, QString)), this, SLOT(appendSmsToView(QByteArray, QString)));
	connect(session, SIGNAL(smsFailed()), this, SLOT(smsFailed()));

	statusBar->addPermanentWidget(sendButton);
	statusBar->setSizeGripEnabled(false);

	if (session->contact()->showMicroblogText())
	{
		microblogChanged(session->contact()->microblogText());
		session->contact()->setShowMicroblogText(false);
	}

	ChatSession::MessageIterator it = session->messagesBegin();
	for (; it != session->messagesEnd(); ++it)
		appendMessageToView(*it, false);
	session->clearHash();

	shakeTimeLine = new QTimeLine(2000, this);
	shakeTimeLine->setFrameRange(0, 200);
	connect(shakeTimeLine, SIGNAL(frameChanged(int)), SLOT(shakeStep()));
	connect(shakeTimeLine, SIGNAL(finished()), SLOT(restorePosAfterShake()));

	setLayout(layout);

	qDebug() << Q_FUNC_INFO << "}";
}

void ChatWindow::send()
{
	if (messageEditor && messageEditor->isVisible())
		sendMessage();
	else if (smsEditor && smsEditor->isVisible())
		sendSms();
}

quint32 ChatWindow::sendMessage()
{
	PlainTextExporter textExporter(messageEditor->document());
	QString messageText = textExporter.toText();

	RtfExporter rtfExporter(messageEditor->document());
	QByteArray messageRtf = rtfExporter.toRtf();

	if (messageText.isEmpty())
		return 0;

	qDebug() << "ChatWindow::sendMessage()" << messageRtf;

	messageEditor->clear();

	theRM.getAudio()->play(STOtprav);

	if (!session->contact()->isConference())
	{
		QList<QByteArray> receiversList = broadcastPanel->receivers();
		if (receiversList.count() == 0 || (receiversList.count() == 1 && receiversList.at(0) == session->contact()->email()))
			return session->sendMessage(messageText, messageRtf);
		else
		{
			qDebug() << "Broadcast";
			return session->broadcastMessage(broadcastPanel->receivers(), messageText, messageRtf);
		}
	}
	else
		return session->sendMessage(messageText, messageRtf);
}

quint32 ChatWindow::sendSms()
{
	if (!m_account->onlineStatus().connected())
		return 0;

	QString text = smsEditor->text();
	QByteArray number = smsEditor->phoneNumber();

	if (smsEditor->text().isEmpty())
		return 0;

	smsEditor->blockInput();
	emit smsEditorActivate();

	theRM.getAudio()->play(STOtprav);

	return session->sendSms(number, text);
}

void ChatWindow::appendMessageToView(const Message* msg, bool newIncoming)
{
	qDebug() << "ChatWindow::appendMessageToView Message, newIncoming =" << newIncoming;
	if (msg->type() == Message::Incoming)
	{
		clearStatus();
		qDebug() << "IsActiveWindow: " << isActiveWindow();
		if (newIncoming && (!isVisible()))
		{
			setContactStatusIcon("message");
			isNewMessage = true;
		}
	}
	show();

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	QString prompt;
	if (msg->flags() & MESSAGE_FLAG_SMS)
	{
		prompt = "<font color=red>" + tr("Sms from number") + " " + msg->rtfText() + " (" + msg->dateTime().time().toString(m_account->settings()->value("Messages/DateMask", theRM.defDateFormat).toString()) + ") :</font><br>";
		lastMessageFrom = "";
	}
	else if (msg->flags() & MESSAGE_SMS_DELIVERY_REPORT)
	{
		prompt = msg->dateTime().time().toString() + " <b>" + tr("Sms status for number") + " " + msg->rtfText() + "</b> :<br>";
		lastMessageFrom = "";
	}
	else if (msg->flags() & MESSAGE_FLAG_ALARM)
	{
		prompt = "<font color=green>" + msg->dateTime().toString() + " <b>" + tr("Alarm clock:")  + " <b></font>";
		lastMessageFrom = "";
	}
	else
	{
		QString nick;
		QByteArray currMessageFrom;
		if (msg->type() == Message::Outgoing)
		{
			nick = "<font color=blue>" + session->account()->nickname();
			currMessageFrom = session->account()->email();
		}
		else
		{
			if (msg->isConfMessage())
			{
				Contact* c = session->account()->contactList()->findContact(msg->getConfUser());
				if (c == 0)
				{
					nick = "<font color=red><b>" + msg->getConfUser();
					currMessageFrom = msg->getConfUser();
				}
				else
				{
					nick = "<font color=red><b>" + c->nickname();
					currMessageFrom = c->email();
				}
			}
			else
			{
				nick = "<font color=red><b>" + session->contact()->nickname() + "</b>";
				currMessageFrom = session->contact()->email();
			}
		}

		prompt = "";
		qDebug() << !session->account()->settings()->value("Messages/mergeMessages", true).toBool();
		if (currMessageFrom != lastMessageFrom || !session->account()->settings()->value("Messages/mergeMessages", true).toBool())
			prompt = nick + " (" + msg->dateTime().toString(m_account->settings()->value("Messages/DateMask", theRM.defDateFormat).toString()) + ") :</font><br>";
		lastMessageFrom = currMessageFrom;
	}

	cursor.insertHtml(prompt);
	if (session->contact()->isPhone())
		cursor.insertFragment(msg->documentFragment());
	else if (messageEditor->isIgnoreFont())
		cursor.insertFragment(msg->documentFragment(messageEditor->getDefFont(), messageEditor->getDefFontColor(), messageEditor->getDefBkColor()));
	else
		cursor.insertFragment(msg->documentFragment());
	cursor.insertHtml("<br>");
	cursor.movePosition(QTextCursor::End);

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	qApp->alert(this);

	if (msg->flags() & MESSAGE_FLAG_ALARM && msg->type() == Message::Incoming)
		shake();
	emit newMessage(this);
}

void ChatWindow::microblogChanged(QString text)
{
	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	QString prompt = tr("%1%2 (%3) added to microblog%4").arg("<font color=#000099><b>").arg(session->contact()->nickname()).arg(QDateTime::currentDateTime().toString(m_account->settings()->value("Messages/DateMask", theRM.defDateFormat).toString())).arg("</font><br>");

	cursor.insertHtml(prompt);
	cursor.insertHtml("<font size=80%>" + text + "</font>");
	cursor.insertHtml("<br>");
	cursor.movePosition(QTextCursor::End);

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
}

void ChatWindow::contactTyping()
{
	qDebug() << "ChatWindow::contactTyping()";
	statusBar->showMessage(tr("Contact is typing"), 7000);

	timer->start(7000);
	setContactStatusIcon("typing");
}

void ChatWindow::messageDelivered(bool really, Message* msg)
{
	if (!really)
	{
		if (msg->flags() & MESSAGE_FLAG_ALARM)
		{
			delete msg;
			return;
		}

		QTextCursor cursor = chatView->textCursor();
		QString prompt;

		prompt = msg->dateTime().time().toString() + " <b>" + session->contact()->nickname() + "</b>: <font color=red>" + tr("Message not delivered") + ": </font><br>";
		cursor.movePosition(QTextCursor::End);
		cursor.insertHtml(prompt);
		cursor.movePosition(QTextCursor::End);
		cursor.insertFragment(msg->documentFragment());
		prompt = "<a href=\"resend_" + QString::number(msg->getId()) + "\">" + tr("Resend") + "</a><br><br>";
		cursor.insertBlock();
		cursor.insertHtml(prompt);

		QScrollBar* vScrollBar = chatView->verticalScrollBar();
		vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

		if (session->contact()->isConference())
			theRM.getAudio()->play(STConference);
		else
			theRM.getAudio()->play(STMessage);

		emit newMessage(this);
	}
	else
		delete msg;
}

void ChatWindow::checkContactStatus(OnlineStatus status)
{
	qDebug() << "ChatWindow::checkContactStatus";
	Contact* contact = session->contact();
	if (contact->isConference())
		setWindowTitle(contact->nickname());
	else
		if (session->contact()->client() != "")
			setWindowTitle(contact->nickname().append(" - ").append(status.statusDescr()).append(" - ").append(session->contact()->client()));
		else
			setWindowTitle(contact->nickname().append(" - ").append(status.statusDescr()));
	setContactStatusIcon();
}

ChatWindow::~ ChatWindow()
{
	qDebug() << "ChatWindow::~ ChatWindow()";
}

void ChatWindow::appendSmsToView(QByteArray phoneNumber, QString text)
{
	show();
	smsEditor->unblockInput();
	
	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);
	
	QString prompt = QTime::currentTime().toString() + " <b>" + phoneNumber + "</b>: ";
	cursor.insertHtml(prompt);
	
	cursor.insertText(text);
	cursor.insertText("\n");

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	smsEditor->clear();

	qApp->alert(this);
	emit newMessage(this);
}

void ChatWindow::smsFailed()
{
	smsEditor->unblockInput();
}

void ChatWindow::shake()
{
	if (shakeTimeLine.isNull())
		return;
	
	savedX = x();
	savedY = y();

	theRM.getAudio()->play(STRing);

	shakeTimeLine->start();
}

void ChatWindow::shakeStep()
{
	int dx = (qrand() - RAND_MAX/2) % 5;
	int dy = (qrand() - RAND_MAX/2) % 5;
	move(savedX+dx, savedY+dy);
}

void ChatWindow::restorePosAfterShake()
{
	move(savedX, savedY);
}

void ChatWindow::wakeupContact()
{
	if (session->wakeupContact())
		shake();
}

void ChatWindow::saveTopAvatarBoxState(bool checked)
{
	m_account->settings()->setValue("ChatWindow/TopAvatarBoxState", checked);
}

void ChatWindow::saveBottomAvatarBoxState(bool checked)
{
	m_account->settings()->setValue("ChatWindow/BottomAvatarBoxState", checked);
}

void ChatWindow::saveConferenceListState(bool checked)
{
	m_account->settings()->setValue("ChatWindow/ConferenceListVisible", checked);
}

void ChatWindow::editorActivate()
{
	if (session->contact()->isPhone())
		slotEditorActivate(1);
	else
		slotEditorActivate(editorsWidget->currentIndex());
}

void ChatWindow::slotEditorActivate(int tab)
{
	if (tab == 0 && !session->contact()->isPhone())
		emit messageEditorActivate();
	else if (tab == 1)
		emit smsEditorActivate();
}

void ChatWindow::slotTimeout()
{
	qDebug() << "ChatWindow::slotTimeout()";
	setContactStatusIcon();
}

void ChatWindow::clearStatus()
{
	qDebug() << "ChatWindow::clearStatus()";
	statusBar->clearMessage();
	if (isNewMessage)
		setContactStatusIcon("message");
	else
		setContactStatusIcon();
	if (timer->isActive())
		timer->stop();
}

void ChatWindow::slotMakeRead()
{
	qDebug() << "ChatWindow::slotMakeRead" << isNewMessage;
	if (isNewMessage)
	{
		isNewMessage = false;
		setContactStatusIcon();
	}
}

/*void ChatWindow::fileTransferring(FileMessage* fmsg)
{
	qDebug() << "ChatWindow::fileTransferring";

	connect(fmsg, SIGNAL(startTransferring(quint32)), this, SLOT(transferStarted(quint32)));
	connect(fmsg, SIGNAL(fileTransferred(FileMessage::Status, QString, QString)), this, SLOT(slotFileTransferred(FileMessage::Status, QString, QString)));

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	cursor.insertBlock();
	cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
	cursor.insertHtml("<font color=green>" + tr("You offered to your interlocutor to get files.") + "</font><br>");
	cursor.insertHtml("<font color=green>" + fmsg->getFilesInHtml() + "</font><br>");
	cursor.insertHtml("<font color=green>" + tr("Basic size: ") + fmsg->getSizeInString(fmsg->getTotalSize()) + "</font>");

	cursor.insertBlock();
	cursor.insertHtml("<a href=\"ft_cancel_" + QString::number(fmsg->getSessionId()) + "\">" + tr("Cancel transferring") + "</a><br><br>");

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);	
}*/

void ChatWindow::fileTransferring(QList<QFileInfo> files)
{
	qDebug() << "ChatWindow::fileTransferring";

	transferStatus = FilesTransferring;
	fileMessageOut->setFileList(files);

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	cursor.insertBlock();
	cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
	cursor.insertHtml("<font color=green>" + tr("You offered to your interlocutor to get files.") + "</font><br>");
	cursor.insertHtml("<font color=green>" + fileMessageOut->getFilesInHtml() + "</font><br>");
	cursor.insertHtml("<font color=green>" + tr("Basic size: ") + fileMessageOut->getSizeInString(fileMessageOut->getTotalSize()) + "</font>");

	cursor.insertBlock();
	cursor.insertHtml("<a href=\"ft_cancel_" + QString::number(fileMessageOut->getSessionId()) + "\">" + tr("Cancel transferring") + "</a><br><br>");

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	//session->fileTransfer(fileMessageOut);
	fileMessageOut->sendFiles();
}

void ChatWindow::fileReceiving(quint32 totalSize, quint32 sessionId, QByteArray filesAnsi, QString filesUtf, QByteArray ips)
{
	qDebug() << "ChatWindow::fileReceiving";

	transferStatus = FilesReceiving;
/*	connect(fmsg, SIGNAL(startTransferring(quint32)), this, SLOT(transferStarted(quint32)));
	connect(fmsg, SIGNAL(fileTransferred(FileMessage::Status, QString, QString)), this, SLOT(slotFileTransferred(FileMessage::Status, QString, QString)));*/

	fileMessageIn->setParameters(totalSize, sessionId, filesAnsi, filesUtf, ips);

	cleanupCommandUrls();

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	cursor.insertBlock();
	cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
	cursor.insertHtml("<font color=green>" + tr("Your interlocutor offered to you to get files.") + "</font><br>");
	cursor.insertHtml("<font color=green>" + fileMessageIn->getFilesInHtml() + "</font><br>");
	cursor.insertHtml("<font color=green>" + tr("Basic size: ") + fileMessageIn->getSizeInString(fileMessageIn->getTotalSize()) + "</font>");

	cursor.insertBlock();
	cursor.insertHtml("<a href=\"ft_ok_" + QString::number(sessionId) + "\">" + tr("Receive") + "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	cursor.insertHtml("<a href=\"ft_save_as_" + QString::number(sessionId) + "\">" + tr("Save as...") + "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	cursor.insertHtml("<a href=\"ft_cancel_" + QString::number(sessionId) + "\">" + tr("Decline") + "</a><br><br>");

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	qApp->alert(this);
}

void ChatWindow::slotAnchorClicked(QUrl url)
{
	qDebug() << "ChatWindow::slotAnchorClicked" << url;

	if (url.path().contains("ft_ok"))
	{
		cleanupCommandUrls();
		messageEditor->receiveFiles();
		fileMessageIn->receiveFiles(url.path().mid(6).toUInt());
	}
	else if (url.path().contains("ft_save_as"))
	{
		//qDebug() << url.path().mid(11);
		cleanupCommandUrls();
	}
	else if (url.path().contains("ft_cancel"))
	{
		cleanupCommandUrls();
		transferringCancelled();
		//messageEditor->cancelTransferring(url.path().mid(10).toUInt());
		//session->cancelTransferring(url.path().mid(10).toUInt());
	}
	else if (url.path().contains("resend"))
	{
		//cleanupCommandUrls("resend");
		session->resendMessage(url.path().mid(7).toUInt());
	}
}

void ChatWindow::cleanupCommandUrls(QString str)
{
	qDebug() << "ChatWindow::clenupCommandUrls()";

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	QTextCharFormat nullFormat;
	nullFormat.setAnchor(false);
	nullFormat.setAnchorHref("");

	QTextBlock tb = cursor.block();
	cursor.setPosition(tb.position());

	while (tb.isValid())
	{
		if (cursor.charFormat().isAnchor() && (cursor.charFormat().anchorHref().left(7) == "file://" || cursor.charFormat().anchorHref().left(7) == "resend_"))
		{
			cursor.select(QTextCursor::LineUnderCursor);
			cursor.setCharFormat(nullFormat);
		}

		tb = tb.previous();
		cursor.setPosition(tb.position());

		//qDebug() << "In cycle";
	}
}

void ChatWindow::transferStarted(quint32 sessId)
{
	qDebug() << "ChatWindow::transferStarted()";

	cleanupCommandUrls();

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	cursor.insertBlock();
	cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
	cursor.insertHtml("<font color=green>" + tr("File transferring began") + "</font>");

	cursor.insertBlock();
	cursor.insertHtml("<a href=\"ft_cancel_" + QString::number(sessId) + "\">" + tr("Break") + "</a><br><br>");

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
}

void ChatWindow::slotFileTransferred(FileMessage::Status action, QString filesInHtml, QString destination)
{
	qDebug() << "ChatWindow::slotFileTransferred";

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);

	switch (action)
	{
		case FileMessage::TRANSFERRING_COMPLETE:
		case FileMessage::RECEIVING_COMPLETE:
			cleanupCommandUrls();
			cursor.insertBlock();
			cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
			cursor.insertHtml("<font color=green>" + tr("File transferring successful complete") + "</font><br>");
			if (action == FileMessage::RECEIVING_COMPLETE)
			{
				cursor.insertHtml("<font color=green>Received file(s):</font><br>");
				cursor.insertHtml("<font color=green>" + filesInHtml + "</font><br>");
				cursor.insertBlock();
				cursor.insertHtml("<a href=\"file://" + destination + "\">" + tr("Open folder") + "</a>");
				cursor.insertBlock();
				transferStatus = None;
			}
			break;

		case FileMessage::TRANSFER_ERROR:
		case FileMessage::RECEIVE_ERROR:
			cleanupCommandUrls();
			cursor.insertBlock();
			cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
			cursor.insertHtml("<font color=green>" + tr("Error occured while file transfering") + "</font><br>");
			transferStatus = None;
			break;

		case FileMessage::TRANSFER_CANCEL:
		case FileMessage::RECEIVE_CANCEL:
			cleanupCommandUrls();
			cursor.insertBlock();
			cursor.insertHtml("<font color=green>" + tr("Files transferring") + " (" + QDateTime::currentDateTime().time().toString() + ")</font><br>");
			cursor.insertHtml("<font color=green>" + tr("Transferring canceled") + "</font><br>");
			transferStatus = None;
			break;

		default:
			break;
	}

	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
}

void ChatWindow::setContactStatusIcon(QString type)
{
	setWindowIcon(session->contact()->chatWindowIcon(type));
	emit setMainWindowIconAndTitle(windowIcon(), this);
}

void ChatWindow::contactUpdated()
{
	if (session->contact()->isConference())
		setWindowTitle(session->contact()->nickname());
	else
		setWindowTitle(session->contact()->nickname() + " - " + session->contact()->status().statusDescr());
	sendButtonEnabledProcess();
	emit setMainWindowIconAndTitle(windowIcon(), this);
}

void ChatWindow::sendButtonEnabledProcess()
{
	qDebug() << "sendButtonEnabledProcess()";

	if (!m_account->onlineStatus().connected() || (session->contact()->isConference() && session->contact()->isTemporary()))
		sendButton->setEnabled(false);
	else
		sendButton->setEnabled(true);
}

void ChatWindow::showBroadcastPanel(bool visible)
{
	broadcastPanel->setVisible(visible);
}

void ChatWindow::contactIgnored()
{
	if (session->contact()->isIgnored())
		emit contactIgnored(true);
	else
		emit contactIgnored(false);
}

void ChatWindow::transferringCancelled()
{
	qDebug() << Q_FUNC_INFO << transferStatus;
	if (transferStatus == FilesTransferring)
		fileMessageOut->cancelTransferring(fileMessageOut->getSessionId());
	else if (transferStatus == FilesReceiving)
		fileMessageIn->cancelTransferring(fileMessageIn->getSessionId());
	//transferStatus = None;
}
