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

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <QLocale>

#include "accountmanager.h"
#include "contactlist.h"
#include "logindialog.h"
#include "contactlistwindow.h"

#include "audio.h"
#include "resourcemanager.h"

#ifdef Q_WS_WIN
#include <QtPlugin>

Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);
	app.setApplicationName("MyMailAgentRu");

#ifdef DATADIR
	qDebug() << "datadir = " << QLatin1String(DATADIR);
#endif

	QTranslator qtTranslator;
#ifdef DATADIR
	qtTranslator.load("qt_" + QLocale::system().name(), QLatin1String(DATADIR));
#else
        qtTranslator.load("locale/qt_" + QLocale::system().name());
#endif
	app.installTranslator(&qtTranslator);
	
	QTranslator myappTranslator;
#ifdef DATADIR
        myappTranslator.load("myagent-im_" + QLocale::system().name(), QLatin1String(DATADIR));
#else
	myappTranslator.load("locale/myagent-im_" + QLocale::system().name());
#endif
	app.installTranslator(&myappTranslator);

	Audio* audio = new Audio();
	theRM.setAudio(audio);

	LoginDialog* ld = new LoginDialog;
	
	if (ld->exec() == QDialog::Rejected)
		return 0;

	AccountManager am;
	Account* account = am.getAccount(ld->email(), ld->password());
	account->saveOnlineStatus(ld->status());
	audio->setAccount(account);
	ContactListWindow clw(account);
	clw.show();
	
	account->setOnlineStatus(ld->status());

	delete ld;
	
	return app.exec();
}
