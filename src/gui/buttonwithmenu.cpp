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

#include "buttonwithmenu.h"

#include <QMenu>

ButtonWithMenu::ButtonWithMenu(QWidget* parent)
	: QPushButton(parent)
{
	connect(this, SIGNAL(pressed()), this, SLOT(execMenu()));
}

ButtonWithMenu::~ButtonWithMenu()
{
}

void ButtonWithMenu::setMenu(QMenu* menu)
{
	this->menu = menu;
}

void ButtonWithMenu::execMenu()
{
	menu->exec(mapToGlobal(QPoint(0, -menu->sizeHint().height())));
	setDown(false);
}
