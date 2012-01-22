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

#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include <QLocale>

#include "accountmanager.h"
#include "gui/logindialog.h"
#include "gui/contactlistwindow.h"

#include "audio.h"
#include "resourcemanager.h"
#include "account.h"

#ifdef Q_WS_WIN
#include <QtPlugin>

Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	qDebug() << Q_FUNC_INFO;
	app.setQuitOnLastWindowClosed(false);
	app.setApplicationName("MyMailAgentRu");

#ifdef DATADIR
	qDebug() << "datadir = " << QLatin1String(DATADIR);
#endif

	QTranslator myappTranslator;
#ifdef DATADIR
	myappTranslator.load("myagent-im_" + QLocale::system().name(), QLatin1String(DATADIR));
#else
	myappTranslator.load("locale/myagent-im_" + QLocale::system().name());
#endif
	app.installTranslator(&myappTranslator);

	Audio* audio = new Audio();
	theRM.setAudio(audio);

	QScopedPointer<LoginDialog> ld(new LoginDialog);

	if (ld->exec() == QDialog::Rejected)
		return 0;

	AccountManager am;
	Account* account = am.getAccount(ld->email(), ld->password());
	theRM.setAccount(account);
	qDebug() << Q_FUNC_INFO << "status id from login dialog:" << ld->status().id();
	account->saveOnlineStatus(ld->status());
	account->savePassword(ld->savePassword());
	audio->setAccount(account);
	ContactListWindow clw(account);
	clw.show();

	account->setOnlineStatus(ld->status());

	ld.reset();

	return app.exec();
}
