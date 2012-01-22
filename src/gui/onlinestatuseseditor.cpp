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

#include "onlinestatuseseditor.h"

#include <QDebug>
#include <QSettings>
#include <QPushButton>

#include "account.h"
#include "resourcemanager.h"
#include "onlinestatusselector.h"

//-------StatusRow------------//
StatusRow::StatusRow(OnlineStatusesEditor* parent, qint32 id)
	: m_parent(parent), m_id(id)
{
	m_changed = false;

	m_statusId = OnlineStatus(parent->m_account->settings()->value("Statuses/statusid" + QByteArray::number(id), OnlineStatus::getDefIdStatus(id)).toByteArray(), "").id();
	m_checkBox = new QCheckBox;
	m_buttonIcon = new QPushButton;
	m_selector = new OnlineStatusSelector(parent);
	m_checkBox->setChecked(parent->m_account->settings()->value("Statuses/statuschecked" + QByteArray::number(id), true).toBool());
	m_buttonIcon->setIcon(OnlineStatus(parent->m_account->settings()->value("Statuses/statusid" + QByteArray::number(id), OnlineStatus::getDefIdStatus(id)).toByteArray(), "").statusIcon());
	m_statusDescr = new QLineEdit(parent->m_account->settings()->value("Statuses/statusdescr" + QByteArray::number(id), OnlineStatus::getDefDescrStatus(id)).toString());

	connect(m_buttonIcon, SIGNAL(clicked(bool)), this, SLOT(showStatusSelector()));
	connect(m_selector, SIGNAL(selected(QString)), this, SLOT(statusSelected(QString)));
	connect(m_statusDescr, SIGNAL(textEdited(const QString&)), this, SLOT(statusChanged()));
	connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(statusChanged()));

	addWidget(m_checkBox);
	addWidget(m_buttonIcon);
	addWidget(m_statusDescr);
}

StatusRow::~StatusRow()
{
	qDebug() << "StatusRow::~StatusRow()";
}

void StatusRow::showStatusSelector()
{
	qDebug() << "OnlineStatusesEditor::showStatusSelector()";

	QRect rect(m_parent->mapToGlobal(m_buttonIcon->pos()).x(),
		   m_parent->mapToGlobal(m_buttonIcon->pos()).y() + m_buttonIcon->geometry().height(),
		   0, 0);

	m_selector->setGeometry(rect);
	m_selector->show();
}

void StatusRow::statusSelected(QString id)
{
	qDebug() << "StatusRow::statusSelected" << id;

	m_buttonIcon->setIcon(OnlineStatus(id.toLatin1(), "").statusIcon());
	m_statusId = id.toLatin1();

	m_changed = true;
}

void StatusRow::statusChanged()
{
	qDebug() << "StatusRow::statusChanged()";
	m_changed = true;
}

//-------OnlineStatusesEditor------------//
OnlineStatusesEditor::OnlineStatusesEditor(Account* account)
	: m_account(account)
{
	qDebug() << "OnlineStatusesEditor::OnlineStatusesEditor";
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Statuses editor"));

	QVBoxLayout* layout = new QVBoxLayout;

	int i;
	for (i = 0; i < m_account->settings()->value("Statuses/count", theRM.minDefaultStatuses).toInt(); i++)
	{
		StatusRow* statusRow = new StatusRow(this, i);
		statusRows.append(statusRow);

		layout->addLayout(statusRow);
	}
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	QPushButton* okButton = new QPushButton(tr("Save"));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"));

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(saveStatuses()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	layout->addLayout(buttonsLayout);
	setLayout(layout);
}

OnlineStatusesEditor::~OnlineStatusesEditor()
{
	qDeleteAll(statusRows);
}

void OnlineStatusesEditor::saveStatuses()
{
	if (m_account->settings()->value("Statuses/count", theRM.minDefaultStatuses).toInt() == theRM.minDefaultStatuses)
		m_account->settings()->remove("Statuses/count");
	m_account->settings()->setValue("Statuses/count", statusRows.size());

	for (int i = 0; i < statusRows.size(); i++)
	{
		StatusRow* statusRow = statusRows[i];

		if (!statusRow->changed())
			continue;

		m_account->settings()->setValue("Statuses/statuschecked" + QByteArray::number(statusRow->id()), statusRow->checked());
		m_account->settings()->setValue("Statuses/statusid" + QByteArray::number(statusRow->id()), statusRow->statusId());
		m_account->settings()->setValue("Statuses/statusdescr" + QByteArray::number(statusRow->id()), statusRow->statusDescr());

		Q_EMIT statusChanged(statusRow->id(), OnlineStatus(statusRow->statusId(), statusRow->statusDescr()));
	}
	Q_EMIT statusesChanged();

	qDebug() << "ok";
	close();
}
