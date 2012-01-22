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

#include "removecontactdialog.h"

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>

RemoveContactDialog::RemoveContactDialog(const QString &title, const QString &text,	QWidget *parent, Qt::WindowFlags flags)
 : QDialog(parent, flags)
{
	QVBoxLayout* layout = new QVBoxLayout;
	QLabel* textLabel = new QLabel(text);
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	QPushButton* buttonNo = new QPushButton(tr("No"));
	QPushButton* buttonYes = new QPushButton(tr("Yes"));
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(buttonNo);
	buttonsLayout->addWidget(buttonYes);
	connect(buttonNo, SIGNAL(clicked()), this, SLOT(reject()));
	connect(buttonYes, SIGNAL(clicked()), this, SLOT(accept()));
	removeHistoryCheckBox = new QCheckBox(tr("Remove all history"));
	removeHistoryCheckBox->setChecked(true);
	layout->addWidget(textLabel);
	layout->addWidget(removeHistoryCheckBox);
	layout->addLayout(buttonsLayout);
	setWindowTitle(title);
	setLayout(layout);
}

RemoveContactDialog::~RemoveContactDialog()
{
}

bool RemoveContactDialog::removeHistory()
{
	return removeHistoryCheckBox->isChecked();
}
