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

#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QWidget>

#include "avatar.h"

class AvatarWidget : public QWidget
{
Q_OBJECT
public:
	AvatarWidget(QWidget *parent = 0);
	~AvatarWidget();
	
	void load(const QString& dirname, const QString& email);
	bool isNull() const { return avatar.state() == Avatar::Null; }
protected:
	virtual void paintEvent(QPaintEvent* event);
	
private Q_SLOTS:
	void slotAvatarUpdated();
private:
	Avatar avatar;
	int baseWidth;
	int baseHeight;
};

#endif
