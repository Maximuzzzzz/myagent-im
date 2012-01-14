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

#include "contactinfolistwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>

#include "gui/contactinfolistwidget.h"
#include "gui/centerwindow.h"

ContactInfoListWindow::ContactInfoListWindow(Account* account)
{
	setWindowTitle(tr("Contacts found"));
	setWindowIcon(QIcon(":icons/cl_add_contact.png"));

	setAttribute(Qt::WA_DeleteOnClose);
	
	QVBoxLayout* layout = new QVBoxLayout;
	
	infoListWidget = new ContactInfoListWidget(account, this);
	
	QCheckBox* showPhotosCheckBox = new QCheckBox(tr("Show photos"));
	connect(showPhotosCheckBox, SIGNAL(toggled(bool)), infoListWidget, SLOT(showPhotos(bool)));

	QPushButton* addButton = new QPushButton(tr("Add contact"));
	connect(addButton, SIGNAL(clicked(bool)), this, SLOT(slotAddButtonClicked()));

	moreContactsButton = new QPushButton();
	connect(moreContactsButton, SIGNAL(clicked(bool)), SIGNAL(moreContactsButtonClicked()));

	QPushButton* newSearchButton = new QPushButton(tr("New search"));
	connect(newSearchButton, SIGNAL(clicked(bool)), SIGNAL(newSearchButtonClicked()));

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(showPhotosCheckBox);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(addButton);
	buttonsLayout->addWidget(moreContactsButton);
	buttonsLayout->addWidget(newSearchButton);
	
	layout->addWidget(infoListWidget);
	layout->addLayout(buttonsLayout);
	
	setLayout(layout);
}

void ContactInfoListWindow::setInfo(const QList<ContactInfo>& info, uint maxRows)
{
	infoListWidget->setInfo(info);
	moreContactsButton->setText(tr("Another %1 contacts").arg(maxRows));
	if (info.size() < maxRows)
		moreContactsButton->setDisabled(true);
	else
		moreContactsButton->setEnabled(true);

	resize(sizeHint());
	centerWindow(this);
}

void ContactInfoListWindow::slotAddButtonClicked()
{
	if (infoListWidget->hasSelection())
		Q_EMIT addButtonClicked(infoListWidget->selectedInfo());
}
