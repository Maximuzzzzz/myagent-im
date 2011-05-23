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
#include <QProcess>

#include "resourcemanager.h"
#include "contact.h"

SettingsWindow::SettingsWindow(Account* account, ContactListWindow* clw)
	: m_account(account), m_clw(clw)
{
	m_flags =  0xff;
	init();
}

SettingsWindow::SettingsWindow(quint8 flags)
	: m_flags(flags)
{
	init();
}

void SettingsWindow::init()
{
	setWindowTitle(tr("Settings"));
	setWindowIcon(QIcon(":icons/settings.png"));
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowModality(Qt::ApplicationModal);

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

	if (m_flags & SHOW_COMMON_PAGE)
		createCommonPage();
	if (m_flags & SHOW_MESSAGES_PAGE)
		createMessagesPage();
	if (m_flags & SHOW_WINDOWS_PAGE)
		createWindowsPage();
	if (m_flags & SHOW_AUDIO_PAGE)
		createAudioPage();
	if (m_flags & SHOW_VIEW_PAGE)
		createViewPage();
	if (m_flags & SHOW_CONNECTION_PAGE)
		createConnectionPage();
	if (m_flags & SHOW_NOTIFICATION_PAGE)
		createNotificationPage();

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

	QHBoxLayout* hl1 = new QHBoxLayout;
	QHBoxLayout* hl2 = new QHBoxLayout;

	awayCheckBox = new QCheckBox(tr("Set away status after"));
	awayCheckBox->setChecked(m_account->settings()->value("Common/autoAwayEnabled", false).toBool());
	minutesEdit = new QLineEdit;
	minutesEdit->setValidator(new QIntValidator(1, 1000, minutesEdit));
	minutesEdit->setFixedWidth(minutesEdit->fontMetrics().size(Qt::TextSingleLine, "1000").width());
	minutesEdit->setText(m_account->settings()->value("Common/autoAwayMinutes", "10").toString());
	connect(awayCheckBox, SIGNAL(toggled(bool)), minutesEdit, SLOT(setEnabled(bool)));
	minutesEdit->setEnabled(awayCheckBox->isChecked());
	QLabel* tailLabel = new QLabel(tr("minutes of idle"));
	QLabel* labelOnlineStatuses = new QLabel(tr("Type extended online statuses count (5-10):"));
	onlineStatusesCount = new QLineEdit;
	onlineStatusesCount->setValidator(new QIntValidator(5, 10, minutesEdit));
	onlineStatusesCount->setFixedWidth(minutesEdit->fontMetrics().size(Qt::TextSingleLine, "1000").width());
	onlineStatusesCount->setText(m_account->settings()->value("Statuses/count", theRM.minDefaultStatuses).toString());

	hl1->addWidget(awayCheckBox);
	hl1->addWidget(minutesEdit);
	hl1->addWidget(tailLabel);
	hl1->addStretch();
	hl2->addWidget(labelOnlineStatuses);
	hl2->addWidget(onlineStatusesCount);
	hl2->addStretch();

	layout->addLayout(hl1);
	layout->addLayout(hl2);
	layout->addStretch();

	commonSettingsPage->setLayout(layout);

	listWidget->addItem(tr("Common"));
	pagesWidget->addWidget(commonSettingsPage);
}

void SettingsWindow::saveSettings()
{
	if (m_flags & SHOW_COMMON_PAGE)
		saveCommonSettings();
	if (m_flags & SHOW_MESSAGES_PAGE)
		if (!saveMessagesSettings())
			return;
	if (m_flags & SHOW_AUDIO_PAGE)
		saveAudioSettings();
	if (m_flags & SHOW_VIEW_PAGE)
		saveViewSettings();
	if (m_flags & SHOW_CONNECTION_PAGE)
		saveConnectionSettings();
	if (m_flags & SHOW_WINDOWS_PAGE)
		if (!saveWindowsSettings())
			return;
	if (m_flags & SHOW_NOTIFICATION_PAGE)
		saveNotificationSettings();
	close();
}

void SettingsWindow::saveCommonSettings()
{
	m_account->settings()->setValue("Common/autoAwayEnabled", awayCheckBox->isChecked());
	m_account->settings()->setValue("Common/autoAwayMinutes", minutesEdit->text());
	if (onlineStatusesCount->text().toInt() >= 5 && onlineStatusesCount->text().toInt() <= 10)
	{
		m_account->settings()->setValue("Statuses/count", onlineStatusesCount->text());
		emit statusesCountChanged();
	}
}

void SettingsWindow::createMessagesPage()
{
	QWidget* messagesSettingsPage = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;

	QGroupBox* sendBox = new QGroupBox(tr("Sending messages"));
	QVBoxLayout* sendLayout = new QVBoxLayout;

	enterButton = new QRadioButton(tr("Send message on Enter pressed"));
	doubleEnterButton = new QRadioButton(tr("Send message on double Enter pressed"));
	ctrlEnterButton = new QRadioButton(tr("Send message on Ctrl+Enter pressed"));
	altSButton = new QCheckBox(tr("Send message on Alt+S pressed"));

	mergeMessagesCheckBox = new QCheckBox(tr("Merge few messages from one contact"));
	mergeMessagesCheckBox->setChecked(m_account->settings()->value("Messages/mergeMessages", true).toBool());

	QString enterVariant = m_account->settings()->value("Messages/sendOnEnter", "Ctrl+Enter").toString();
	if (enterVariant == "Enter")
		enterButton->setChecked(true);
	else if (enterVariant == "Enter+Enter")
		doubleEnterButton->setChecked(true);
	else
		ctrlEnterButton->setChecked(true);

	altSButton->setChecked(m_account->settings()->value("Messages/sendOnAltS", false).toBool());

	sendLayout->addWidget(enterButton);
	sendLayout->addWidget(doubleEnterButton);
	sendLayout->addWidget(ctrlEnterButton);
	sendLayout->addWidget(altSButton);

	sendLayout->addWidget(mergeMessagesCheckBox);

	sendBox->setLayout(sendLayout);
	sendBox->setFixedHeight(sendBox->sizeHint().height());
	sendBox->setMinimumWidth(sendBox->sizeHint().width());

	QGroupBox* dtSettings = new QGroupBox(tr("Date time settings"));
	QVBoxLayout* dtSettingsLayout = new QVBoxLayout;
	QHBoxLayout* dtFormatLayout = new QHBoxLayout;

	dateTimeFormat = new QLineEdit(m_account->settings()->value("Messages/DateMask", theRM.defDateFormat).toString());
	helpFormat = new QPushButton("?");
	connect(helpFormat, SIGNAL(clicked()), this, SLOT(showFormatHelp()));
	dtFormatLayout->addWidget(dateTimeFormat);
	dtFormatLayout->addWidget(helpFormat);

	dtSettingsLayout->addLayout(dtFormatLayout);

	dtSettings->setLayout(dtSettingsLayout);

	layout->addWidget(sendBox);
	layout->addWidget(dtSettings);
	layout->addStretch();

	messagesSettingsPage->setLayout(layout);

	listWidget->addItem(tr("Messages"));
	pagesWidget->addWidget(messagesSettingsPage);
}

bool SettingsWindow::saveMessagesSettings()
{
	QString enterVariant;
	if (enterButton->isChecked())
		enterVariant = "Enter";
	else if (doubleEnterButton->isChecked())
		enterVariant = "Enter+Enter";
	else
		enterVariant = "Ctrl+Enter";

	if (QDateTime::currentDateTime().toString(dateTimeFormat->text()) == "")
	{
		QMessageBox::critical(this, tr("Error saving settings"), tr("Error date time format! Check format string or use default \"%1\"!").arg(theRM.defDateFormat), QMessageBox::Ok);
		return false;
	}

	m_account->settings()->setValue("Messages/sendOnEnter", enterVariant);
	m_account->settings()->setValue("Messages/sendOnAltS", altSButton->isChecked());
	m_account->settings()->setValue("Messages/mergeMessages", mergeMessagesCheckBox->isChecked());

	m_account->settings()->setValue("Messages/DateMask", dateTimeFormat->text());
	return true;
}

void SettingsWindow::setChatWindowsManager(ChatWindowsManager* cwm)
{
	chatWindowsManager = cwm;
}

void SettingsWindow::createWindowsPage()
{
	QWidget* page = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;

	tabWindows = new QCheckBox(tr("Tabs in dialog window"));
	tabWindows->setChecked(m_account->settings()->value("Windows/UseTabs", true).toBool());

	layout->addWidget(tabWindows);
	layout->addStretch();

	page->setLayout(layout);

	listWidget->addItem(tr("Windows"));
	pagesWidget->addWidget(page);
}

bool SettingsWindow::saveWindowsSettings()
{
	bool useTabs = m_account->settings()->value("Windows/UseTabs", true).toBool();
	if (useTabs != tabWindows->isChecked() && chatWindowsManager->isAnyWindowVisible())
	{
		if (QMessageBox::question(this,
					  tr("Closing chats"),
					  tr("All chats will be closed. Continue?"),
					  QMessageBox::Yes, QMessageBox::No)
			!= QMessageBox::Yes)
			return false;
	}
	m_account->settings()->setValue("Windows/UseTabs", tabWindows->isChecked());
	chatWindowsManager->reloadStatus(tabWindows->isChecked());
	return true;
}

void SettingsWindow::createAudioPage()
{
	QWidget* page = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;

	enableSounds = new QCheckBox(tr("Enable sounds"));
	enableSounds->setChecked(m_account->settings()->value("Sounds/Enable", true).toBool());

	layout->addWidget(enableSounds);
	layout->addStretch();

	page->setLayout(layout);

	listWidget->addItem(tr("Sounds"));
	pagesWidget->addWidget(page);
}

void SettingsWindow::saveAudioSettings()
{
	m_account->settings()->setValue("Sounds/Enable", enableSounds->isChecked());
}

void SettingsWindow::createViewPage()
{
	QWidget* page = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;

	enableMicroBlog = new QCheckBox(tr("Enable microblog"));
	enableMicroBlog->setChecked(m_account->settings()->value("MicroBlog/Enable", true).toBool());

	sortGroups = new QCheckBox(tr("Sort user groups by name"));
	sortGroups->setChecked(m_account->settings()->value("ContactListWindow/SortGroups", true).toBool());

	layout->addWidget(enableMicroBlog);
	layout->addWidget(sortGroups);
	layout->addStretch();

	page->setLayout(layout);

	listWidget->addItem(tr("View"));
	pagesWidget->addWidget(page);
}

void SettingsWindow::saveViewSettings()
{
	m_account->settings()->setValue("MicroBlog/Enable", enableMicroBlog->isChecked());
	m_account->settings()->setValue("ContactListWindow/SortGroups", sortGroups->isChecked());
	m_clw->visibleWidget(ContactListWindow::MicroBlog, enableMicroBlog->isChecked());
	m_account->contactList()->update();
}

void SettingsWindow::createConnectionPage()
{
	QWidget* page = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout;
	QVBoxLayout* selectorLayout = new QVBoxLayout;

	QGroupBox* proxyType = new QGroupBox(tr("Choose proxy type"));
	typeSimple = new QRadioButton(tr("Without proxy"));
	typeSocks5 = new QRadioButton(tr("Socks5"));
	typeHttp = new QRadioButton(tr("HTTP-proxy"));
	typeHttps = new QRadioButton(tr("HTTPS"));
	connect(typeSimple, SIGNAL(toggled(bool)), this, SLOT(setProxyType()));
	connect(typeSocks5, SIGNAL(toggled(bool)), this, SLOT(setProxyType()));
	connect(typeHttp, SIGNAL(toggled(bool)), this, SLOT(setProxyType()));
	connect(typeHttps, SIGNAL(toggled(bool)), this, SLOT(setProxyType()));

	selectorLayout->addWidget(typeSimple);
	selectorLayout->addWidget(typeSocks5);
	selectorLayout->addWidget(typeHttp);
	selectorLayout->addWidget(typeHttps);
	selectorLayout->addStretch();

	proxyType->setLayout(selectorLayout);
	proxyType->setFixedHeight(proxyType->sizeHint().height());
	proxyType->setMinimumWidth(proxyType->sizeHint().width());

	layout->addWidget(proxyType);

	QVBoxLayout* rightSide = new QVBoxLayout;
	QVBoxLayout* serverSettings = new QVBoxLayout;
	QVBoxLayout* authLayout = new QVBoxLayout;

	QHBoxLayout* hostLayout = new QHBoxLayout;
	QHBoxLayout* portLayout = new QHBoxLayout;

	QLabel* hostLabel = new QLabel(tr("Server:"));
	QLabel* portLabel = new QLabel(tr("Port:"));
	host = new QLineEdit;
	port = new QLineEdit;

	hostLayout->addWidget(hostLabel);
	hostLayout->addWidget(host);

	portLayout->addWidget(portLabel);
	portLayout->addWidget(port);

	serverSettings->addLayout(hostLayout);
	serverSettings->addLayout(portLayout);
	serverSettings->addStretch();

	authCheck = new QCheckBox("Authentification");
	connect(authCheck, SIGNAL(toggled(bool)), this, SLOT(setProxyType()));

	QHBoxLayout* userLayout = new QHBoxLayout;
	QHBoxLayout* passwordLayout = new QHBoxLayout;

	QLabel* userLabel = new QLabel(tr("User:"));
	QLabel* passwordLabel = new QLabel(tr("Password:"));
	user = new QLineEdit;
	password = new QLineEdit;
	password->setEchoMode(QLineEdit::Password);

	userLayout->addWidget(userLabel);
	userLayout->addWidget(user);

	passwordLayout->addWidget(passwordLabel);
	passwordLayout->addWidget(password);

	authLayout->addWidget(authCheck);
	authLayout->addLayout(userLayout);
	authLayout->addLayout(passwordLayout);
	authLayout->addStretch();

	rightSide->addLayout(serverSettings);
	rightSide->addLayout(authLayout);

	layout->addLayout(rightSide);

	QString connType = theRM.settings()->value("Connection/type", "Simple").toString();
	if (connType == "Socks5")
		typeSocks5->setChecked(true);
	else if (connType == "HTTP")
		typeHttp->setChecked(true);
	else if (connType == "HTTPS")
		typeHttps->setChecked(true);
	else
		typeSimple->setChecked(true);

	host->setText(theRM.settings()->value("Connection/host", "").toString());
	port->setText(theRM.settings()->value("Connection/port", "").toString());
	user->setText(theRM.settings()->value("Connection/user", "").toString());
	password->setText(theRM.settings()->value("Connection/password", "").toString());
	authCheck->setChecked(theRM.settings()->value("Connection/authChecked", false).toBool());

	page->setLayout(layout);
	listWidget->addItem(tr("Connection"));
	pagesWidget->addWidget(page);
}


void SettingsWindow::saveConnectionSettings()
{
	if (typeSimple->isChecked())
		theRM.settings()->remove("Connection/type");
	else if (typeSocks5->isChecked())
		theRM.settings()->setValue("Connection/type", "Socks5");
	else if (typeHttp->isChecked())
		theRM.settings()->setValue("Connection/type", "HTTP");
	else if (typeHttps->isChecked())
		theRM.settings()->setValue("Connection/type", "HTTPS");

	if (authCheck->isEnabled())
		theRM.settings()->setValue("Connection/authChecked", authCheck->isChecked());
	else
		theRM.settings()->remove("Connection/authChecked");

	if (user->isEnabled())
		theRM.settings()->setValue("Connection/user", user->text());
	else
		theRM.settings()->remove("Connection/user");

	if (host->isEnabled())
		theRM.settings()->setValue("Connection/host", host->text());
	else
		theRM.settings()->remove("Connection/host");

	if (port->isEnabled())
		theRM.settings()->setValue("Connection/port", port->text());
	else
		theRM.settings()->remove("Connection/port");

	if (password->isEnabled())
		theRM.settings()->setValue("Connection/password", password->text());
	else
		theRM.settings()->remove("Connection/password");
}

void SettingsWindow::createNotificationPage()
{
	QWidget* notificationPage = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	QGroupBox* notificationType = new QGroupBox(tr("Notification type"));

	QVBoxLayout* notificationLayout = new QVBoxLayout;

	notificationTypeInner = new QRadioButton(tr("Inner"));
	notificationTypeExternal = new QRadioButton(tr("External"));
	QLabel* labelMessageReceived = new QLabel(tr("New message notification:"));
	QLabel* labelLetterReceived = new QLabel(tr("New letter notification:"));
	QLabel* labelLettersCount = new QLabel(tr("Unread letters notification:"));
	messageReceivedCommand = new QLineEdit(m_account->settings()->value("Notification/MessageReceivedText", "").toString());
	letterReceivedCommand = new QLineEdit(m_account->settings()->value("Notification/LetterReceivedText", "").toString());
	lettersCountCommand = new QLineEdit(m_account->settings()->value("Notification/LettersCount", "").toString());
	messageReceivedProbe = new QPushButton(tr("Test"));
	letterReceivedProbe = new QPushButton(tr("Test"));
	lettersCountProbe = new QPushButton(tr("Test"));
	QHBoxLayout* messageReceivedLayout = new QHBoxLayout;
	QHBoxLayout* letterReceivedLayout = new QHBoxLayout;
	QHBoxLayout* lettersCountLayout = new QHBoxLayout;
	QPushButton* helpFormat = new QPushButton(tr("Format?"));

	notificationLayout->addWidget(notificationTypeInner);
	notificationLayout->addWidget(notificationTypeExternal);

	messageReceivedLayout->addWidget(messageReceivedCommand);
	messageReceivedLayout->addWidget(messageReceivedProbe);
	letterReceivedLayout->addWidget(letterReceivedCommand);
	letterReceivedLayout->addWidget(letterReceivedProbe);
	lettersCountLayout->addWidget(lettersCountCommand);
	lettersCountLayout->addWidget(lettersCountProbe);

	notificationLayout->addWidget(labelMessageReceived);
	notificationLayout->addLayout(messageReceivedLayout);
	notificationLayout->addWidget(labelLetterReceived);
	notificationLayout->addLayout(letterReceivedLayout);
	notificationLayout->addWidget(labelLettersCount);
	notificationLayout->addLayout(lettersCountLayout);
	notificationLayout->addWidget(helpFormat);

	connect(notificationTypeInner, SIGNAL(toggled(bool)), this, SLOT(notificationTypeChanged()));
	connect(messageReceivedProbe, SIGNAL(clicked()), this, SLOT(execNotifyMessageReceived()));
	connect(letterReceivedProbe, SIGNAL(clicked()), this, SLOT(execNotifyLetterReceived()));
	connect(lettersCountProbe, SIGNAL(clicked()), this, SLOT(execNotifyLettersCount()));
	connect(helpFormat, SIGNAL(clicked()), this, SLOT(slotNotificationFormat()));

	notificationType->setLayout(notificationLayout);

	notificationTypeInner->setChecked(m_account->settings()->value("Notification/Inner", true).toBool());
	notificationTypeExternal->setChecked(!notificationTypeInner->isChecked());

	layout->addWidget(notificationType);

	notificationPage->setLayout(layout);

	listWidget->addItem(tr("Notifications"));
	pagesWidget->addWidget(notificationPage);
}

void SettingsWindow::saveNotificationSettings()
{
	m_account->settings()->setValue("Notification/Inner", notificationTypeInner->isChecked());
/*	messageReceivedCommand->setEnabled(!notificationTypeInner->isChecked());
	letterReceivedCommand->setEnabled(!notificationTypeInner->isChecked());*/
	m_account->settings()->setValue("Notification/MessageReceivedText", messageReceivedCommand->text());
	m_account->settings()->setValue("Notification/LetterReceivedText", letterReceivedCommand->text());
	m_account->settings()->setValue("Notification/LettersCount", lettersCountCommand->text());

	emit newNotificationType();
}


void SettingsWindow::setProxyType()
{
	if (typeSimple->isChecked() || typeHttps->isChecked())
	{
		host->setEnabled(false);
		port->setEnabled(false);
		authCheck->setEnabled(false);
		user->setEnabled(false);
		password->setEnabled(false);
	}
	if (typeHttp->isChecked() || typeSocks5->isChecked())
	{
		host->setEnabled(true);
		port->setEnabled(true);
		authCheck->setEnabled(true);
	}
	user->setEnabled(authCheck->isChecked() && authCheck->isEnabled());
	password->setEnabled(authCheck->isChecked() && authCheck->isEnabled());
}

void SettingsWindow::showFormatHelp()
{
	QMessageBox::information(this, tr("Date time format help"), tr("%1\tthe day as number without a leading zero (1 to 31)\n%2\tthe day as number with a leading zero (01 to 31)\n%3\tthe abbreviated localized day name (e.g. 'Mon' to 'Sun').\n%4\tthe long localized day name (e.g. 'Monday' to 'Sunday').\n%5\tthe month as number without a leading zero (1-12)\n%6\tthe month as number with a leading zero (01-12)\n%7\tthe abbreviated localized month name (e.g. 'Jan' to 'Dec').\n%8\tthe long localized month name (e.g. 'January' to 'December').\n%9\tthe year as two digit number (00-99)\n%10\tthe year as four digit number\n\n%11\tthe hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)\n%12\tthe hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)\n%13\tthe minute without a leading zero (0 to 59)\n%14\tthe minute with a leading zero (00 to 59)\n%15\tthe second without a leading zero (0 to 59)\n%16\tthe second with a leading zero (00 to 59)\n%17\tthe milliseconds without leading zeroes (0 to 999)\n%18\tthe milliseconds with leading zeroes (000 to 999)\n%19\tuse AM/PM display. AP will be replaced by either \"AM\" or \"PM\".\n%20\tuse am/pm display. ap will be replaced by either \"am\" or \"pm\".").arg("d").arg("dd").arg("ddd").arg("dddd").arg("M").arg("MM").arg("MMM").arg("MMMM").arg("yy").arg("yyyy").arg("h").arg("hh").arg("m").arg("mm").arg("s").arg("ss").arg("z").arg("zzz").arg("AP").arg("ap"), QMessageBox::Ok);
}

void SettingsWindow::notificationTypeChanged()
{
	messageReceivedCommand->setEnabled(!notificationTypeInner->isChecked());
	letterReceivedCommand->setEnabled(!notificationTypeInner->isChecked());
	lettersCountCommand->setEnabled(!notificationTypeInner->isChecked());
	messageReceivedProbe->setEnabled(!notificationTypeInner->isChecked());
	letterReceivedProbe->setEnabled(!notificationTypeInner->isChecked());
	lettersCountProbe->setEnabled(!notificationTypeInner->isChecked());
}

void SettingsWindow::execNotifyMessageReceived()
{
	ContactList::ConstContactsIterator it = m_account->contactList()->contactsBegin();
	if (++it != m_account->contactList()->contactsEnd())
		QProcess::execute(messageReceivedCommand->text().replace("%me", m_account->email()).replace("%ME", m_account->nickname()).replace("%c", (*it)->email()).replace("%C", (*it)->nickname()).replace("%d", QDateTime::currentDateTime().toString()).replace("%n", "\n"));
	else
		QProcess::execute(messageReceivedCommand->text().replace("%me", m_account->email()).replace("%ME", m_account->nickname()).replace("%c", tr("linustorvalds@mail.ru")).replace("%C", tr("Linus Torvalds")).replace("%d", QDateTime::currentDateTime().toString()).replace("%n", "\n"));
}

void SettingsWindow::execNotifyLetterReceived()
{
	QProcess::execute(letterReceivedCommand->text().replace("%me", m_account->email()).replace("%ME", m_account->nickname()).replace("%c", tr("torvalds@klaava.Helsinki.Fi")).replace("%S", "Hello everybody out there using minix").replace("%d", QDateTime::currentDateTime().toString()).replace("%n", "\n"));
}

void SettingsWindow::execNotifyLettersCount()
{
	quint32 unreadMessages = 50;
	QProcess::execute(lettersCountCommand->text().replace("%me", m_account->email()).replace("%ME", m_account->nickname()).replace("%L", QString::number(unreadMessages)).replace("%n", "\n"));
}

void SettingsWindow::slotNotificationFormat()
{
	QMessageBox::information(this, tr("Notifications format help"), tr("%1\tYour email\n%2\tYour nickname\n%3\tContact's email\n%4\tContacts nickname\n%5\tLetters quantity\n%6\tLetter subject\n%7\tDate time\n%8\tNew string\n\nFor new messages you can use only %1, %2, %3, %4, %7 and %8.\nFor new letter only %1, %2, %3, %6, %7, and %8.\nFor unread letters count only %1, %2, %5 and %8.").arg("%me").arg("%ME").arg("%c").arg("%C").arg("%L").arg("%S").arg("%d").arg("%n"), QMessageBox::Ok);
}
