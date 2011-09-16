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

#include "statusbarwidget.h"

#include <QHBoxLayout>
#include <QFont>
#include <QDebug>
#include <QTextCodec>
#include <QMouseEvent>

StatusBarWidget::StatusBarWidget()
{
	QHBoxLayout* statusLabelLayout = new QHBoxLayout;
	statusLabelLayout->setContentsMargins(0, 2, 0, 4);
	QLabel* iconStatusLabel = new QLabel;
	iconStatusLabel->setPixmap(QPixmap(":icons/bubble.png"));
	iconStatusLabel->setFixedSize(iconStatusLabel->sizeHint());

	statusLabel = new QLabel;
	statusLabel->setWordWrap(true);

	statusLabelLayout->addWidget(iconStatusLabel);
	statusLabelLayout->addWidget(statusLabel);
	setLayout(statusLabelLayout);

	setActive(false);
	clearStatus();
}

StatusBarWidget::~StatusBarWidget()
{
	qDebug() << "StatusBarWidget::~StatusBarWidget()";
}

void StatusBarWidget::clearStatus()
{
	statusLabel->setText("<font color=#888888>" + tr("Say to your friends, what are you doing") + "</font>");

	QFont font;
	font.setPointSize(8);
	setFont(font);
}

void StatusBarWidget::setStatus(const QString& status)
{
	qDebug() << "StatusBarWidget::setStatus(" << status << ")";

	statusLabel->setText("<font color=#000000>" + status + "</font>");
}

void StatusBarWidget::setStatus(QByteArray status)
{
	qDebug() << "StatusBarWidget::setStatus(" << status << ")";

	QTextCodec* codec = QTextCodec::codecForName("CP1251");
	statusLabel->setText("<font color=#000000>" + codec->toUnicode(status) + "</font>");
}

void StatusBarWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		if (active)
			Q_EMIT clicked();
}

void StatusBarWidget::setActive(bool state)
{
	qDebug() << "StatusBarWidget::setActive" << state;

	active = state;
	QCursor cursor;
	if (state)
		cursor.setShape(Qt::PointingHandCursor);
	else
		cursor.setShape(Qt::ArrowCursor);
	setCursor(cursor);
}
