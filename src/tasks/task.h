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

#ifndef TASK_H
#define TASK_H

#include <QObject>

class MRIMClient;
class QTimer;

class Task : public QObject
{
Q_OBJECT
public:
    Task(MRIMClient* client, QObject* parent = 0);
	virtual ~Task() {}
	virtual bool exec() = 0;
	void setTimer();
	void setTimer(int msec);
	void unsetTimer();

Q_SIGNALS:
	void done(quint32 status, bool timeout);
	
protected Q_SLOTS:
	virtual void timeout();
protected:
	virtual void onTimeout() {}
protected:
	MRIMClient* mc;
	QTimer* timer;
	quint32 seq;
};

#endif
