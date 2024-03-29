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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef CONTACTLISTWINDOW_H
#define CONTACTLISTWINDOW_H

#include <QWidget>

#include "idle/idle.h"
#include "core/onlinestatus.h"

class QTreeView;

class Account;
class Contact;
class StatusButton;
class ChatWindowsManager;
class SystemTrayIcon;
class MainMenuButton;
class StatusBarWidget;
class StatusEditor;

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

public Q_SLOTS:
	void openMail();
	void visibleWidget(Widgets w, bool st);

protected:
	virtual void closeEvent(QCloseEvent* event);

private Q_SLOTS:
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
	void slotMicroblogChanged(QByteArray email, QString microText, const QDateTime& dt);
	void slotSetOnlineStatus(OnlineStatus status);
	void slotConnectionError(QString mess);
	void slotAccountNicknameChanged();

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
