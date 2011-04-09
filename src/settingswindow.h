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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include "chatwindowsmanager.h"
#include "account.h"
#include "contactlistwindow.h"

#define SHOW_COMMON_PAGE	0x01
#define SHOW_MESSAGES_PAGE	0x02
#define	SHOW_WINDOWS_PAGE	0x04
#define SHOW_AUDIO_PAGE		0x08
#define SHOW_VIEW_PAGE		0x10
#define SHOW_CONNECTION_PAGE	0x20

class Account;

class QListWidget;
class QStackedWidget;

class QCheckBox;
class QRadioButton;
class QLineEdit;

class SettingsWindow : public QWidget
{
Q_OBJECT
public:
	SettingsWindow(Account* account, ContactListWindow* clw);
	SettingsWindow(quint8 flags);
	~SettingsWindow();

	void setChatWindowsManager(ChatWindowsManager* cwm);

signals:
	void statusesCountChanged();

private slots:
	void saveSettings();
	void setProxyType();
	void showFormatHelp();

	void init();

private:
	void createCommonPage();
	void saveCommonSettings();

	void createMessagesPage();
	bool saveMessagesSettings();

	void createWindowsPage();
	bool saveWindowsSettings();

	void createAudioPage();
	void saveAudioSettings();

	void createViewPage();
	void saveViewSettings();

	void createConnectionPage();
	void saveConnectionSettings();

	QListWidget* listWidget;
	QStackedWidget* pagesWidget;

	// common page widgets
	QCheckBox* awayCheckBox;
	QLineEdit* minutesEdit;
	QLineEdit* onlineStatusesCount;

	//messages page widgets
	QRadioButton* enterButton;
	QRadioButton* doubleEnterButton;
	QRadioButton* ctrlEnterButton;
	QCheckBox* altSButton;
	QLineEdit* dateTimeFormat;
	QPushButton* helpFormat;
	QCheckBox* mergeMessagesCheckBox;

	//windows page widgets
	QCheckBox* tabWindows;

	//audio page widgets
	QCheckBox* enableSounds;

	//view page widgets
	QCheckBox* enableMicroBlog;

	//connection page
	QRadioButton* typeSimple;
	QRadioButton* typeSocks5;
	QRadioButton* typeHttp;
	QRadioButton* typeHttps;
	QCheckBox* authCheck;
	QLineEdit* host;
	QLineEdit* port;
	QLineEdit* user;
	QLineEdit* password;

	ChatWindowsManager* chatWindowsManager;

	Account* m_account;

	quint8 m_flags;

	ContactListWindow* m_clw;

};

#endif
