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

#include "centerwindow.h"

#include <QDebug>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

void centerWindow(QWidget* window)
{
	window->adjustSize();
	
	QDesktopWidget* desktop = QApplication::desktop();

	QRect screenRect = desktop->availableGeometry(window);

	qDebug() << "availableGeometry = " << screenRect;

	int windowWidth = window->geometry().width();
	int windowHeight = window->geometry().height();

	qDebug() << "windowWidth = " << windowWidth << ", windowHeight = " << windowHeight;
	qDebug() << "width = " << window->width() << ". height = " << window->height();
	qDebug() << "sizeHint = " << window->sizeHint();

	int x = (screenRect.width() - windowWidth) / 2;
	int y = (screenRect.height() - windowHeight) / 2;

	window->setGeometry(x, y, windowWidth, windowHeight);
}
