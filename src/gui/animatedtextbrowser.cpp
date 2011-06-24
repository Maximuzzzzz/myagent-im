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

#include "animatedtextbrowser.h"

#include <QDesktopServices>

AnimatedTextBrowser::AnimatedTextBrowser ( QWidget *parent )
		: AnimationSupport<QTextBrowser> ( parent )
{
	connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(processDocumentChange(int,int,int)));

	setOpenLinks(false);
	connect(this, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(openUrl(const QUrl&)));
}

void AnimatedTextBrowser::openUrl(const QUrl & url)
{
	QString scheme = url.scheme();
	if (!scheme.isEmpty() && /*scheme != "file" &&*/ scheme != "qrc")
		QDesktopServices::openUrl(url);
}

/*void AnimatedTextBrowser::setSource(const QString & name)
{
	qDebug() << "AnimatedTextBrowser::setSource" << name;

//	QString s = name.stripWhiteSpace();
	QTextBrowser::setSource(name);
}*/
