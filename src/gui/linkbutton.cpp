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

#include "linkbutton.h"

#include <QDebug>

#include <QMouseEvent>
#include <QDesktopServices>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QMimeData>

LinkButton::LinkButton(const QString& url, const QIcon& icon, QWidget *parent)
	: ToolButton(icon, parent), m_url(url)
{
	init();
}

LinkButton::LinkButton(QAction* action, QWidget * parent)
	: ToolButton(action, parent)
{
	init();
}

void LinkButton::init()
{
	setCursor(Qt::PointingHandCursor);
	qDebug() << "LinkButton icon size = " << icon().actualSize(QSize(24, 24));
	setFixedSize(icon().actualSize(QSize(24, 24)));
}

void LinkButton::mouseReleaseEvent(QMouseEvent* event)
{
	ToolButton::mouseReleaseEvent(event);
	
	if (event->button() & Qt::LeftButton && rect().contains(event->pos()))
	{
		if (!m_url.isEmpty())
			QDesktopServices::openUrl(m_url);
	}
	else if (event->button() & Qt::RightButton && m_url.isValid())
	{
		QMenu menu;
		menu.addAction(tr("Copy &Link Location"), this, SLOT(copyLinkLocation()));
		menu.exec(event->globalPos());
	}
}

void LinkButton::copyLinkLocation()
{
	qDebug() << "LinkButton::copyLinkLocation url = " << m_url.toString();
	QMimeData *md = new QMimeData;
    md->setText(m_url.toString());
    QApplication::clipboard()->setMimeData(md);
}
