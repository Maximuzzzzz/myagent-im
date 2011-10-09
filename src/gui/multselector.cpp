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

#include "multselector.h"

#include <QDebug>
#include <QBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QSignalMapper>

#include "resourcemanager.h"

MultSelector::MultSelector(QWidget* parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_AlwaysShowToolTips);

	setFrameStyle(QFrame::Panel | QFrame::Raised);

	setCurrentLayout();

	setFixedSize(sizeHint());
}

void MultSelector::closeEvent(QCloseEvent* event)
{
	Q_EMIT closed();
}

MultSelector::~MultSelector()
{
}

QWidget* MultSelector::createMultIconsWidget()
{
	qDebug() << Q_FUNC_INFO;
	QWidget* setWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(15);
	QGridLayout* setLayout = new QGridLayout;
	setLayout->setSpacing(2);

	int row = 0;
	int col = 0;

	QSignalMapper* signalMapper = new QSignalMapper(this);

	Q_FOREACH (QString multId, theRM.mults()->multList())
	{
		const MultInfo* info = theRM.mults()->getMultInfo(multId);

		if (!info)
			continue;

		QString filename = theRM.flashResourcePrefix().append(":").append(info->fileName()).append(".png");

		QPixmap pm(filename);

		QPushButton* pb = new QPushButton;
		pb->setFlat(true);
		pb->setIcon(pm);
		pb->setIconSize(pm.size());

		connect(pb, SIGNAL(clicked()), signalMapper, SLOT(map()));
		signalMapper->setMapping(pb, multId);

		setLayout->addWidget(pb, row, col);
		if (col == 4)
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

void MultSelector::setCurrentLayout()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);

	mults = createMultIconsWidget();
	layout->addWidget(mults);
	setLayout(layout);
}

void MultSelector::appear(MessageEditor* editor, bool visible)
{
	m_selectorSender = editor;
	setVisible(visible);
}

void MultSelector::slotClicked(QString id)
{
	Q_EMIT selected(m_selectorSender, id);
	//close();
}
