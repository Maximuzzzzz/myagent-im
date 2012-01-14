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

#include "onlinestatusselector.h"

#include <QGridLayout>
#include <QPushButton>
#include <QSignalMapper>

#include "resourcemanager.h"

OnlineStatusSelector::OnlineStatusSelector(QWidget *parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_AlwaysShowToolTips);

	setFrameStyle(QFrame::Panel | QFrame::Raised);

	setCurrentLayout();

	setFixedSize(sizeHint());
}

void OnlineStatusSelector::correctSize()
{
	setFixedSize(sizeHint());
}

QWidget* OnlineStatusSelector::createStatusIconsWidget(int emoticonsPerRow)
{
	QWidget* setWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(15);
	QGridLayout* setLayout = new QGridLayout;
	setLayout->setSpacing(2);

	int row = 0;
	int col = 0;

	QSignalMapper* signalMapper = new QSignalMapper(this);

	Q_FOREACH (QString statusId, theRM.onlineStatuses()->statusesList())
	{
		const OnlineStatusInfo* info = theRM.onlineStatuses()->getOnlineStatusInfo(statusId);

		if (!info)
			continue;

		QString filename = theRM.statusesResourcePrefix().append(":").append(info->icon());
		QPixmap pm(filename);

		QPushButton* pb = new QPushButton;
		pb->setStyleSheet("QPushButton { border: 0px; }");
		pb->setIcon(pm);
		pb->setIconSize(pm.size());
		pb->setFixedSize(pm.size() + QSize(2, 2));

		connect(pb, SIGNAL(clicked()), signalMapper, SLOT(map()));
		signalMapper->setMapping(pb, statusId);

		setLayout->addWidget(pb, row, col);
		if (col == emoticonsPerRow - 1)
		{
			col = 0;
			row++;
		}
		else
			col++;
	}

	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(slotClicked(QString)));

	layout->addLayout(setLayout);

	setWidget->setLayout(layout);

	return setWidget;
}

OnlineStatusSelector::~OnlineStatusSelector()
{
}

void OnlineStatusSelector::slotClicked(QString id)
{
	Q_EMIT selected(id);
	close();
}

void OnlineStatusSelector::setCurrentLayout()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	//layout->setSpacing(2);

	int emoticonsPerRow = 8;

	statuses = createStatusIconsWidget(emoticonsPerRow);
	layout->addWidget(statuses);
	setLayout(layout);
}
