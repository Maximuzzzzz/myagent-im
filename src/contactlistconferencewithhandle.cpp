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

#include "contactlistconferencewithhandle.h"

#include <QBoxLayout>
#include <QToolButton>
#include <QDebug>

ContactListConferenceWithHandle::ContactListConferenceWithHandle(ContactList* cl, QWidget* parent)
 : QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);

	QVBoxLayout* listLayout = new QVBoxLayout;
	contactListConference = new ContactListConference;

	listLayout->addWidget(contactListConference);

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
	contactListConference->setVisible(checked);
	emit toggled(checked);
}

void ContactListConferenceWithHandle::toggle(bool visible)
{
	contactListConference->setVisible(visible);
	handleButton->setChecked(visible);
}
