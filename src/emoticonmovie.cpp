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

#include "emoticonmovie.h"

#include "resourcemanager.h"

#include <QDebug>

EmoticonMovie::EmoticonMovie(QObject *parent)
	: QMovie(parent)
{
	animated = false;
}

EmoticonMovie::~EmoticonMovie()
{
}

bool EmoticonMovie::isAnimated(const QString& fileName)
{
	QMovie movie;
	movie.setCacheMode(QMovie::CacheAll);
	movie.setFileName(fileName);
	
	if (!movie.isValid())
		return false;
	
	movie.jumpToFrame(1);

	if (movie.currentFrameNumber() != 1)
		return false;
	else
		return true;
}

void EmoticonMovie::setFileName(const QString& fileName)
{
	QMovie::setFileName(fileName);
	animated = isAnimated(fileName);
}

void EmoticonMovie::start()
{
	if (!animated)
		return;
	
	QMovie::start();
}

void EmoticonMovie::setPaused(bool paused)
{
	if (!animated)
		return;
	
	QMovie::setPaused(paused);
}

void EmoticonMovie::load(const QString& id, int from)
{
	if (from == 0)
	{
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(id);

		if (!info)
		{
			qDebug() << "EmoticonMovie::load: can't get emoticon for id = " << id;
			return;
		}

		QString filename = theRM.emoticonsResourcePrefix() + ":" + info->path();
		setFileName(filename);
		id_ = id;

	}
	else if (from == 1)
	{
		const OnlineStatusInfo* info = theRM.onlineStatuses()->getOnlineStatusInfo(id);

		if (!info)
		{
			qDebug() << "EmoticonMovie::load: can't get online status for id = " << id;
			return;
		}

		QString filename = theRM.statusesResourcePrefix() + ":" + info->icon();
		setFileName(filename);
		id_ = id;
	}

	jumpToFrame(0);
	QSize size = currentPixmap().size();
	//qDebug() << "EmoticonMovie::load currentPixmap size = " << size;
}
