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

ChatWindow::ChatWindow(Account* account, ChatSession* s)
	: m_account(account), session(s), messageEditor(0), smsEditor(0)
{
	QIcon windowIcon;
	windowIcon.addFile(":icons/message_32x32.png");
	windowIcon.addFile(":icons/message_16x16.png");
	setWindowIcon(windowIcon);

	splitter = new QSplitter(Qt::Vertical, this);
	
	QWidget* chatWidget = new QWidget;
	QHBoxLayout* chatWidgetLayout = new QHBoxLayout;
	chatWidgetLayout->setContentsMargins(1, 1, 1, 1);
	chatWidgetLayout->setSpacing(1);
	chatView = new AnimatedTextBrowser;	
	chatWidgetLayout->addWidget(chatView);
	if (!s->contact()->isPhone())
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
		
		QTabWidget* editorsWidget = new QTabWidget;
		editorsWidget->setStyleSheet("QTabWidget::pane { border: 0px; } QTabWidget::tab-bar { left: 1px; } QTabBar::tab { padding: 0px 32px 0px 32px; font: bold 9px; }");
		editorsWidget->setTabPosition(QTabWidget::South);
		editorsWidget->setTabShape(QTabWidget::Triangular);
		
		messageEditor = new MessageEditor(m_account, session->contact());
		connect(messageEditor, SIGNAL(sendPressed()), this, SLOT(sendMessage()));
		connect(messageEditor, SIGNAL(textChanged()), session, SLOT(sendTyping()));
		connect(messageEditor, SIGNAL(wakeupPressed()), this, SLOT(wakeupContact()));
		
		smsEditor = new SmsEditor(m_account, session->contact());
		connect(smsEditor, SIGNAL(sendPressed()), this, SLOT(sendSms()));
		
		editorsWidget->addTab(messageEditor, tr("Text"));
		editorsWidget->addTab(smsEditor, tr("SMS"));
		
		AvatarBoxWithHandle* avatarBoxWithHandle = new AvatarBoxWithHandle(m_account->avatarsPath(), m_account->email());
		avatarBoxWithHandle->toggle(m_account->settings()->value("ChatWindow/BottomAvatarBoxState", false).toBool());
		connect(avatarBoxWithHandle, SIGNAL(toggled(bool)), SLOT(saveBottomAvatarBoxState(bool)));
		
		bottomWidgetLayout->addWidget(editorsWidget);
		bottomWidgetLayout->addWidget(avatarBoxWithHandle);
		
		bottomWidget->setLayout(bottomWidgetLayout);
		
		splitter->addWidget(bottomWidget);
		
		connect(session->contact(), SIGNAL(statusChanged(OnlineStatus)), this, SLOT(checkContactStatus(OnlineStatus)));
		checkContactStatus(session->contact()->status());
	}
	else
	{
		setWindowTitle(session->contact()->nickname());
		
		smsEditor = new SmsEditor(s->account(), s->contact());
		connect(smsEditor, SIGNAL(sendPressed()), this, SLOT(sendSms()));
		splitter->addWidget(smsEditor);
	}
	
	splitter->setStretchFactor(0, 7);
	splitter->setStretchFactor(1, 3);
	
	setCentralWidget(splitter);
	
	QStatusBar* sb = statusBar();
	QPushButton* sendButton = new QPushButton(tr("Send"), this);
	connect(sendButton, SIGNAL(clicked(bool)), this, SLOT(send()));

	connect(session->contact(), SIGNAL(typing()), this, SLOT(contactTyping()));
	connect(session, SIGNAL(messageDelivered(bool)), this, SLOT(messageDelivered(bool)));
	connect(session, SIGNAL(messageAppended(const Message*)), this, SLOT(appendMessageToView(const Message*)));
	connect(session, SIGNAL(smsDelivered(QByteArray,QString)), this, SLOT(appendSmsToView(QByteArray, QString)));
	connect(session, SIGNAL(smsFailed()), this, SLOT(smsFailed()));

	sb->addPermanentWidget(sendButton);
	sb->setSizeGripEnabled(false);
	
	resize(750, 550);

	ChatSession::MessageIterator it = session->messagesBegin();
	for (; it != session->messagesEnd(); ++it)
	{
		appendMessageToView(*it);
	}

	shakeTimeLine = new QTimeLine(2000, this);
	shakeTimeLine->setFrameRange(0, 200);
	connect(shakeTimeLine, SIGNAL(frameChanged(int)), SLOT(shakeStep()));
	connect(shakeTimeLine, SIGNAL(finished()), SLOT(restorePosAfterShake()));
}

void ChatWindow::send()
{
	if (messageEditor && messageEditor->isVisible() && !messageEditor->isBlocked())
		sendMessage();
	else if (smsEditor && smsEditor->isVisible() && !smsEditor->isBlocked())
		sendSms();
}

quint32 ChatWindow::sendMessage()
{
	if (!m_account->onlineStatus().connected())
		return 0;
	
	PlainTextExporter textExporter(messageEditor->document());
	QString messageText = textExporter.toText();
	
	if (messageText.isEmpty())
		return 0;
	
	RtfExporter rtfExporter(messageEditor->document());
	QByteArray messageRtf = rtfExporter.toRtf();
	
	messageEditor->blockInput();
	
	return session->sendMessage(messageText, messageRtf);
}

quint32 ChatWindow::sendSms()
{
	if (!m_account->onlineStatus().connected())
		return 0;

	QString text = smsEditor->text();
	QByteArray number = smsEditor->phoneNumber();
	
	smsEditor->blockInput();
	return session->sendSms(number, text);
}

void ChatWindow::appendMessageToView(const Message* msg)
{
	qDebug() << "ChatWindow::appendMessageToView";
	statusBar()->clearMessage();
	show();

	QTextCursor cursor = chatView->textCursor();
	cursor.movePosition(QTextCursor::End);
	
	QString prompt;

	if (msg->flags() & MESSAGE_FLAG_SMS)
	{
		prompt = msg->dateTime().time().toString() + " <b>" + tr("Sms from number") + " " + msg->rtfText() + "</b>: ";
	}
	else if (msg->flags() & MESSAGE_FLAG_SMS_STATUS)
	{
		prompt = msg->dateTime().time().toString() + " <b>" + tr("Sms status for number") + " " + msg->rtfText() + "</b>: ";
	}
	else if (msg->flags() & MESSAGE_FLAG_BELL)
	{
		prompt = "<font color=green>" + msg->dateTime().time().toString() + " <b>" + tr("Alarm clock:")  + " <b></font>";
	}
	else
	{
		QString nick;
		if (msg->type() == Message::Outgoing)
			nick = session->account()->nickname();
		else
			nick = session->contact()->nickname();
		
		prompt = msg->dateTime().time().toString() + " <b>" + nick + "</b>: ";
	}
	
	cursor.insertHtml(prompt);
	cursor.insertFragment(msg->documentFragment());
	cursor.movePosition(QTextCursor::End);
	
	QScrollBar* vScrollBar = chatView->verticalScrollBar();
	vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	qApp->alert(this);

	if (msg->flags() & MESSAGE_FLAG_BELL && msg->type() == Message::Incoming)
		shake();
}

void ChatWindow::contactTyping()
{
	statusBar()->showMessage(tr("Contact is typing"), 10000);
}

void ChatWindow::messageDelivered(bool really)
{
	messageEditor->unblockInput();
	
	if (really)
	{
		/*QTextCursor cursor(messageEditor->document());
		cursor.select(QTextCursor::Document);
		cursor.removeSelectedText();*/
		messageEditor->clear();
		
		statusBar()->clearMessage();
	}
}

void ChatWindow::checkContactStatus(OnlineStatus status)
{
	Contact* contact = session->contact();
	setWindowTitle(contact->email() + " - " + status.description());
}

ChatWindow::~ ChatWindow()
{
	qDebug() << "ChatWindow::~ ChatWindow()";
}

void ChatWindow::appendSmsToView(QByteArray phoneNumber, QString text)
{
	show();
	statusBar()->clearMessage();
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
