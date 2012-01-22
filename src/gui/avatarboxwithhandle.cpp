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

#include "avatarboxwithhandle.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QStyle>

#include "avatarbox.h"

AvatarBoxWithHandle::AvatarBoxWithHandle(const QString & avatarsPath, const QString & email, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);

	QVBoxLayout* avatarLayout = new QVBoxLayout;
	avatarBox = new AvatarBox(avatarsPath, email);
	
	avatarLayout->addWidget(avatarBox);
	avatarLayout->addStretch();

	QVBoxLayout* handleLayout = new QVBoxLayout;
	
	QIcon handleIcon;
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_left.png"), QIcon::Normal, QIcon::Off);
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_right.png"), QIcon::Normal, QIcon::On);

	handleButton = new QToolButton;
	handleButton->setIcon(handleIcon);
	handleButton->setCheckable(true);
	connect(handleButton, SIGNAL(clicked(bool)), this, SLOT(toggleAvatarBox(bool)));
	avatarBox->setVisible(false);
	handleButton->setAutoRaise(true);
	int d = handleButton->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	handleButton->setFixedSize((handleButton->icon().actualSize(QSize(16, 16)) + QSize(2*d, 2*d)));

	handleLayout->addWidget(handleButton);
	handleLayout->addStretch();

	layout->addLayout(avatarLayout);
	layout->addLayout(handleLayout);

	setLayout(layout);
}

void AvatarBoxWithHandle::toggleAvatarBox(bool checked)
{
	qDebug() << "AvatarBoxWithHandle::toggleAvatarBox checked = " << checked;
	avatarBox->setVisible(checked);
	Q_EMIT toggled(checked);
}

void AvatarBoxWithHandle::toggle(bool visible)
{
	avatarBox->setVisible(visible);
	handleButton->setChecked(visible);
}
