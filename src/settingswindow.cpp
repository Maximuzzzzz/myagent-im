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

#include "settingswindow.h"

#include <QDebug>

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollBar>
#include <QMessageBox>

#include "resourcemanager.h"

SettingsWindow::SettingsWindow()
{
	setWindowTitle(tr("Settings"));
	setWindowIcon(QIcon(":icons/settings.png"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout* layout = new QVBoxLayout;

	QHBoxLayout* hl = new QHBoxLayout;

	listWidget = new QListWidget;

	QWidget* pagesLayoutWidget = new QWidget;
	QVBoxLayout* pagesLayout = new QVBoxLayout;
	pagesLayout->setContentsMargins(0, 0, 0, 0);
	pagesWidget = new QStackedWidget;
	connect(listWidget, SIGNAL(currentRowChanged(int)), pagesWidget, SLOT(setCurrentIndex(int)));
	QFrame* separator = new QFrame;
	separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	pagesLayout->addWidget(pagesWidget);
	pagesLayout->addWidget(separator);
	pagesLayoutWidget->setLayout(pagesLayout);

	hl->addWidget(listWidget);
	hl->addWidget(pagesLayoutWidget);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));

	layout->addLayout(hl);
	layout->addWidget(buttonBox);

	setLayout(layout);

	createCommonPage();
	createMessagesPage();

	listWidget->setCurrentRow(0);
	listWidget->setMaximumWidth(listWidget->sizeHintForColumn(0) + 10);
}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::createCommonPage()
{
	QWidget* commonSettingsPage = new QWidget;

	QVBoxLayout* layout = new QVBoxLayout;

	QHBoxLayout* hl = new QHBoxLayout;

	awayCheckBox = new QCheckBox(tr("Set away status after"));
	awayCheckBox->setChecked(theRM.settings()->value("Common/autoAwayEnabled", false).toBool());
	minutesEdit = new QLineEdit;
	minutesEdit->setValidator(new QIntValidator(1, 1000, minutesEdit));
	minutesEdit->setFixedWidth(minutesEdit->fontMetrics().size(Qt::TextSingleLine, "1000").width());
	minutesEdit->setText(theRM.settings()->value("Common/autoAwayMinutes", "10").toString());
	connect(awayCheckBox, SIGNAL(toggled(bool)), minutesEdit, SLOT(setEnabled(bool)));
	minutesEdit->setEnabled(awayCheckBox->isChecked());
	QLabel* tailLabel = new QLabel(tr("minutes of idle"));

	hl->addWidget(awayCheckBox);
	hl->addWidget(minutesEdit);
	hl->addWidget(tailLabel);
	hl->addStretch();

	layout->addLayout(hl);
	layout->addStretch();

	commonSettingsPage->setLayout(layout);

	listWidget->addItem(tr("Common"));
	pagesWidget->addWidget(commonSettingsPage);
}

void SettingsWindow::saveSettings()
{
	if (saveMessagesSettings())
	{
		saveCommonSettings();
		close();
	}
}

void SettingsWindow::saveCommonSettings()
{
	theRM.settings()->setValue("Common/autoAwayEnabled", awayCheckBox->isChecked());
	theRM.settings()->setValue("Common/autoAwayMinutes", minutesEdit->text());
}

void SettingsWindow::createMessagesPage()
{
	QWidget* messagesSettingsPage = new QWidget;

	QVBoxLayout* layout = new QVBoxLayout;

	QGroupBox* sendBox = new QGroupBox(tr("Sending messages"));
	QGroupBox* windowsBox = new QGroupBox(tr("Windows options"));
	QVBoxLayout* sendLayout = new QVBoxLayout;
	QVBoxLayout* windowsLayout = new QVBoxLayout;

	positWin = new QCheckBox(tr("Tabs in dialog window"));
	//positWin->setChecked(chatWindowsManager->settings()->value("Windows/UseTabs", true).toBool());
	positWin->setChecked(theRM.settings()->value("Windows/UseTabs", true).toBool());

	enterButton = new QRadioButton(tr("Send message on Enter pressed"));
	doubleEnterButton = new QRadioButton(tr("Send message on double Enter pressed"));
	ctrlEnterButton = new QRadioButton(tr("Send message on Ctrl+Enter pressed"));
	altSButton = new QCheckBox(tr("Send message on Alt+S pressed"));

	QString enterVariant = theRM.settings()->value("Messages/sendOnEnter", "Ctrl+Enter").toString();
	if (enterVariant == "Enter")
		enterButton->setChecked(true);
	else if (enterVariant == "Enter+Enter")
		doubleEnterButton->setChecked(true);
	else
		ctrlEnterButton->setChecked(true);

	altSButton->setChecked(theRM.settings()->value("Messages/sendOnAltS", false).toBool());

	windowsLayout->addWidget(positWin);
	windowsBox->setLayout(windowsLayout);

	sendLayout->addWidget(enterButton);
	sendLayout->addWidget(doubleEnterButton);
	sendLayout->addWidget(ctrlEnterButton);
	sendLayout->addWidget(altSButton);

	sendBox->setLayout(sendLayout);
	sendBox->setFixedHeight(sendBox->sizeHint().height());
	sendBox->setMinimumWidth(sendBox->sizeHint().width());

	layout->addWidget(windowsBox);
	layout->addWidget(sendBox);
	layout->addStretch();

	messagesSettingsPage->setLayout(layout);

	listWidget->addItem(tr("Messages"));
	pagesWidget->addWidget(messagesSettingsPage);
}

bool SettingsWindow::saveMessagesSettings()
{
	bool res = true;
	QString enterVariant;
	if (enterButton->isChecked())
		enterVariant = "Enter";
	else if (doubleEnterButton->isChecked())
		enterVariant = "Enter+Enter";
	else
		enterVariant = "Ctrl+Enter";

	bool useTabs = theRM.settings()->value("Windows/UseTabs", true).toBool();
	if (useTabs != positWin->isChecked() && chatWindowsManager->isAnyWindowVisible())
		if (QMessageBox::question(this, tr("Closing chats"), tr("All chats will be closed. Continue?"), QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
			return false;
	theRM.settings()->setValue("Windows/UseTabs", positWin->isChecked());
	theRM.settings()->setValue("Messages/sendOnEnter", enterVariant);
	theRM.settings()->setValue("Messages/sendOnAltS", altSButton->isChecked());
	chatWindowsManager->reloadStatus(positWin->isChecked());
	return res;
}

void SettingsWindow::setChatWindowsManager(ChatWindowsManager* cwm)
{
	chatWindowsManager = cwm;
}
