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

#include "avatarwidget.h"

#include <QPainter>

AvatarWidget::AvatarWidget(QWidget* parent)
	: QWidget(parent)
{
	connect(&avatar, SIGNAL(updated()), this, SLOT(slotAvatarUpdated()));
	setFixedSize(avatar.size());
	baseWidth = avatar.width();
	baseHeight = avatar.height();
}

AvatarWidget::~AvatarWidget()
{
}

void AvatarWidget::load(const QString& dirname, const QString& email)
{
	avatar.load(dirname, email);
}

void AvatarWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	int dx, dy;
	
	dx = (baseWidth - avatar.width()) / 2;
	dx = dx < 0 ? 0 : dx;
	
	dy = (baseHeight - avatar.height()) / 2;
	dy = dy < 0 ? 0 : dy;
	
	painter.drawPixmap(dx, dy, avatar);
}

void AvatarWidget::slotAvatarUpdated()
{
	update();
}
