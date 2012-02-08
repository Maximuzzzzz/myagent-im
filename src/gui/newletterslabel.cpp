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

#include "newletterslabel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QFont>

#include "core/account.h"

NewLettersLabel::NewLettersLabel(Account* account, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0, 2, 0, 4);
	QLabel* iconLabel = new QLabel;
	iconLabel->setPixmap(QPixmap(":icons/letter.png"));
	iconLabel->setFixedSize(iconLabel->sizeHint());
	lettersLabel = new QLabel;
	connect(lettersLabel, SIGNAL(linkActivated(const QString&)), SIGNAL(clicked()));
	connect(account, SIGNAL(unreadLettersChanged(uint)), this, SLOT(updateLettersNumber(uint)));
	layout->addWidget(iconLabel);
	layout->addWidget(lettersLabel);
	setLayout(layout);

	QFont font;
	font.setPointSize(8);
	setFont(font);
}

NewLettersLabel::~NewLettersLabel()
{
}

void NewLettersLabel::updateLettersNumber(uint n)
{
	lettersLabel->clear();
	lettersLabel->setText("<a href=x>" + tr("%n new letter(s)", "", n) + "</a>");
}
