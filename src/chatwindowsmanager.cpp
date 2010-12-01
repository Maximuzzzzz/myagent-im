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
#include <QVBoxLayout>

#include "account.h"
#include "contact.h"
#include "chatwindow.h"
#include "chatsession.h"
#include "resourcemanager.h"

ChatWindowsManager::ChatWindowsManager(Account* account, QObject *parent)
	: QObject(parent), m_account(account)
{
	connect(m_account->chatsManager(), SIGNAL(sessionInitialized(ChatSession*)), this, SLOT(createWindow(ChatSession*)));
	useTabs = m_account->settings()->value("Windows/UseTabs", true).toBool();

	if (useTabs)
		loadMainWindow();
}

ChatWindowsManager::~ChatWindowsManager()
{
	qDebug() << Q_FUNC_INFO << "{";
	QList<ChatWindow*> wnds = windows.values();
	qDebug() << Q_FUNC_INFO << "windows num = " << wnds.size();
	qDeleteAll(wnds);
	qDebug() << Q_FUNC_INFO << "}";
}

ChatWindow* ChatWindowsManager::getWindow(ChatSession* session)
{
	qDebug() << Q_FUNC_INFO;
	ChatWindow* wnd = windows.value(session, NULL);
	if (wnd == NULL)
		wnd = createWindow(session);

	if (useTabs)
	{
		/*if (tabs->count() == 0)
			tabsWindow->setGeometry(wnd->geometry());*/

		int tabIndex = tabs->indexOf(wnd);
		if (tabIndex != -1)
		{
			//if (!tabsWindow->isVisible())
			tabs->setCurrentIndex(tabIndex);
			tabs->setTabIcon(tabIndex, wnd->windowIcon());
		}
		else
		{
			tabIndex = tabs->addTab(wnd, wnd->windowIcon(), session->contact()->nickname());
			tabs->setCurrentIndex(tabIndex);
		}
		changeTab(tabs->currentIndex());

		tabsWindow->show();
	}

	return wnd;
}

void ChatWindowsManager::slotRemoveTab(int tab)
{
	qDebug() << Q_FUNC_INFO;

	tabs->removeTab(tab);

	if (tabs->count() == 0)
		tabsWindow->close();
}

void ChatWindowsManager::removeWindow(QObject* session)
{
	qDebug() << Q_FUNC_INFO;
	ChatSession* s = static_cast<ChatSession*>(session);
	ChatWindow* wnd = windows.take(s);
	if (useTabs)
	{
		int tabIndex = tabs->indexOf(wnd);
		if (tabIndex != -1)
			slotRemoveTab(tabIndex);
	}
	delete wnd;
}

ChatWindow* ChatWindowsManager::createWindow(ChatSession* session)
{
	qDebug() << Q_FUNC_INFO;
	ChatWindow* wnd = new ChatWindow(m_account, session);
	windows.insert(session, wnd);
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(removeWindow(QObject*)));
	connect(wnd, SIGNAL(setIgnore(bool)), this, SIGNAL(ignoreSet(bool)));
	connect(this, SIGNAL(ignoreSet(bool)), wnd, SIGNAL(ignoreSet(bool)));
	connect(wnd, SIGNAL(setSignalCheckSpelling(bool)), this, SIGNAL(signalCheckSpellingSet(bool)));
	connect(this, SIGNAL(signalCheckSpellingSet(bool)), wnd, SIGNAL(signalCheckSpellingSet(bool)));
	if (useTabs)
	{
		int tabIndex = tabs->addTab(wnd, wnd->windowIcon(), session->contact()->nickname());

		if (!tabsWindow->isVisible())
			tabs->setCurrentIndex(tabIndex);

		changeTab(tabs->currentIndex());
		connect(wnd, SIGNAL(setMainWindowIconAndTitle(QIcon, ChatWindow*)), this, SLOT(changeIconAndTitle(QIcon, ChatWindow*)));
		connect(wnd, SIGNAL(newMessage(ChatWindow*)), this, SLOT(mainWindowActivate(ChatWindow*)));
		tabsWindow->show();
	}
	else
		wnd->resize(750, 550);

	return wnd;
}

void ChatWindowsManager::changeIconAndTitle(const QIcon& icon, ChatWindow* sender)
{
	qDebug() << "ChatWindowsManager::changeIconAndTitle";
	int tabIndex = tabs->indexOf(sender);
	if (tabIndex != -1)
	{
		ChatSession* session = windows.key(sender);
		qDebug() << "Nickname" << session->contact()->nickname();
		tabs->setTabIcon(tabIndex, icon);
		if (!(session->contact()->isConference() && session->contact()->isTemporary()))
			tabs->setTabText(tabIndex, session->contact()->nickname());
		if (tabIndex == tabs->currentIndex())
			changeTab(tabIndex);
	}
}

void ChatWindowsManager::changeTab(int index)
{
	qDebug() << Q_FUNC_INFO << index;
	if (tabs->count() > 0)
	{
		ChatWindow* wnd = static_cast<ChatWindow*>(tabs->widget(index));

		if (wnd != NULL)
		{
			wnd->slotMakeRead();
			wnd->editorActivate();
		}
		tabsWindow->setWindowIcon(tabs->tabIcon(index));
		tabsWindow->setWindowTitle(tabs->currentWidget()->windowTitle());
	}
}

void ChatWindowsManager::mainWindowActivate(ChatWindow* wnd)
{
	qDebug() << Q_FUNC_INFO << wnd << "{";

	int tabIndex = tabs->indexOf(wnd);
	if (tabIndex != -1)
	{
		if (!tabsWindow->isVisible())
		{
			tabs->setCurrentIndex(tabIndex);
			changeTab(tabs->currentIndex());
			tabsWindow->show();
		}
	}
	else
	{
		ChatSession* s = windows.key(wnd);
		tabIndex = tabs->addTab(wnd, wnd->windowIcon(), s->contact()->nickname());
		if (!tabsWindow->isVisible())
			tabs->setCurrentIndex(tabIndex);
		changeTab(tabs->currentIndex());
		tabsWindow->show();
	}

	qDebug() << Q_FUNC_INFO << "}";
}

void ChatWindowsManager::loadMainWindow()
{
	qDebug() << Q_FUNC_INFO;
	tabsWindow = new QWidget;

	QIcon windowIcon;
	windowIcon.addFile(":icons/message_32x32.png");
	windowIcon.addFile(":icons/message_16x16.png");
	tabsWindow->setWindowIcon(windowIcon);

	tabs = new QTabWidget;
	tabs->setStyleSheet("QTabWidget::pane { border: 0px; } QTabWidget::tab-bar { left: 1px; } QTabBar::tab { padding: 5px 5px 5px 5px; font: bold 10px; }");
	tabs->setTabsClosable(true);
	tabs->setTabPosition(QTabWidget::North);
	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(slotRemoveTab(int)));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

	//connect(tabsWindow, SIGNAL(showEvent()), this, SLOT(onShowWindow())); // looks strange

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(1, 1, 1, 1);
	layout->addWidget(tabs);
	tabsWindow->setLayout(layout);

	tabsWindow->resize(750, 550);
}

bool ChatWindowsManager::isAnyWindowVisible()
{
	if (useTabs)
		return tabsWindow->isVisible();
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
	qDebug() << Q_FUNC_INFO << "doUseTabs =" << doUseTabs;
	if (doUseTabs == useTabs)
		return;
	if (!doUseTabs)
	{
		windows.clear();
		delete tabs;
		delete tabsWindow;
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
	// seems to be unused
	changeTab(tabs->currentIndex());
}

void ChatWindowsManager::raiseWindow(ChatWindow* wnd)
{
	if (!useTabs)
	{
		if (wnd->isMinimized())
			wnd->showNormal();
		wnd->show();
		wnd->raise();
		wnd->activateWindow();
	}
}
