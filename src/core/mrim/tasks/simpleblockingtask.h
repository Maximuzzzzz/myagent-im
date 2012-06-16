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

#ifndef TASKSSIMPLEBLOCKINGTASK_H
#define TASKSSIMPLEBLOCKINGTASK_H

#include <QDebug>

#include "task.h"

namespace Tasks
{

template <class ConcreteTask>
class SimpleBlockingTask : public Task
{
public:
	SimpleBlockingTask(MRIMClient* client, QObject* parent = 0)
		: Task(client, parent) {}
	~SimpleBlockingTask();

protected:
	bool block();
	void unblock();
	bool checkCall(quint32 msgseq);
	bool isMyResponse(quint32 msgseq);
	virtual void onTimeout();
	
private:
	static bool blocked;
};

template <class ConcreteTask>
bool SimpleBlockingTask<ConcreteTask>::blocked = false;

}

template <class ConcreteTask>
Tasks::SimpleBlockingTask<ConcreteTask>::~SimpleBlockingTask()
{
	unblock();
}

template <class ConcreteTask>
bool Tasks::SimpleBlockingTask<ConcreteTask>::block()
{
	if (mc == NULL)
	{
		deleteLater();
		return false;
	}
	
	if (blocked)
	{
		qDebug () << "task execution blocked, deleting";
		deleteLater();
		return false;
	}

	blocked = true;
	return true;
}

template <class ConcreteTask>
bool Tasks::SimpleBlockingTask<ConcreteTask>::checkCall(quint32 msgseq)
{
	if (msgseq == 0)
	{
		deleteLater();
		return false;
	}

	seq = msgseq;
	setTimer();

	return true;
}

template <class ConcreteTask>
void Tasks::SimpleBlockingTask<ConcreteTask>::onTimeout()
{
	unblock();
}

template <class ConcreteTask>
void Tasks::SimpleBlockingTask<ConcreteTask>::unblock()
{
	blocked = false;
}

template <class ConcreteTask>
bool Tasks::SimpleBlockingTask<ConcreteTask>::isMyResponse(quint32 msgseq)
{
	if (seq == msgseq)
	{
		unsetTimer();
		unblock();
		return true;
	}
	
	return false;
}

#endif
