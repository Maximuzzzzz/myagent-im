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

#ifndef ONLINESTATUSESSELECTOR_H
#define ONLINESTATUSESSELECTOR_H

#include <QFrame>

class QToolBox;
class EmoticonMovie;

class OnlineStatusSelector : public QFrame
{
Q_OBJECT
public:
	OnlineStatusSelector(QWidget *parent = 0);
	~OnlineStatusSelector();

Q_SIGNALS:
	void selected(QString id);

private Q_SLOTS:
	void slotClicked(QString id);
	void correctSize();
	void setCurrentLayout();

private:
	QWidget* createStatusIconsWidget(int emoticonsPerRow);

private:
	QWidget* statuses;
};

#endif
