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

#include "emoticonselector.h"

#include <QDebug>

#include <cmath>
#include <QLabel>
#include <QGridLayout>

#include "emoticonmovie.h"
#include "emoticonwidget.h"
#include "resourcemanager.h"
#include "favouriteemoticonsdialog.h"

EmoticonSelector::EmoticonSelector(QWidget *parent)
	: QFrame(parent)
{
	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_AlwaysShowToolTips);

	setFrameStyle(QFrame::Panel | QFrame::Raised);

	setCurrentLayout();

	setFixedSize(sizeHint());
}

void EmoticonSelector::correctSize()
{
	setFixedSize(sizeHint());
}

QWidget* EmoticonSelector::createFavouriteEmoticonsWidget(int emoticonsPerRow)
{
	QWidget* setWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(15);
	QGridLayout* setLayout = new QGridLayout;
	setLayout->setSpacing(2);

	int row = 0;
	int col = 0;

	foreach (QString emoticonId, theRM.emoticons().favouriteEmoticons())
	{
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(emoticonId);

		EmoticonWidget* w = new EmoticonWidget(info->id(), this);
		w->setToolTip(info->tip());
		connect(w, SIGNAL(clicked(QString)), this, SLOT(slotClicked(QString)));
		setLayout->addWidget(w, row, col);
		if (col == emoticonsPerRow)
		{
			col = 0;
			row++;
		}
		else
			col++;
	}

	QHBoxLayout* labelLayout = new QHBoxLayout;
	QLabel* settingsLabel = new QLabel(tr("<a href=x>Select</a>"));
	connect(settingsLabel, SIGNAL(linkActivated( const QString& )), this, SLOT(setupFavouriteEmoticons()));
	labelLayout->addStretch();
	labelLayout->addWidget(settingsLabel);

	layout->addLayout(setLayout);
	layout->addLayout(labelLayout);

	setWidget->setLayout(layout);

	return setWidget;
}

EmoticonSelector::~EmoticonSelector()
{
	qDeleteAll(movieList);
}

void EmoticonSelector::slotClicked(QString id)
{
	emit selected(id);
	qDebug() << "DblClicked";
}

void EmoticonSelector::closeEvent(QCloseEvent* /*event*/)
{
	emit closed();
}

void EmoticonSelector::setupFavouriteEmoticons()
{
	FavouriteEmoticonsDialog dlg;
	connect (&dlg, SIGNAL(doubleClicked(QString)), this, SLOT(slotClicked(QString)));
	if (dlg.exec() == QDialog::Accepted)
	{
		delete this->layout();
		delete emotions;

		setCurrentLayout();
	}
}

void EmoticonSelector::setCurrentLayout()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	//layout->setSpacing(2);

	int maxSetSize = theRM.emoticons().maxSetSize();
	int numberOfFavouriteEmoticons = theRM.emoticons().favouriteEmoticons().size();
	maxSetSize = maxSetSize > numberOfFavouriteEmoticons ? maxSetSize : numberOfFavouriteEmoticons;
	int emoticonsPerRow = static_cast<int>(ceil(sqrt((double)maxSetSize)));

	emotions = createFavouriteEmoticonsWidget(emoticonsPerRow);
	layout->addWidget(emotions);
	setLayout(layout);
}
