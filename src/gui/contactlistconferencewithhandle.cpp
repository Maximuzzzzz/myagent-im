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

#include "contactlistconferencewithhandle.h"

#include <QDebug>
#include <QBoxLayout>
#include <QToolButton>
#include <QLabel>

#include "contactlistconference.h"

ContactListConferenceWithHandle::ContactListConferenceWithHandle(Contact* conference, Account* acc, QWidget* parent)
 : QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);

	QVBoxLayout* listLayout = new QVBoxLayout;
	members = new QLabel(tr("Members: 0"));
	contactListConference = new ContactListConference(conference, acc);
	connect(contactListConference, SIGNAL(setMembersCount(quint32)), this, SLOT(setMembersCount(quint32)));

	QHBoxLayout* addMembersLayout = new QHBoxLayout;
	iconLabel = new QLabel;
	iconLabel->setPixmap(QPixmap(":icons/msg_conference.png"));
	iconLabel->setFixedSize(iconLabel->sizeHint());

	addMembersLabel = new QLabel(tr("Add members"));

	addMembersLayout->addWidget(iconLabel);
	addMembersLayout->addWidget(addMembersLabel);

	listLayout->addWidget(members);
	listLayout->addWidget(contactListConference);
	listLayout->addLayout(addMembersLayout);

	QVBoxLayout* handleLayout = new QVBoxLayout;

	QIcon handleIcon;
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_left.png"), QIcon::Normal, QIcon::Off);
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_right.png"), QIcon::Normal, QIcon::On);

	handleButton = new QToolButton;
	handleButton->setIcon(handleIcon);
	handleButton->setCheckable(true);
	connect(handleButton, SIGNAL(clicked(bool)), this, SLOT(toggleConferenceList(bool)));
	contactListConference->setVisible(false);
	handleButton->setAutoRaise(true);
	int d = handleButton->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	handleButton->setFixedSize((handleButton->icon().actualSize(QSize(16, 16)) + QSize(2*d, 2*d)));
	qDebug() << "handleButton" << handleButton->width();

	handleLayout->addWidget(handleButton);
	handleLayout->addStretch();

	layout->addLayout(listLayout);
	layout->addLayout(handleLayout);

	setLayout(layout);
}

void ContactListConferenceWithHandle::toggleConferenceList(bool checked)
{
	members->setVisible(checked);
	contactListConference->setVisible(checked);
	iconLabel->setVisible(checked);
	addMembersLabel->setVisible(checked);
	Q_EMIT toggled(checked);
}

void ContactListConferenceWithHandle::toggle(bool visible)
{
	members->setVisible(visible);
	contactListConference->setVisible(visible);
	iconLabel->setVisible(visible);
	addMembersLabel->setVisible(visible);
	handleButton->setChecked(visible);
}

void ContactListConferenceWithHandle::setMembersCount(quint32 cnt)
{
	members->setText(tr("Members: %1").arg(QString::number(cnt)));
}
