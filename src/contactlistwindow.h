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

#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QLabel>

#include <QWidget>
#include <QModelIndex>
#include <QEvent>

#include "onlinestatus.h"
#include "idle/idle.h"
#include "statusbarwidget.h"
#include "statuseditor.h"

class Account;
class Contact;
class QTreeView;
class StatusButton;
class ChatWindowsManager;
class SystemTrayIcon;
class MainMenuButton;

class ContactListWindow : public QWidget
{
Q_OBJECT
public:
	ContactListWindow(Account* account);
	~ContactListWindow();

	void extHide();
	void extShow();

	QAction* myWorldAction() { return m_myWorldAction; }
	QAction* myBlogAction() { return m_myBlogAction; }
	QAction* myPhotosAction() { return m_myPhotosAction; }
	QAction* myVideosAction() { return m_myVideosAction; }

	ChatWindowsManager* getChatWindowsManager() { return chatWindowsManager; }

	enum Widgets
	{
		MicroBlog
	};

public slots:
	void openMail();
	void visibleWidget(Widgets w, bool st);

protected:
	virtual void closeEvent(QCloseEvent* event);

private slots:
	void slotContactItemActivated(Contact* contact);
	void slotLoginRejected(QString reason);
	void slotLoggedOut(quint32 reason);
	void slotContactAsksAuthorization(const QByteArray& email, const QString& nickname, const QString& message);
	void authorizeContact();
	void openMailRuProject();
	void openMailRuUrlEnd(quint32 status, bool timeout);
	void checkAutoAwayTime(int seconds);
	void openStatusEditor();
	void sendMicrotext(const QString& microText);
	void slotMicroblogChanged(QString microText);
	void slotSetOnlineStatus(OnlineStatus status);
	void slotConnectionError(QString mess);

private:
	void writeSettings();
	void readSettings();

	QIcon projectIcon(const QString& project) const;
	void createActions();

	void openMailRuUrl(const QString& url);

	Account* m_account;
	QTreeView* contactsTreeView;
	StatusButton* statusButton;
	MainMenuButton* mainMenuButton;
	ChatWindowsManager* chatWindowsManager;
	StatusBarWidget* statusBar;
	StatusEditor* statusEditor;

	QAction* m_myWorldAction;
	QAction* m_myBlogAction;
	QAction* m_myPhotosAction;
	QAction* m_myVideosAction;

	QPoint m_pos;
	QSize m_size;
	SystemTrayIcon* sysTray;

	Idle idleDetector;
};

#endif
