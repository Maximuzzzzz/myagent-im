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

#ifndef HISTORYSEARCHTAB_H
#define HISTORYSEARCHTAB_H

#include <QWidget>
#include <QUrl>

#include <xapian/queryparser.h>

namespace Xapian
{
class Database;
class Document;
}

class QMenu;
class QLineEdit;
class QPushButton;
class AnimatedTextBrowser;

class HistorySearchTab : public QWidget
{
Q_OBJECT
public:
	HistorySearchTab(Xapian::Database* db, const QString& incomingName, const QString& outgoingName, QWidget *parent = 0);
	~HistorySearchTab();
	
private Q_SLOTS:
	void checkUrl(const QUrl&);

	void slotProcessQuery();
	void slotBackButtonTriggered(QAction* queryAction);
	void slotBackButtonClicked();
	void slotForwardButtonTriggered(QAction* queryAction);
	void slotForwardButtonClicked();

private:
	void startSearch();
	void showMessage(const Xapian::Document& doc);
	void swapQueryActions(QMenu* fromMenu, QAction* queryAction, QMenu* toMenu);
	void searchLastQuery();

private:
	QLineEdit* queryEditor;
	QPushButton* startSearchButton;
	AnimatedTextBrowser* resultsBrowser;

	Xapian::Database* database;
	QString incomingNickname;
	QString outgoingNickname;

	Xapian::QueryParser parser;
	Xapian::DateValueRangeProcessor* date_proc;

	QMenu* backHistoryMenu;
	QMenu* forwardHistoryMenu;
	QAction* lastQueryAction;
};

#endif
