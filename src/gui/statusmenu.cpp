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

#include <QDebug>

#include "statusmenu.h"

#include "resourcemanager.h"

StatusMenu::StatusMenu(Account* acc, QWidget* parent)
	: QMenu(parent), m_account(acc)
{
	QAction* editStatuses = new QAction(tr("Edit statuses..."), this);
	addAction(editStatuses);

	mainSeparator = addSeparator();

	extendedActions = new QActionGroup(this);

	updateExtendedStatuses();

	QActionGroup* statusActions = new QActionGroup(this);

	OnlineStatus onlineStatus;
	QList<QByteArray> statuses;
	statuses << "status_1" << "status_dating" << "status_chat" << "status_2" << "status_3" << "status_dnd";
	QList<QByteArray>::const_iterator it;
	for (it = statuses.begin(); it != statuses.end(); ++it)
	{
		onlineStatus.setIdStatus(*it);
		if (theRM.onlineStatuses()->getOnlineStatusInfo(onlineStatus.id())->available() == "1")
			statusActions->addAction(createStatusAction(onlineStatus));
	}
	onlineStatus.setIdStatus("status_0");
	statusActions->addAction(createStatusAction(onlineStatus));

	addActions(statusActions->actions());

	connect(statusActions, SIGNAL(triggered(QAction*)), this, SLOT(slotActionTriggered(QAction*)));
	connect(extendedActions, SIGNAL(triggered(QAction*)), this, SLOT(slotActionTriggered(QAction*)));
	connect(editStatuses, SIGNAL(triggered(bool)), acc, SLOT(showOnlineStatusesEditor()));
}

StatusMenu::~StatusMenu()
{
}

QAction* StatusMenu::createStatusAction(OnlineStatus status)
{
	QAction* action = new QAction(status.statusIcon(), status.statusDescr(), this);
	action->setData(QVariant::fromValue(status));
	return action;
}

void StatusMenu::slotActionTriggered(QAction* action)
{
	OnlineStatus newStatus = action->data().value<OnlineStatus>();
	if (m_account->onlineStatus() == newStatus)
		return;

	qint32 i = extendedActions->actions().indexOf(action);

	Q_EMIT statusChanged(newStatus, i);
}

void StatusMenu::updateExtendedStatuses()
{
	qDebug() << "StatusMenu::updateExtendedStatuses()";
	qDeleteAll(extendedActions->actions());

	OnlineStatus tmp;
	int i;
	for (i = 0; i < m_account->settings()->value("Statuses/count", theRM.minDefaultStatuses).toInt(); i++)
	{
		if (!m_account->settings()->value("Statuses/statuschecked" + QByteArray::number(i), true).toBool())
			continue;
		extendedActions->addAction(createStatusAction(OnlineStatus(m_account->settings()->value("Statuses/statusid" + QByteArray::number(i), tmp.getDefIdStatus(i)).toByteArray(), m_account->settings()->value("Statuses/statusdescr" + QByteArray::number(i), tmp.getDefDescrStatus(i)).toString())));
	}
	insertActions(mainSeparator, extendedActions->actions());
}

