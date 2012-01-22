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

#include "historyviewer.h"

#include <QDebug>

#include <xapian/database.h>

#include "historyviewtab.h"
#include "historysearchtab.h"
#include "contact.h"
#include "account.h"

HistoryViewer::HistoryViewer(Contact* contact)
	: QTabWidget(0)
{
	qDebug() << "HistoryViewer::HistoryViewer";
	
	setWindowTitle(tr("History") + " - " + contact->email());
	setWindowIcon(QIcon(":icons/history.png"));
	
	incomingNickname = contact->nickname();
	qDebug() << "incomingNickname = " << incomingNickname;
	outgoingNickname = contact->account()->nickname();
	qDebug() << "outgoingNickname = " << outgoingNickname;

	QString path = contact->path() + "/history";
	qDebug() << "HistoryViewer::HistoryViewer db path = " << path;

	database = 0;
	
	try
	{
		database = new Xapian::Database(path.toLocal8Bit().constData());
	}
	catch(...)
	{
		qDebug() << "can't open database";
	}

	viewPage = new HistoryViewTab(database, incomingNickname, outgoingNickname);
	searchPage = new HistorySearchTab(database, incomingNickname, outgoingNickname);

	addTab(viewPage, tr("View"));
	addTab(searchPage, tr("Search"));
	
	resize(800, 600);

	if (database == 0) setDisabled(true);
}

HistoryViewer::~HistoryViewer()
{
	qDebug() << "HistoryViewer::~HistoryViewer()";

	if (database)
	{
		delete database;
		database = 0;
	}
}
