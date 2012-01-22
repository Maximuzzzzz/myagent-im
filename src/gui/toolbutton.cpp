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

#include "toolbutton.h"

ToolButton::ToolButton(QAction* action, QWidget* parent)
	: QToolButton(parent)
{
	setDefaultAction(action);
	init();
}

ToolButton::ToolButton(const QIcon& icon, QWidget* parent)
	: QToolButton(parent)
{
	setIcon(icon);
	init();
}

void ToolButton::init()
{
	setStyleSheet("QToolButton { border: 0px; padding: 0px }");
	setCheckable(true);
	setAutoRaise(true);
}

void ToolButton::mousePressEvent(QMouseEvent* event)
{
	QToolButton::mousePressEvent(event);
	setChecked(true);
}

void ToolButton::mouseReleaseEvent(QMouseEvent * event)
{
	QToolButton::mouseReleaseEvent(event);
	setChecked(false);
}
