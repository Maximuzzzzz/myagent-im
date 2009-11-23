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
#include <QToolBox>
#include <QGridLayout>

#include "emoticonmovie.h"
#include "emoticonwidget.h"
#include "resourcemanager.h"
#include "favouriteemoticonsdialog.h"

EmoticonSelector::EmoticonSelector(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_AlwaysShowToolTips);
	setWindowTitle(tr("Emoticons"));
	setWindowIcon(QIcon("smiles:/smiles/smiles/smile.gif"));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(10, 10, 10, 10);
	//layout->setSpacing(2);

	toolBox = new QToolBox;

	int maxSetSize = theRM.emoticons().maxSetSize();
	int numberOfFavouriteEmoticons = theRM.emoticons().favouriteEmoticons().size();
	maxSetSize = maxSetSize > numberOfFavouriteEmoticons ? maxSetSize : numberOfFavouriteEmoticons;
	int emoticonsPerRow = static_cast<int>(ceil(sqrt((double)maxSetSize)));

	toolBox->addItem(createFavouriteEmoticonsWidget(emoticonsPerRow), tr("Favourite"));

	//qDebug() << "emoticonsPerRow = " << emoticonsPerRow;

/*	Emoticons::const_iterator set_it = theRM.emoticons().begin();
	Emoticons::const_iterator setEnd_it = theRM.emoticons().end();

	for (; set_it != setEnd_it; ++set_it)
	{
		//qDebug() << "emoticon set " << (*set_it)->title();
		EmoticonSet::const_iterator info_it = (*set_it)->begin();
		EmoticonSet::const_iterator infoEnd_it = (*set_it)->end();

		QWidget* setWidget = new QWidget;
		QGridLayout* setLayout = new QGridLayout;
		setLayout->setSpacing(2);

		int row = 0;
		int col = 0;

		for (; info_it != infoEnd_it; ++info_it)
		{
			EmoticonWidget* w = new EmoticonWidget((*info_it)->id(), this);
			w->setToolTip((*info_it)->tip());
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

		setWidget->setLayout(setLayout);
		toolBox->addItem(setWidget, (*set_it)->title());
	}*/

	layout->addWidget(toolBox);
	setLayout(layout);

	connect(toolBox, SIGNAL(currentChanged(int)), SLOT(correctSize()));
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
}

void EmoticonSelector::closeEvent(QCloseEvent* /*event*/)
{
	emit closed();
}

void EmoticonSelector::setupFavouriteEmoticons()
{
	FavouriteEmoticonsDialog dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		QWidget* favouriteEmotionsWidget = toolBox->widget(0);
		toolBox->removeItem(0);
		delete favouriteEmotionsWidget;
		int maxSetSize = theRM.emoticons().maxSetSize();
		int numberOfFavouriteEmoticons = theRM.emoticons().favouriteEmoticons().size();
		maxSetSize = maxSetSize > numberOfFavouriteEmoticons ? maxSetSize : numberOfFavouriteEmoticons;
		int emoticonsPerRow = static_cast<int>(ceil(sqrt((double)maxSetSize)));
		favouriteEmotionsWidget = createFavouriteEmoticonsWidget(emoticonsPerRow);
		toolBox->insertItem(0, favouriteEmotionsWidget, tr("Favourite"));
		setFixedSize(sizeHint());
		toolBox->setCurrentIndex(0);
	}
}
