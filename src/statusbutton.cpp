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

#include <QMenu>
#include <QAction>
#include <QDebug>

#include "statusbutton.h"
#include "resourcemanager.h"
#include "proto.h"

StatusButton::StatusButton(QWidget* parent)
	: ButtonWithMenu(parent)
{
	setStyleSheet("QPushButton { text-align: left; }");
	
	QMenu* menu = new QMenu(this);
	
	menu->addAction(createAction(OnlineStatus::online));
	menu->addAction(createAction(OnlineStatus::away));
	menu->addAction(createAction(OnlineStatus::invisible));
	menu->addAction(createAction(OnlineStatus::offline));
	
	setMenu(menu);
	connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(slotStatusChanged(QAction*)));
	
	setIcon(status.contactListIcon());
	setText(status.description());
}

StatusButton::~StatusButton()
{
}

void StatusButton::setStatus(OnlineStatus status)
{
	if (status == this->status)
		return;
	
	qDebug() << "StatusButton::setStatus " << status.type();
	
	this->status = status;
	
	setIcon(status.contactListIcon());
	setText(status.description());
	qDebug() << "StatusButton status desc " << status.description();
	
	emit statusChanged(status);
}

void StatusButton::slotStatusChanged(QAction* action)
{
	OnlineStatus newStatus = action->data().value<OnlineStatus>();
	setStatus(newStatus);
}

QAction* StatusButton::createAction(OnlineStatus status)
{
	QAction* action = new QAction(status.contactListIcon(), status.description(), this);
	action->setData(QVariant::fromValue(status));
	return action;
}

