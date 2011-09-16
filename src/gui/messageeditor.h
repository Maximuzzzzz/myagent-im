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

#ifndef MESSAGEEDITOR_H
#define MESSAGEEDITOR_H

#include <QWidget>
#include <QTextCharFormat>
#include <QPointer>
#include <QFileInfo>
#include <QProgressBar>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>

#include "onlinestatus.h"
#include "protocol/mrim/filemessage.h"

class QToolBar;
class MessageEdit;
class QAction;
class QToolButton;
class EmoticonSelector;
class MultSelector;
class ToolButton;
class QComboBox;
class QLabel;

class Account;
class Contact;

class MessageEditor : public QWidget
{
Q_OBJECT
public:
	MessageEditor(Account* account, Contact* contact, EmoticonSelector* emSel, MultSelector* multSel, QWidget* parent = 0);
	~MessageEditor();

	bool isBlocked();

	QTextDocument* document() const;

	QFont getDefFont() { return defaultFont; }
	QColor getDefFontColor() { return defaultFontColor; }
	QColor getDefBkColor() { return defaultBkColor; }

public Q_SLOTS:
	void clear();
	void receiveFiles();
	void cancelTransferring(quint32 sessId = 0);
	bool isIgnoreFont();
	void slotIgnoreSet(bool ignore);
	void setCheckSpelling(bool on);

Q_SIGNALS:
	void textChanged();
	void sendPressed();
	void wakeupPressed();
	void filesTransfer(QList<QFileInfo> files);
	void filesReceiveAck(FileMessage* fmsg, quint32 status);
	void signalProxy(quint32 idRequest, quint32 dataType, QByteArray userData, QByteArray lpsIpPort, quint32 sessionId);
	void signalProxyAck(QByteArray mirrorIps);
	void setIgnore(bool b);
	void setSignalCheckSpelling(bool b);
	void showBroadcastPanel(bool visible);
	void transferringCancel();
	void showMult(const QString& id);

protected:
	virtual void hideEvent(QHideEvent* event);
	bool eventFilter(QObject *obj, QEvent *ev);
	virtual bool event(QEvent* event);

private Q_SLOTS:
	void chooseFont();
	void chooseFontColor();
	void chooseBackgroundColor();

	void setCurrentFont(const QFont& f);
	void setTextColor(const QColor& c);
	void setTextBackgroundColor(const QColor& c);

	void setFontBold(bool b);
	void setFontItalic(bool b);
	void setFontUnderline(bool b);

	void insertEmoticon(MessageEditor* editor, const QString& id);
	void insertFlash(MessageEditor* editor, const QString& id);

	void updateFormatActions();

	void triggerEmoticonSelector();
	void triggerMultSelector();

	void addFile();
	void deleteFile();
	void sendFiles();
	void fileTransfer(bool checked);

	void slotCurrentCharFormatChanged(const QTextCharFormat& f);

	void checkContactStatus(OnlineStatus status);

	void readSettings();
	void writeSettings();
	void messageEditorActivate();

	void slotProgress(FileMessage::Status action, int percentage);

private:
	QAction* addToolAction(const QIcon& icon, const QObject* receiver, const char* method);
	QIcon toolIcon(const QString& toolName) const;
	void createToolBar();
	
	void mergeMyCharFormat(const QTextCharFormat& fmt);

	void createFileTransferBar();
	QIcon fileTransferIcon(const QString& toolName) const;
	QToolButton* createFileTransferToolButton(const QIcon& icon, const QObject* receiver, const char* method);

private:
	QPointer<Account> m_account;
	QPointer<Contact> m_contact;

	QToolBar* toolBar;
	QAction* fileTransferAction;
	QAction* broadcastAction;
	MessageEdit* messageEdit;

	QAction* spellAction;

	QAction* fontAction;
	QAction* colorAction;
	QAction* bkColorAction;
	
	QAction* boldFontAction;
	QAction* italicFontAction;
	QAction* underlinedFontAction;
	QAction* ignoreAction;
	
	QAction* smilesAction;
	QAction* multsAction;

	ToolButton* wakeupButton;

	EmoticonSelector* emoticonSelector;
	MultSelector* multSelector;

	QTextCharFormat lastUserFormat;

	QToolBar* fileTransferBar;
	QToolBar* fileProcessBar;
	QComboBox* filesBox;
	QLabel* bytesLabel;
	QLabel* fileStatusLabel;
	QProgressBar* fileProgress;

	QList<QFileInfo> fileList;

	QToolButton* plus;
	QToolButton* minus;
	QToolButton* send;
	QPushButton* cancel;

	quint32 totalSize;
	double progressStep;

	quint32 sessId;

	QTcpSocket* tcpSocket;
	QTcpServer* tcpServer;

	QFile* m_currentFile;

	QFont defaultFont;
	QColor defaultFontColor;
	QColor defaultBkColor;
};

#endif
