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

#include "animation.h"

#include <QPainter>
#include <QDebug>
#include <QTextEdit>

#include "gui/emoticonwidget.h"

Animation::Animation(QTextEdit* e)
	: EmoticonMovie(e)
{
	editor = e;
	setCacheMode(QMovie::CacheAll);
}

Animation::~Animation()
{
	//qDebug() << "destroying animation";
}

void Animation::insert(int id, const QRect& rect, bool selected)
{
	if (!editor)
		return;
	
	if (!emoticonWidgets.contains(id))
	{
		EmoticonWidget* widget = new EmoticonWidget(this, editor->viewport());
		emoticonWidgets[id] = widget;
	}
	
	EmoticonWidget* widget = emoticonWidgets[id];
	if (selected)
		widget->setBackgroundRole(QPalette::Highlight);
	else
		widget->setBackgroundRole(QPalette::Base);
	
	widget->move(rect.topLeft());
	widget->show();
	start();
}

void Animation::remove(int id)
{
	if (emoticonWidgets.contains(id))
	{
		delete emoticonWidgets[id];
		emoticonWidgets.remove(id);
		if (emoticonWidgets.isEmpty())
			stop();
	}
}

QSize Animation::size()
{
	//qDebug() << "Animation::size, currentFrameNumber = " << currentFrameNumber();
	return currentPixmap().size();
}

void Animation::hide()
{
	Q_FOREACH (EmoticonWidget* widget, emoticonWidgets)
		widget->hide();
}
