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

#include "task.h"

#include <QTimer>
#include <QDebug>

#include "mrimclient.h"

Task::Task(MRIMClient* client, QObject *parent)
 : QObject(parent), mc(client)
{
	connect(mc, SIGNAL(disconnectedFromServer()), this, SLOT(deleteLater()));
	timer = new QTimer(this);
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void Task::setTimer()
{
//timer->start(1000);

	if (mc != 0)
		timer->start(mc->getPingTime());
}

void Task::setTimer(int msec)
{
	timer->start(msec);
}

void Task::unsetTimer()
{
	timer->stop();
}

void Task::timeout()
{
	qDebug() << "Task::timeout()";
	onTimeout();
	
	emit done(0, true);
	deleteLater();
}
