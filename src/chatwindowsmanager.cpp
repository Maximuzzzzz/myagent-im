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

#include "chatwindowsmanager.h"

#include <QDebug>
#include <QTabWidget>
#include <QPushButton>

#include "account.h"
#include "contact.h"
#include "chatwindow.h"
#include "chatsession.h"
#include "resourcemanager.h"

ChatWindowsManager::ChatWindowsManager(Account* account, QObject *parent)
	: QObject(parent), m_account(account)
{
	//useTabs = theRM.settings()->value("Windows/UseTabs", true).toBool();
	useTabs = m_account->settings()->value("Windows/UseTabs", true).toBool();

	if (useTabs)
		loadMainWindow();
}

ChatWindowsManager::~ChatWindowsManager()
{
	qDebug() << "ChatWindowsManager::~ChatWindowsManager()";
	QList<ChatWindow*> wnds = windows.values();
	qDebug() << "ChatWindowsManager::~ChatWindowsManager() windows num = " << wnds.size();
	qDeleteAll(wnds);
	qDebug() << "ChatWindowsManager::~ChatWindowsManager() finished";
}

ChatWindow* ChatWindowsManager::getWindow(ChatSession* session)
{
	qDebug() << "ChatWindowsManager::getWindow";
	ChatWindow* wnd = windows.value(session, NULL);
	if (wnd == NULL)
		wnd = createWindow(session);

	if (useTabs)
	{
		/*if (tabs->count() == 0)
			mainWindow->setGeometry(wnd->geometry());*/

		QHash<ChatSession*, int>::const_iterator it = tabHash.find(session);
		if (it.key() == session)
		{
			//if (!mainWindow->isVisible())
				tabs->setCurrentIndex(tabHash.value(session));
			tabs->setTabIcon(tabHash.value(session), wnd->windowIcon());
		}
		else
		{
			int tabIndex = tabs->addTab(wnd, wnd->windowIcon(), session->contact()->nickname());
			tabHash.insert(session, tabIndex);
			tabs->setCurrentIndex(tabIndex);
		}
		changeTab(tabs->currentIndex());

		mainWindow->show();
	}

	return wnd;
}

void ChatWindowsManager::slotRemoveTab(int tab)
{
	qDebug() << "ChatWindowsManager::slotRemoveTab";
	ChatSession* s = tabHash.key(tab);
	tabs->removeTab(tabHash.value(s));
	tabHash = deleteFromHash(tabHash.take(s));
	if (tabs->count() == 0)
		mainWindow->close();
}

void ChatWindowsManager::removeWindow(QObject* session)
{
	qDebug() << "ChatWindowsManager::removeWindow";
	ChatSession* s = static_cast<ChatSession*>(session);
	ChatWindow* wnd = windows.take(s);
	if (useTabs)
		slotRemoveTab(tabHash.value(s));
	delete wnd;
}

ChatWindow* ChatWindowsManager::createWindow(ChatSession* session)
{
	qDebug() << "ChatWindowsManager::createWindow";
	ChatWindow* wnd = new ChatWindow(m_account, session);
	windows.insert(session, wnd);
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(removeWindow(QObject*)));
	if (useTabs)
	{
		int tabIndex = tabs->addTab(wnd, wnd->windowIcon(), session->contact()->nickname());
		tabHash.insert(session, tabIndex);
		if (!mainWindow->isVisible())
			tabs->setCurrentIndex(tabIndex);
		changeTab(tabs->currentIndex());
		connect(wnd, SIGNAL(setMainWindowIconAndTitle(QIcon, ChatWindow*)), this, SLOT(changeIconAndTitle(QIcon, ChatWindow*)));
		connect(wnd, SIGNAL(newMessage(ChatWindow*)), this, SLOT(mainWindowActivate(ChatWindow*)));
		mainWindow->show();
	}

	return wnd;
}

void ChatWindowsManager::changeIconAndTitle(QIcon icon, ChatWindow* sender)
{
	ChatSession* s = windows.key(sender);
	QHash<ChatSession*, int>::const_iterator it = tabHash.find(s);
	if (it.key() == s)
	{
		int index = tabHash.value(s);
		tabs->setTabIcon(index, icon);
		if (index == tabs->currentIndex())
			changeTab(tabs->currentIndex());
	}
}

void ChatWindowsManager::changeTab(int index)
{
	qDebug() << "ChatWindowsManager::changeTab";
	if (tabs->count() > 0)
	{
		ChatSession* s = tabHash.key(tabs->currentIndex());
		ChatWindow* wnd = windows.value(s, NULL);
		if (wnd != NULL)
		{
			wnd->slotMakeRead();
			wnd->editorActivate();
		}
		mainWindow->setWindowIcon(tabs->tabIcon(index));
		mainWindow->setWindowTitle(tabs->currentWidget()->windowTitle());
	}
}

void ChatWindowsManager::mainWindowActivate(ChatWindow* wnd)
{
	qDebug() << "ChatWindowsManager::mainWindowActivate";

	ChatSession* s = windows.key(wnd);
	QHash<ChatSession*, int>::const_iterator it = tabHash.find(s);
	int tabIndex;
	if (it.key() == s)
	{
		qDebug() << "(it.key() == s)";
		tabIndex = tabHash.value(s);
		if (!mainWindow->isVisible())
		{
			tabs->setCurrentIndex(tabIndex);
			changeTab(tabs->currentIndex());
			mainWindow->show();
		}
	}
	else
	{
		qDebug() << "(it.key() != s)";
		tabIndex = tabs->addTab(wnd, wnd->windowIcon(), s->contact()->nickname());
		tabHash.insert(s, tabIndex);
		if (!mainWindow->isVisible())
			tabs->setCurrentIndex(tabIndex);
		changeTab(tabs->currentIndex());
		mainWindow->show();
	}
}

QHash<ChatSession*, int> ChatWindowsManager::deleteFromHash(int indexRemoved)
{
	qDebug() << "ChatWindowsManager::updateHash(" << indexRemoved << ")";
	QHash<ChatSession*, int> tmpHash;
	QHash<ChatSession*, int>::iterator it = tabHash.begin();
	while (it != tabHash.end())
	{
		int currIndex = it.value();
		ChatSession* s = tabHash.key(currIndex);
		if (currIndex > indexRemoved)
			tmpHash.insert(s, currIndex - 1);
		else
			tmpHash.insert(s, currIndex);
		it++;
	}
	return tmpHash;
}

void ChatWindowsManager::loadMainWindow()
{
	qDebug() << "ChatWindowsManager::loadMainWindow()";
	mainWindow = new QMainWindow();

	QIcon windowIcon;
	windowIcon.addFile(":icons/message_32x32.png");
	windowIcon.addFile(":icons/message_16x16.png");
	mainWindow->setWindowIcon(windowIcon);

	tabs = new QTabWidget;
	tabs->setStyleSheet("QTabWidget::pane { border: 0px; } QTabWidget::tab-bar { left: 1px; } QTabBar::tab { padding: 5px 5px 5px 5px; font: bold 10px; }");
	tabs->setTabsClosable(true);
	tabs->setTabPosition(QTabWidget::North);
	tabs->setTabShape(QTabWidget::Triangular);
	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(slotRemoveTab(int)));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

	connect(mainWindow, SIGNAL(showEvent()), this, SLOT(onShowWindow()));

	mainWindow->setCentralWidget(tabs);
}

bool ChatWindowsManager::isAnyWindowVisible()
{
	if (useTabs)
		return mainWindow->isVisible();
	else
	{
		QHash<ChatSession*, ChatWindow*>::iterator it = windows.begin();
		while (it != windows.end())
		{
			ChatWindow* wnd = it.value();
			if (wnd->isVisible())
				return true;
			it++;
		}
		return false;
	}
}

void ChatWindowsManager::reloadStatus(bool doUseTabs)
{
	qDebug() << "ChatWindowsManager::reloadStatus()";
	if (doUseTabs == useTabs)
		return;
	if (!doUseTabs)
	{
		tabHash.clear();
		windows.clear();
		delete tabs;
		delete mainWindow;
	}
	else
	{
		QHash<ChatSession*, ChatWindow*>::iterator it = windows.begin();
		while (it != windows.end())
		{
			ChatWindow* wnd = it.value();
			delete wnd;
			it++;
		}
		windows.clear();
		loadMainWindow();
	}
	useTabs = doUseTabs;
}

void ChatWindowsManager::onShowWindow()
{
	changeTab(tabs->currentIndex());
}
