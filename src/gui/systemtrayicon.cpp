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

#include "systemtrayicon.h"

#include <QDebug>

#include <QApplication>
#include <QActionGroup>
#include <QWidgetAction>
#include <QLabel>
#include <QProcess>

#include "onlinestatus.h"
#include "resourcemanager.h"
#include "contactlistwindow.h"
#include "account.h"
#include "protocol/mrim/mrimclient.h"
#include "audio.h"
#include "contact.h"

SystemTrayIcon::SystemTrayIcon(Account* a, ContactListWindow* w, StatusMenu* sm)
 : QSystemTrayIcon(w), mainWindow(w), account(a)
{
	m_popupsExists = false;

	updateTooltip();
	setOnlineStatus(OnlineStatus::offline);

	popsStack = new PopupWindowsStack(this);
	connect(popsStack, SIGNAL(mouseEntered()), this, SLOT(popsStackMouseEntered()));
	connect(popsStack, SIGNAL(mouseLeaved()), this, SLOT(popsStackMouseLeaved()));
	connect(popsStack, SIGNAL(messageActivated(QByteArray&)), this, SIGNAL(messageActivated(QByteArray&)));
	connect(popsStack, SIGNAL(allPopupWindowsRemoved()), this, SLOT(popusRemovedAll()));

	//contextMenu->addAction(QIcon(""), tr("Check e-mail"), NULL, SLOT()); //TODO
	connect(account, SIGNAL(nicknameChanged()), this, SLOT(updateTooltip()));
	connect(account->client(), SIGNAL(newLetter(QString, QString, QDateTime)), this, SLOT(newLetter(QString, QString, QDateTime)));
	connect(account->client(), SIGNAL(newNumberOfUnreadLetters(quint32)), this, SLOT(newLetters(quint32)));

	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(processActivation(QSystemTrayIcon::ActivationReason)));
	
	contextMenu = new QMenu;

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), popsStack, SLOT(closeAllUnclosedWindows()));
	timer->setSingleShot(true);

	iconTimer = new QTimer;
	connect(iconTimer, SIGNAL(timeout()), this, SLOT(iconTimerTimeOver()));
	iconTimer->setSingleShot(true);
	iconTimer->setInterval(500);
	iconDisplayOff = false;

	QWidgetAction* accountWidgetAction = new QWidgetAction(contextMenu);
	QLabel* accountLabel = new QLabel("<b>" + account->email() + "</b>");
	accountLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	accountLabel->setContentsMargins(1, 1, 1, 9);
	accountWidgetAction->setDefaultWidget(accountLabel);
	accountWidgetAction->setDisabled(true);

	contextMenu->addAction(accountWidgetAction);

	contextMenu->addMenu(sm);

	contextMenu->addSeparator();

	mainWindowVisibilityAction =
		contextMenu->addAction(tr("Show contact list"), this, SLOT(toggleMainWindowVisibility()));

	connect(contextMenu, SIGNAL(aboutToShow()), this, SLOT(slotContextMenuAboutToShow()));

	contextMenu->addAction(QIcon(":icons/exit.png"), tr("Quit"), qApp, SLOT(quit()));
	setContextMenu(contextMenu);
}

SystemTrayIcon::~SystemTrayIcon()
{
	delete contextMenu;
}

void SystemTrayIcon::processActivation(QSystemTrayIcon::ActivationReason reason)
{
	if (account->settings()->value("Notification/Inner", true).toBool() && m_popupsExists)
	{
		timer->stop();
		iconTimer->stop();
		popsStack->deleteAllWindows();
	}
	else
		if (mainWindow && reason == QSystemTrayIcon::Trigger)
			toggleMainWindowVisibility();
}

void SystemTrayIcon::updateTooltip()
{
//	setToolTip("");
	QString nickname = account->nickname();
	if (nickname.isEmpty())
#ifdef Q_WS_X11
		setToolTip("<b>" + account->email() + "</b>");
#else
		setToolTip(account->email());
#endif
	else
#ifdef Q_WS_X11
		setToolTip("<b>" + account->nickname() + "</b><br>" + account->email());
#else
		setToolTip(account->nickname() + "\n" + account->email());
#endif
}

bool SystemTrayIcon::event(QEvent* e)
{
	if (e->type() == QEvent::ToolTip && account->settings()->value("Notification/Inner", true).toBool() && m_popupsExists)
	{
		timer->stop();
		popsStack->showAllUnclosedWindows();
		timer->setInterval(2000);
		timer->start();
		return false;
	}
	return QSystemTrayIcon::event(e);
}

void SystemTrayIcon::newLetter(QString sender, QString subject, QDateTime dateTime)
{
	theRM.getAudio()->play(STLetter);

	if (account->settings()->value("Notification/Inner", true).toBool())
	{
		messageIcon = QIcon(":/icons/letter.png");
		setIcon(messageIcon);

		if (!m_popupsExists)
			iconTimer->start();

		popsStack->showNewLetter(sender, subject, dateTime);
		m_popupsExists = true;
	}
	else
	{
		doExProc(account->settings()->value("Notification/LetterReceivedText", "").toString().replace("%me", account->email()).replace("%ME", account->nickname()).replace("%c", sender).replace("%S", subject).replace("%d", dateTime.toString()).replace("%n", "\n"));
	}
}

void SystemTrayIcon::newLetters(quint32 unreadMessages)
{
	if (unreadMessages > 0)
	{
		theRM.getAudio()->play(STLetter);

		if (account->settings()->value("Notification/Inner", true).toBool())
		{
			messageIcon = QIcon(":/icons/letter.png");
			setIcon(messageIcon);

			if (!m_popupsExists)
				iconTimer->start();

			popsStack->showLettersUnread(unreadMessages);
			m_popupsExists = true;
		}
		else
		{
			doExProc(account->settings()->value("Notification/LettersCount", "").toString().replace("%me", account->email()).replace("%ME", account->nickname()).replace("%L", QString::number(unreadMessages)).replace("%n", "\n"));
		}
	}
}

void SystemTrayIcon::newMessage(Contact * from, const QString & to, const QDateTime dateTime)
{
	qDebug() << Q_FUNC_INFO;

	if (account->settings()->value("Notification/Inner", true).toBool())
	{
		messageIcon = QIcon(":/icons/message_16x16.png");
		setIcon(messageIcon);

		if (!m_popupsExists)
			iconTimer->start();

		popsStack->showNewMessage(from, to, dateTime);
		m_popupsExists = true;
	}
	else
	{
		doExProc(account->settings()->value("Notification/MessageReceivedText", "").toString().replace("%me", account->email()).replace("%ME", account->nickname()).replace("%c", from->email()).replace("%C", from->nickname()).replace("%d", dateTime.toString()).replace("%n", "\n"));
	}
}

void SystemTrayIcon::toggleMainWindowVisibility()
{
	if (mainWindow->isMinimized())
	{
		mainWindow->hide();
		mainWindow->showNormal();
	}
	else if (mainWindow->isVisible() && !mainWindow->isActiveWindow())
	{
		mainWindow->extHide();
		mainWindow->extShow();
	}
	else if (mainWindow->isVisible())
	{
		mainWindow->extHide();
	}
	else if (!mainWindow->isVisible())
	{
		mainWindow->extShow();
		mainWindow->raise();
	}
}

void SystemTrayIcon::slotContextMenuAboutToShow()
{
	setupMainWindowVisibilityAction();
}

void SystemTrayIcon::setupMainWindowVisibilityAction()
{
	if (mainWindow->isMinimized())
	{
		mainWindowVisibilityAction->setText(tr("Show contact list"));
	}
	else if (mainWindow->isVisible() && !mainWindow->isActiveWindow())
	{
		mainWindowVisibilityAction->setText(tr("Show contact list"));
	}
	else if (mainWindow->isVisible())
	{
		mainWindowVisibilityAction->setText(tr("Hide contact list"));
	}
	else if (!mainWindow->isVisible())
	{
		mainWindowVisibilityAction->setText(tr("Show contact list"));
	}
}

void SystemTrayIcon::setOnlineStatus(OnlineStatus status)
{
	statusIcon = status.statusIcon();
	if ((account->settings()->value("Notification/Inner", true).toBool() && !m_popupsExists) || !account->settings()->value("Notification/Inner", true).toBool())
		setIcon(statusIcon);
}

SystemTrayIcon::SysTrayPosition SystemTrayIcon::sysTrayPosition()
{
	QPoint desktopTLAtScreen = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft());
	QPoint desktopBRAtScreen = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().bottomRight());
	if (geometry().top() - desktopTLAtScreen.y() < 0)
		return Top;
	else if (geometry().bottom() > desktopBRAtScreen.y())
		return Bottom;
	else if (geometry().left() - desktopTLAtScreen.x() < 0)
		return Left;
	else if (geometry().right() > desktopBRAtScreen.x())
		return Right;
	else
	{
		qDebug() << "Strange geometry...";
		return Top;
	}
}

void SystemTrayIcon::popsStackMouseEntered()
{
	timer->stop();
}

void SystemTrayIcon::popsStackMouseLeaved()
{
	timer->stop();
	timer->setInterval(500);
	timer->start();
}

void SystemTrayIcon::iconTimerTimeOver()
{
	if (m_popupsExists)
	{
		if (iconDisplayOff)
			setIcon(messageIcon);
		else
			setIcon(QIcon());
		iconDisplayOff = !iconDisplayOff;
		iconTimer->start();
	}
	else
		setIcon(statusIcon);
}

void SystemTrayIcon::notificationTypeChange()
{
	if (!account->settings()->value("Notification/Inner", true).toBool())
	{
		timer->stop();
		iconTimer->stop();
		popsStack->deleteAllWindows();
	}
}

void SystemTrayIcon::popusRemovedAll()
{
	m_popupsExists = false;
	iconDisplayOff = false;
	setIcon(statusIcon);
}

void SystemTrayIcon::doExProc(QString prog)
{
	qDebug() << Q_FUNC_INFO << prog;
	//%me = my email
	//%ME = my nick
	//%c = contact email
	//%C = contact nick
	//%L = letters count
	//%S = letter subj
	//%d = date
	//%n = new string
	QProcess::execute(prog);
}
