/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#ifndef POPUPWINDOWSSTACK_H
#define POPUPWINDOWSSTACK_H

#include <QObject>

#include "gui/popupwindow.h"

class SystemTrayIcon;
class Contact;

class PopupWindowsStack : public QObject
{
Q_OBJECT
public:
	enum Direction
	{
		Up,
		Down
	};

	PopupWindowsStack(SystemTrayIcon* sysTray);
	~PopupWindowsStack();

public Q_SLOTS:
	void showNewMessage(Contact * from, const QString & to, const QDateTime& dateTime);
	void showNewLetter(const QString & from, const QString & subject, const QDateTime& dateTime);
	void showLettersUnread(const quint32 cnt);

	void deleteAllWindows();
	void showAllUnclosedWindows();
	void closeAllUnclosedWindows();

Q_SIGNALS:
	void mouseEntered();
	void mouseLeaved();
	void popupWindowActivated(PopupWindow::Type type);
	void messageActivated(const QByteArray& email);
	void allPopupWindowsRemoved();

private Q_SLOTS:
	void retranslateWindowsGeometry();
	void windowClosed();
	void slotPopupWindowActivated();

	PopupWindow* newWindow();
	QRect newCoords(quint32 n = 0);

private:
	SystemTrayIcon* m_sysTray;
	QList<PopupWindow*> existingWindows;
	QList<PopupWindow*> shownWindows;
	Direction direction;
};

#endif // POPUPWINDOWSSTACK_H
