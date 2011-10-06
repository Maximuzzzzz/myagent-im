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

#include "popupwindowsstack.h"

#include "gui/systemtrayicon.h"
#include "resourcemanager.h"

PopupWindowsStack::PopupWindowsStack(SystemTrayIcon* sysTray)
 : QObject(sysTray), m_sysTray(sysTray)
{
}

PopupWindowsStack::~PopupWindowsStack()
{
	deleteAllWindows();
}

void PopupWindowsStack::showNewMessage(Contact * from, const QString & to, const QDateTime dateTime)
{
	qDebug() << Q_FUNC_INFO;
	PopupWindow* p = newWindow();
	p->setMessageReceived(from->email(), from->nickname(), to, dateTime);
	connect(p, SIGNAL(activated()), this, SLOT(slotPopupWindowActivated()));
	p->show();
}

void PopupWindowsStack::showNewLetter(const QString & from, const QString & subject, const QDateTime dateTime)
{
	qDebug() << Q_FUNC_INFO;
	PopupWindow* p = newWindow();
	p->setLetterReceived(from, subject, dateTime);
	connect(p, SIGNAL(activated()), this, SLOT(slotPopupWindowActivated()));
	p->show();
}

void PopupWindowsStack::showLettersUnread(const quint32 cnt)
{
	qDebug() << Q_FUNC_INFO;
	PopupWindow* p = newWindow();
	p->setUnreadLettersText(cnt);
	connect(p, SIGNAL(activated()), this, SLOT(slotPopupWindowActivated()));
	p->show();
}

void PopupWindowsStack::retranslateWindowsGeometry()
{
	qDebug() << Q_FUNC_INFO << "{";
	QList<PopupWindow*>::iterator it = shownWindows.begin();
	quint32 i = 1;
	for (; it != shownWindows.end(); ++it)
	{
		QRect g = newCoords(i);
		(*it)->setGeometry(g);
		++i;
	}
	qDebug() << Q_FUNC_INFO << "}";
}

QRect PopupWindowsStack::newCoords(quint32 n)
{
	qDebug() << Q_FUNC_INFO << "{";
	int x = 0;
	int y = 0;
	switch (m_sysTray->sysTrayPosition())
	{
		case SystemTrayIcon::Right:
			x = m_sysTray->geometry().left() - theRM.popupWindowWidth;
			y = m_sysTray->geometry().top();
			if (y + theRM.popupWindowHeight > QApplication::desktop()->availableGeometry().height()) //if popup window will be covered by bottom border of available screen
				y = QApplication::desktop()->availableGeometry().height() - theRM.popupWindowHeight;
			if (y - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y() < 0) //if popup window will be covered by top border of available screen
				y = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y();
			if (shownWindows.count() == 0)
			{
				if (y - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y() < QApplication::desktop()->availableGeometry().height() / 2) //if sys tray icon closer to top of the screen
					direction = Down;
				else
					direction = Up;
			}
			if (direction == Up)
				y -= (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
			else
				y += (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
			break;
		case SystemTrayIcon::Bottom:
			x = m_sysTray->geometry().left();
			if (x + theRM.popupWindowWidth > QApplication::desktop()->availableGeometry().width()) //if popup window will be covered by right border of available screen
				x = QApplication::desktop()->availableGeometry().width() - theRM.popupWindowWidth;
			if (x - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).x() < 0) //if popup window will be covered by left border of available screen
				x = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).x();
			y = m_sysTray->geometry().top() - theRM.popupWindowHeight;
			y -= (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
			break;
		case SystemTrayIcon::Left:
			x = m_sysTray->geometry().right();
			y = m_sysTray->geometry().top();
			if (y + theRM.popupWindowHeight > QApplication::desktop()->availableGeometry().height()) //if popup window will be covered by bottom border of available screen
				y = QApplication::desktop()->availableGeometry().height() - theRM.popupWindowHeight;
			if (y - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y() < 0) //if popup window will be covered by top border of available screen
				y = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y();
			if (shownWindows.count() == 0)
			{
				if (y - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).y() < QApplication::desktop()->availableGeometry().height() / 2) //if sys tray icon closer to top of the screen
					direction = Down;
				else
					direction = Up;
			}
			if (direction == Up)
				y -= (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
			else
				y += (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
			break;
		case SystemTrayIcon::Top:
		default:
			x = m_sysTray->geometry().left();
			if (x + theRM.popupWindowWidth > QApplication::desktop()->availableGeometry().width()) //if popup window will be covered by right border of available screen
				x = QApplication::desktop()->availableGeometry().width() - theRM.popupWindowWidth;
			if (x - QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).x() < 0) //if popup window will be covered by left border of available screen
				x = QApplication::desktop()->mapToGlobal(QApplication::desktop()->availableGeometry().topLeft()).x();
			y = m_sysTray->geometry().bottom() + (n == 0 ? shownWindows.count() : n - 1) * theRM.popupWindowHeight;
	}

	qDebug() << Q_FUNC_INFO << "}";
	return QRect(x, y, theRM.popupWindowWidth, theRM.popupWindowHeight);
}

PopupWindow* PopupWindowsStack::newWindow()
{
	qDebug() << Q_FUNC_INFO << "{";
	PopupWindow* p = new PopupWindow(newCoords());
	connect(p, SIGNAL(closePopupWindow()), this, SLOT(windowClosed()));
	connect(p, SIGNAL(mouseEntered()), this, SIGNAL(mouseEntered()));
	connect(p, SIGNAL(mouseLeaved()), this, SIGNAL(mouseLeaved()));
	existingWindows.append(p);
	shownWindows.append(p);	
	qDebug() << Q_FUNC_INFO << "}";
	return p;
}

void PopupWindowsStack::windowClosed()
{
	qDebug() << Q_FUNC_INFO;
	PopupWindow* p = qobject_cast<PopupWindow*>(sender());
	shownWindows.removeAll(p);

	retranslateWindowsGeometry();
}

void PopupWindowsStack::deleteAllWindows()
{
	qDebug() << Q_FUNC_INFO;
	qDeleteAll(existingWindows);
	existingWindows.clear();
	shownWindows.clear();
	Q_EMIT allPopupWindowsRemoved();
}

void PopupWindowsStack::showAllUnclosedWindows()
{
	qDebug() << Q_FUNC_INFO << "{";
	QList<PopupWindow*>::iterator it = existingWindows.begin();
	quint32 i = 1;
	for (; it != existingWindows.end(); ++it)
	{
		(*it)->setNotToClose(true);
		(*it)->setGeometry(newCoords(i));
		(*it)->show();
		++i;
	}
	qDebug() << Q_FUNC_INFO << "}";
}

void PopupWindowsStack::closeAllUnclosedWindows()
{
	qDebug() << Q_FUNC_INFO;
	if (existingWindows.count() == 0)
		return;
	QList<PopupWindow*>::iterator it = existingWindows.begin();
	quint32 i = 1;
	for (; it != existingWindows.end(); ++it)
	{
		(*it)->setNotToClose(false);
		if ((*it)->isClosed())
		{
			(*it)->close();
			shownWindows.removeAll(*it);
		}
		++i;
	}
	retranslateWindowsGeometry();
}

void PopupWindowsStack::slotPopupWindowActivated()
{
	qDebug() << Q_FUNC_INFO;
	PopupWindow * p = qobject_cast<PopupWindow*>(sender());
	if (p->type() == PopupWindow::NewMessage)
	{
/*		existingWindows.removeAll(p);
		shownWindows.removeAll(p);*/
		Q_EMIT messageActivated(p->from());
/*		p->deleteLater();
		retranslateWindowsGeometry();*/
	} //TODO: ChatWindow::activated NEEDS!

	deleteAllWindows();
}
