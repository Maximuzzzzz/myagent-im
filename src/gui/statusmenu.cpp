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

#include "statusmenu.h"

#include <QDebug>
#include <QSettings>

#include "resourcemanager.h"
#include "core/account.h"
#include "centerwindow.h"
#include "onlinestatuseseditor.h"

StatusMenu::StatusMenu(Account* acc, QWidget* parent)
	: QMenu(parent), m_account(acc)
{
	QAction* editStatuses = new QAction(tr("Edit statuses..."), this);
	addAction(editStatuses);

	mainSeparator = addSeparator();

	extendedActions = new QActionGroup(this);

	updateExtendedStatuses();

	QActionGroup* statusActions = new QActionGroup(this);

	QList<QByteArray> statuses;
	statuses << "status_1" << "status_dating" << "status_chat" << "status_2" << "status_3" << "status_dnd";
	QList<QByteArray>::const_iterator it;
	for (it = statuses.begin(); it != statuses.end(); ++it)
	{
		OnlineStatus onlineStatus(*it);
		if (onlineStatus.available())
			statusActions->addAction(createStatusAction(onlineStatus));
	}
	OnlineStatus onlineStatus("status_0");
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
	QAction* action = new QAction(status.statusIcon(), status.description(), this);
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

void StatusMenu::showOnlineStatusesEditor()
{
	if (m_onlineStatusesEditor)
		return;
	qDebug() << "Statuses editor isn't exists";
	m_onlineStatusesEditor = new OnlineStatusesEditor(m_account);
	centerWindow(m_onlineStatusesEditor);

	connect(m_onlineStatusesEditor, SIGNAL(statusesChanged()), this, SIGNAL(extendedStatusesChanged()));
	connect(m_onlineStatusesEditor, SIGNAL(statusChanged(qint32, OnlineStatus)), this, SLOT(extendedStatusChanged(qint32, OnlineStatus)));

	m_onlineStatusesEditor->show();
}

void StatusMenu::updateExtendedStatuses()
{
	qDebug() << "StatusMenu::updateExtendedStatuses()";
	qDeleteAll(extendedActions->actions());

	for (int i = 0; i < m_account->settings()->value("Statuses/count", theRM.minDefaultStatuses).toInt(); i++)
	{
		if (!m_account->settings()->value("Statuses/statuschecked" + QByteArray::number(i), true).toBool())
			continue;
		extendedActions->addAction(createStatusAction(OnlineStatus(m_account->settings()->value("Statuses/statusid" + QByteArray::number(i), OnlineStatus::getDefIdStatus(i)).toByteArray(), m_account->settings()->value("Statuses/statusdescr" + QByteArray::number(i), OnlineStatus::getDefDescrStatus(i)).toString())));
	}
	insertActions(mainSeparator, extendedActions->actions());
}

