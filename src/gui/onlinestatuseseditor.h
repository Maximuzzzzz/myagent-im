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

#ifndef ONLINESTATUSESEDITOR_H
#define ONLINESTATUSESEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include <QBoxLayout>
#include <QCheckBox>

#include "core/onlinestatus.h"

class QPushButton;

class Account;
class OnlineStatusSelector;
class OnlineStatusesEditor;

class StatusRow : public QHBoxLayout
{
Q_OBJECT
public:
	StatusRow(OnlineStatusesEditor* parent, qint32 id);
	~StatusRow();

	qint32 id() { return m_id; }
	bool checked() { return m_checkBox->isChecked(); }
	QByteArray statusId() { return m_statusId; }
	QString statusDescr() { return m_statusDescr->text(); }
	bool changed() { return m_changed; }

private Q_SLOTS:
	void showStatusSelector();
	void statusSelected(QString id);
	void statusChanged();

private:
	OnlineStatusesEditor* m_parent;
	OnlineStatusSelector* m_selector;
	QCheckBox* m_checkBox;
	QPushButton* m_buttonIcon;
	QLineEdit* m_statusDescr;
	QByteArray m_statusId;
	qint32 m_id;
	bool m_changed;
};

class OnlineStatusesEditor : public QWidget
{
Q_OBJECT
	friend class StatusRow;
public:
	OnlineStatusesEditor(Account* account);
	~OnlineStatusesEditor();

Q_SIGNALS:
	void statusChanged(qint32 i, OnlineStatus);
	void statusesChanged();

private Q_SLOTS:
	void saveStatuses();

private:
	Account* m_account;
	QList<StatusRow*> statusRows;
};

#endif
