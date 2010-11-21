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

#include "contactlistwindow.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QDesktopServices>

#include "account.h"
#include "contactlist.h"
#include "contactlistmodel.h"
#include "contactlistsortfilterproxymodel.h"
#include "contactlisttreeview.h"
#include "contactlistitem.h"
#include "mrimclient.h"
#include "proto.h"
#include "chatwindow.h"
#include "chatwindowsmanager.h"
#include "chatsmanager.h"
#include "systemtrayicon.h"
#include "resourcemanager.h"
#include "statusbutton.h"
#include "mainmenubutton.h"
#include "logindialog.h"
#include "authorizedialog.h"
#include "taskaddcontact.h"
#include "taskgetmpopsession.h"
#include "contact.h"
#include "newletterslabel.h"
#include "linkbutton.h"
#include "centerwindow.h"
#include "centeredmessagebox.h"
#include "filtercontactslineedit.h"
#include "triggeroptionbutton.h"
#include "audio.h"

ContactListWindow::ContactListWindow(Account* account)
	: m_account(account)
{
	QIcon windowIcon;
	windowIcon.addFile(":icons/title_32x32.png");
	windowIcon.addFile(":icons/title_16x16.png");
	setWindowIcon(windowIcon);
	setWindowTitle(account->email());

	MRIMClient* mc = account->client();
	connect (account, SIGNAL(statusChanged(QString)), this, SLOT(slotMicroblogChanged(QString)));

	createActions();

	connect(mc, SIGNAL(loginRejected(QString)), this, SLOT(slotLoginRejected(QString)));
	connect(mc, SIGNAL(loggedOut(quint32)), this, SLOT(slotLoggedOut(quint32)));
	connect(mc, SIGNAL(contactAsksAuthorization(const QByteArray&, const QString&, const QString&)), this, SLOT(slotContactAsksAuthorization(const QByteArray&, const QString&, const QString&)));
	connect(mc, SIGNAL(microblogChanged(QString)), this, SLOT(slotMicroblogChanged(QString)));
	connect(mc, SIGNAL(conferenceAsked(const QByteArray&, const QString&)), m_account->contactList(), SLOT(addConferenceOnServer(const QByteArray&, const QString&)));

	chatWindowsManager = new ChatWindowsManager(m_account, this);

	QHBoxLayout* headerLayout = new QHBoxLayout;
	headerLayout->setSpacing(4);
	NewLettersLabel* newLettersLabel = new NewLettersLabel(m_account);
	connect(newLettersLabel, SIGNAL(clicked()), this, SLOT(openMail()));
	headerLayout->addWidget(newLettersLabel);
	headerLayout->addStretch();
	headerLayout->addWidget(new LinkButton(m_myWorldAction));
	headerLayout->addWidget(new LinkButton(m_myBlogAction));
	headerLayout->addWidget(new LinkButton(m_myPhotosAction));
	headerLayout->addWidget(new LinkButton(m_myVideosAction));

	QHBoxLayout* filterLayout = new QHBoxLayout;
	FilterContactsLineEdit* filterLineEdit = new FilterContactsLineEdit;
	filterLayout->addWidget(filterLineEdit);

	TriggerOptionButton* onlineOnlyContactsButton =
			new TriggerOptionButton("online_only", "ContactListWindow/showOnlineOnlyContacts");
	filterLayout->addWidget(onlineOnlyContactsButton);

	contactsTreeView = new ContactListTreeView(mc->account());
	ContactListModel* model = new ContactListModel(mc->account()->contactList());
	ContactListSortFilterProxyModel* proxyModel = new ContactListSortFilterProxyModel(model);
	proxyModel->setSourceModel(model);
	proxyModel->sort(0);

	contactsTreeView->setModel(proxyModel);

	connect(contactsTreeView, SIGNAL(contactItemActivated(Contact*)), this, SLOT(slotContactItemActivated(Contact*)));
	connect(proxyModel, SIGNAL(modelRebuilded()), contactsTreeView, SLOT(expandAll()));

	connect(filterLineEdit,SIGNAL(textChanged(const QString&)), proxyModel, SLOT(setFilterString(const QString&)));
	connect(onlineOnlyContactsButton, SIGNAL(toggled(bool)), proxyModel, SLOT(allowOnlineOnlyContacts(bool)));
	proxyModel->allowOnlineOnlyContacts(onlineOnlyContactsButton->isChecked());

	statusBar = new StatusBarWidget();
	connect(statusBar, SIGNAL(clicked()), this, SLOT(openStatusEditor()));

	statusEditor = new StatusEditor(this);
	connect(statusEditor, SIGNAL(sendMicrotext(const QString&)), this, SLOT(sendMicrotext(const QString&)));

	statusButton = new StatusButton;
	connect(statusButton, SIGNAL(statusChanged(OnlineStatus)), account, SLOT(setOnlineStatus(OnlineStatus)));
	connect(account, SIGNAL(onlineStatusChanged(OnlineStatus)), this, SLOT(slotSetOnlineStatus(OnlineStatus)));

	mainMenuButton = new MainMenuButton(account, this);
	mainMenuButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
	mainMenuButton->setChatWindowsManager(chatWindowsManager);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setSpacing(1);

	layout->addLayout(headerLayout);
	layout->addLayout(filterLayout);
	layout->addWidget(contactsTreeView);
	layout->addWidget(statusBar);

	QHBoxLayout* bottonLayout = new QHBoxLayout;
	bottonLayout->addWidget(mainMenuButton);
	bottonLayout->addWidget(statusButton);

	layout->addLayout(bottonLayout);

	setLayout(layout);

	if (!m_account->settings()->value("MicroBlog/Enable", true).toBool())
		statusBar->hide();

	readSettings();

	sysTray = new SystemTrayIcon(m_account, this);
	sysTray->show();

	connect(&idleDetector, SIGNAL(secondsIdle(int)), SLOT(checkAutoAwayTime(int)));
	idleDetector.start();
}

ContactListWindow::~ContactListWindow()
{
	qDebug() << "ContactListWindow::~ContactListWindow";
	writeSettings();
	qDebug() << "ContactListWindow::~ContactListWindow settings wrote";
}

void ContactListWindow::slotContactItemActivated(Contact* contact)
{
	qDebug() << "contact activated";

	if (contact)
	{
		ChatSession* session = m_account->chatsManager()->getSession(contact);
		ChatWindow* wnd = chatWindowsManager->getWindow(session);
		chatWindowsManager->raiseWindow(wnd);
	}
}

void ContactListWindow::writeSettings()
{
	qDebug() << "ContactListWindow::writeSettings, pos = " << pos() << ", size = " << size();
	QSettings* settings = m_account->settings();
	if (settings)
	{
		settings->setValue("ContactListWindow/pos", pos());
		settings->setValue("ContactListWindow/size", size());
	}
}

void ContactListWindow::readSettings()
{
	QPoint pos(0, 0);
	QSize size(200, 400);

	QSettings* settings = m_account->settings();
	if (settings)
	{
		pos = m_account->settings()->value("ContactListWindow/pos", pos).toPoint();
		size = m_account->settings()->value("ContactListWindow/size", size).toSize();
	}

	resize(size);
	move(pos);
}

void ContactListWindow::closeEvent(QCloseEvent* /*event*/)
{
	qDebug() << "ContactListWindow::closeEvent";
	m_pos = pos();
	m_size = size();
	writeSettings();
}

void ContactListWindow::slotLoginRejected(QString reason)
{
	CenteredMessageBox::warning(tr("Login rejected"), reason);

	LoginDialog ld;
	ld.setEmail(m_account->email());

	if (ld.exec() == QDialog::Rejected)
		qApp->quit();

	m_account->reset(ld.email(), ld.password());
	m_account->setOnlineStatus(ld.status());
}

void ContactListWindow::slotLoggedOut(quint32 reason)
{
	QString reasonText;
	if (reason == LOGOUT_NO_RELOGIN_FLAG)
		reasonText = tr("Someone else entered with your login");

	CenteredMessageBox::warning(tr("Logout"), reasonText);
}

void ContactListWindow::extHide()
{
	m_pos = pos();
	m_size = size();
	hide();
}

void ContactListWindow::extShow()
{
	resize(m_size);
	move(m_pos);
	show();
}

void ContactListWindow::slotContactAsksAuthorization(const QByteArray & email, const QString & nickname, const QString & message)
{
	AuthorizeDialog* dlg = new AuthorizeDialog(m_account, email, nickname, message);
	connect(dlg, SIGNAL(accepted()), this, SLOT(authorizeContact()));

	theRM.getAudio()->play(STAuth);

	dlg->show();
}

void ContactListWindow::authorizeContact()
{
	AuthorizeDialog* dlg = qobject_cast<AuthorizeDialog*>(sender());

	qDebug() << "user wanna authorize contact " << dlg->email();
	MRIMClient* mc = m_account->client();

	mc->authorizeContact(dlg->email());

	if (dlg->addContact())
	{
		ContactList* cl = m_account->contactList();
		Contact* contact = cl->findContact(dlg->email());
		if (!contact || !contact->isTemporary())
		{
			qDebug() << "ContactListWindow::authorizeContact: adding contact " << dlg->email() << " error";
			return;
		}
		cl->addTemporaryContactToGroup(contact, dlg->group());
	}
}

void ContactListWindow::openMail()
{
	openMailRuUrl("http://edu.mail.ru/cgi-bin/start");
}

QIcon ContactListWindow::projectIcon(const QString & project) const
{
	QIcon icon;
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + project + "_d.png"), QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + project + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + project + "_p.png"), QIcon::Active, QIcon::On);

	return icon;
}

void ContactListWindow::createActions()
{
	QString email = m_account->email();
	QString username = email.section('@', 0, 0);
	QString domain = email.section('@', 1, 1).section('.', 0, 0);

	m_myWorldAction = new QAction(projectIcon("my"), tr("My world"), this);
	m_myWorldAction->setData("http://r.mail.ru/cln3587/my.mail.ru/" + domain + "/" + username);
	connect(m_myWorldAction, SIGNAL(triggered(bool)), this, SLOT(openMailRuProject()));

	m_myBlogAction = new QAction(projectIcon("blogs"), tr("My blog"), this);
	m_myBlogAction->setData("http://r.mail.ru/cln3566/blogs.mail.ru/" + domain + "/" + username);
	connect(m_myBlogAction, SIGNAL(triggered(bool)), this, SLOT(openMailRuProject()));

	m_myPhotosAction = new QAction(projectIcon("foto"), tr("My photos"), this);
	m_myPhotosAction->setData("http://r.mail.ru/cln3565/foto.mail.ru/" + domain + "/" + username);
	connect(m_myPhotosAction, SIGNAL(triggered(bool)), this, SLOT(openMailRuProject()));

	m_myVideosAction = new QAction(projectIcon("video"), tr("My videos"), this);
	m_myVideosAction->setData("http://r.mail.ru/cln3567/video.mail.ru/" + domain + "/" + username);
	connect(m_myVideosAction, SIGNAL(triggered(bool)), this, SLOT(openMailRuProject()));
}

void ContactListWindow::openMailRuProject()
{
	QAction* action = qobject_cast<QAction*>(sender());
	openMailRuUrl(action->data().toString());
}

void ContactListWindow::openMailRuUrl(const QString & url)
{
	Task* task = new Tasks::GetMPOPSession(url, m_account->client(), this);
	connect(task, SIGNAL(done(quint32, bool)), SLOT(openMailRuUrlEnd(quint32, bool)));

	task->exec();
}

void ContactListWindow::openMailRuUrlEnd(quint32 status, bool timeout)
{
	if (timeout == true || status != MRIM_GET_SESSION_SUCCESS)
	{
		qDebug() << "MRIMClientPrivate: can't get mpop session";
		return;
	}

	Tasks::GetMPOPSession* task = qobject_cast<Tasks::GetMPOPSession*>(sender());
	if (!task)
	{
		qDebug() << "openMailSession strange sender";
		return;
	}

	qDebug() << Q_FUNC_INFO << "session id =" << task->session();

	QString url = "http://swa.mail.ru/cgi-bin/auth?Login=" + m_account->email() + "&agent=" + task->session() + "&page=" + task->url();
	QDesktopServices::openUrl(QUrl(url));
}

void ContactListWindow::checkAutoAwayTime(int seconds)
{
	if (m_account->settings()->value("Common/autoAwayEnabled", false).toBool())
	{
		int awaySeconds = 60*m_account->settings()->value("Common/autoAwayMinutes", 10).toUInt();

		if (seconds >= awaySeconds)
			m_account->setAutoAway(true);
	}

	if (seconds == 0)
		m_account->setAutoAway(false);
}

void ContactListWindow::openStatusEditor()
{
	qDebug() << "ContactListWindow::openStatusEditor()";

	statusEditor->show();
	statusEditor->setGeometry(this->geometry().x(), statusBar->geometry().y() + this->geometry().y() - statusEditor->geometry().height() + statusBar->geometry().height(), this->geometry().width(), NULL);
}

void ContactListWindow::sendMicrotext(const QString& microText)
{
	qDebug() << "ContactListWindow::sendMicrotext(" << microText << ")";
	m_account->client()->sendMicrotext(microText);
	statusBar->setStatus(microText);
}

void ContactListWindow::slotMicroblogChanged(QString microText)
{
	qDebug() << "slotMicroblogChanged(" << microText << ")";
	statusEditor->setStatus(microText);
	statusBar->setStatus(microText);
}

void ContactListWindow::slotSetOnlineStatus(OnlineStatus status)
{
	qDebug() << "ContactListWindow::slotSetOnlineStatus" << status.id() << status.statusDescr();
	statusButton->setStatus(status);
	if (status == OnlineStatus::offline || status == OnlineStatus::connecting || status == OnlineStatus::unknown)
	{
		statusBar->setActive(false);
		statusEditor->close();
	}
	else
		statusBar->setActive(true);
}

void ContactListWindow::visibleWidget(Widgets w, bool st)
{
	switch (w)
	{
		case MicroBlog:
			statusBar->setVisible(st);
			return;
	}
}

/*void ContactListWindow::slotConferenceAck(const QByteArray& confChat, const QString& confName)
{
	qDebug() << "ContactListWindow::slotConferenceAck" << confChat << confName;

	m_account->contactList()->addContact(ContactData(0, 0, 0, confChat, confName));
}*/
