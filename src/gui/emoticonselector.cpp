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

#include "gui/emoticonmovie.h"
#include "gui/emoticonwidget.h"
#include "resourcemanager.h"
#include "favouriteemoticonsdialog.h"

EmoticonSelectorPage::EmoticonSelectorPage(EmoticonSelector* parent, QStringList set, bool isFav)
	: QWidget(parent), m_set(set), m_isFav(isFav)
{
	m_parentSelector = parent;
	setCurrentLayout();

	setFixedSize(sizeHint());
}

void EmoticonSelectorPage::correctSize()
{
	gridLayout->setHorizontalSpacing((m_parentSelector->width() - gridLayout->sizeHint().width()) / (gridLayout->columnCount() - 1));
}

QWidget* EmoticonSelectorPage::createEmoticonsWidget()
{
	QWidget* setWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(15);
	gridLayout = new QGridLayout;
	gridLayout->setHorizontalSpacing(10);

	int row = 0;
	int col = 0;
	int width = 0;

	foreach (QString emoticonId, m_set)
	{
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(emoticonId);
		EmoticonWidget* w = new EmoticonWidget(info->id(), this);

		if (width + w->geometry().width() > 230)
		{
			col = 0;
			width = 0;
			row++;
		}

		w->setToolTip(info->tip());
		connect(w, SIGNAL(clicked(QString)), m_parentSelector, SLOT(slotClicked(QString)));
		gridLayout->addWidget(w, row, col);

		col++;
		width += w->geometry().width();
	}

	layout->addLayout(gridLayout);

	if (m_isFav)
	{
		QHBoxLayout* labelLayout = new QHBoxLayout;
		QLabel* settingsLabel = new QLabel("<a href=x>" + tr("Select") + "</a>");
		connect(settingsLabel, SIGNAL(linkActivated( const QString& )), this, SLOT(setupFavouriteEmoticons()));
		labelLayout->addStretch();
		labelLayout->addWidget(settingsLabel);
		layout->addLayout(labelLayout);
	}

	setWidget->setLayout(layout);

	return setWidget;
}

EmoticonSelectorPage::~EmoticonSelectorPage()
{
	qDeleteAll(movieList);
}

void EmoticonSelectorPage::setupFavouriteEmoticons()
{
	FavouriteEmoticonsDialog dlg;
	connect (&dlg, SIGNAL(doubleClicked(QString)), m_parentSelector, SLOT(slotClicked(QString)));
	if (dlg.exec() == QDialog::Accepted)
	{
		delete this->layout();
		delete emotions;

		setCurrentLayout();
		correctSize();
	}
}

void EmoticonSelectorPage::setCurrentLayout()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	//layout->setSpacing(2);

	int setSize = m_set.size();
	int numberOfFavouriteEmoticons = theRM.emoticons().favouriteEmoticons().size();
	setSize = setSize > numberOfFavouriteEmoticons ? setSize : numberOfFavouriteEmoticons;
	//int emoticonsPerRow = static_cast<int>(ceil(sqrt((double)setSize)));

	emotions = createEmoticonsWidget();
	layout->addWidget(emotions);
	setLayout(layout);
}

//------------------------------------------*/

EmoticonSelector::EmoticonSelector(QWidget* parent)
	: QFrame(parent)
{
	qDebug() << Q_FUNC_INFO << "{";

	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_AlwaysShowToolTips);

	setFrameStyle(QFrame::Panel | QFrame::Raised);

	QVBoxLayout* layout = new QVBoxLayout;

	tabs = new QTabWidget;
	tabs->setStyleSheet("QTabWidget::pane { border: 0px; } QTabWidget::tab-bar { left: 1px; } QTabBar::tab { padding: 5px 5px 5px 5px; font: bold 10px; }");
	tabs->setTabPosition(QTabWidget::South);

	EmoticonSelectorPage* page;
	for (Emoticons::const_iterator it = theRM.emoticons().begin(); it != theRM.emoticons().end(); ++it)
	{
		EmoticonSelectorPage* page = new EmoticonSelectorPage(this, (*it)->list());
		if ((*it)->logo() != "")
			tabs->addTab(page, QIcon(theRM.statusesResourcePrefix() + ":" + (*it)->logo()), "");
		else
			tabs->addTab(page, QIcon(""), tr("unk"));
		pages.append(page);
	}
	page = new EmoticonSelectorPage(this, theRM.emoticons().favouriteEmoticons(), true);
	tabs->addTab(page, QIcon(), tr("fav"));

	layout->setContentsMargins(1, 1, 1, 1);
	layout->addWidget(tabs);
	setLayout(layout);

	setFixedSize(sizeHint());
	for (QList<EmoticonSelectorPage*>::const_iterator it = pages.begin(); it != pages.end(); ++it)
		(*it)->correctSize();

	qDebug() << Q_FUNC_INFO << "}";
}

void EmoticonSelector::closeEvent(QCloseEvent* /*event*/)
{
	emit closed();
}

EmoticonSelector::~EmoticonSelector()
{
    qDeleteAll(pages);
}

void EmoticonSelector::slotClicked(QString id)
{
	emit selected(m_selectorSender, id);
}

void EmoticonSelector::appear(MessageEditor* editor, bool visible)
{
	m_selectorSender = editor;
	setVisible(visible);
}

