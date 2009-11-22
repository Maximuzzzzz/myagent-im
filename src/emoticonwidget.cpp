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

#include "emoticonwidget.h"

#include <QDebug>

#include <QPainter>
#include <QPaintEvent>
#include <QApplication>

#include "emoticonmovie.h"

EmoticonWidget::EmoticonWidget(const QString& emoticonId, QWidget* parent)
	: QWidget(parent), movie(NULL), draggable(false)
{
	init();
	
	EmoticonMovie* m = new EmoticonMovie(this);
	m->load(emoticonId);
	setMovie(m);
}

EmoticonWidget::EmoticonWidget(EmoticonMovie* m, QWidget* parent)
	: QWidget(parent), movie(NULL), draggable(false)
{
	init();
	
	setMovie(m);
}

void EmoticonWidget::init()
{
	setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_NoSystemBackground);
}

EmoticonWidget::~EmoticonWidget()
{
	qDebug() << Q_FUNC_INFO;
}

void EmoticonWidget::setMovie(EmoticonMovie* m)
{
	if (movie)
		movie->disconnect(this);
	movie = m;
	
	setFixedSize(movie->currentPixmap().size());
	connect(movie, SIGNAL(frameChanged(int)), this, SLOT(update()));
}

void EmoticonWidget::paintEvent(QPaintEvent* event)
{
	if (!movie)
		return;
	
	QPainter p(this);
	p.eraseRect(movie->currentPixmap().rect());
	p.drawPixmap(0, 0, movie->currentPixmap());
	
	event->accept();
}

void EmoticonWidget::mouseReleaseEvent(QMouseEvent* /*event*/)
{
	emit clicked(movie->id());
}

void EmoticonWidget::showEvent(QShowEvent* /*event*/)
{
	movie->start();
}

void EmoticonWidget::hideEvent(QHideEvent* /*event*/)
{
	movie->stop();
}

void EmoticonWidget::setDragEnabled(bool on)
{
	draggable = on;
}

void EmoticonWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
		dragStartPos = event->pos();
}

void EmoticonWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (!draggable)
		return;
	
	if (!(event->buttons() & Qt::LeftButton))
		return;
	
	if ((event->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance())
		return;

	QDrag* drag = new QDrag(this);
	QMimeData* mimeData = new QMimeData;
	
	mimeData->setData("Myagent-IM/emoticon", movie->id().toLatin1());
	drag->setMimeData(mimeData);
	QPixmap pm = movie->currentPixmap();
	drag->setPixmap(pm);
	drag->setHotSpot(QPoint(pm.width(), pm.height()));
	emit draggingStarted();
	drag->exec(Qt::CopyAction);
}

QString EmoticonWidget::emoticonId() const
{
	return movie->id();
}
