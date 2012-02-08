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

#include "historysearchtab.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStyle>
#include <QAction>
#include <QMenu>

#include <xapian/stem.h>
#include <xapian/enquire.h>
#include <xapian/document.h>

#include "animatedtextbrowser.h"
#include "core/message.h"
#include "core/historylogger.h"

HistorySearchTab::HistorySearchTab(Xapian::Database* db, const QString & incomingName, const QString & outgoingName, QWidget * parent)
	: QWidget(parent), database(db), incomingNickname(incomingName), outgoingNickname(outgoingName), date_proc(0)
{
	QVBoxLayout* layout = new QVBoxLayout;

	QHBoxLayout* queryLayout = new QHBoxLayout;

	lastQueryAction = 0;

	QToolButton* backButton = new QToolButton;
	backButton->setAutoRaise(true);
	backButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	backButton->setDisabled(true);
	connect(backButton, SIGNAL(triggered(QAction*)), this, SLOT(slotBackButtonTriggered(QAction*)));
	connect(backButton, SIGNAL(clicked(bool)), this, SLOT(slotBackButtonClicked()));
	backHistoryMenu = new QMenu(backButton);
	backButton->setMenu(backHistoryMenu);
	
	QToolButton* forwardButton = new QToolButton;
	forwardButton->setAutoRaise(true);
	forwardButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
	forwardButton->setDisabled(true);
	connect(forwardButton, SIGNAL(triggered(QAction*)), this, SLOT(slotForwardButtonTriggered(QAction*)));
	connect(forwardButton, SIGNAL(clicked(bool)), this, SLOT(slotForwardButtonClicked()));
	forwardHistoryMenu = new QMenu(forwardButton);
	forwardButton->setMenu(forwardHistoryMenu);

	queryEditor = new QLineEdit;
	connect(queryEditor, SIGNAL(returnPressed()), this, SLOT(slotProcessQuery()));

	startSearchButton = new QPushButton(tr("Search"));
	connect(startSearchButton, SIGNAL(clicked(bool)), this, SLOT(slotProcessQuery()));

	queryLayout->addWidget(backButton);
	queryLayout->addWidget(forwardButton);
	queryLayout->addWidget(queryEditor);
	queryLayout->addWidget(startSearchButton);

	resultsBrowser = new AnimatedTextBrowser;
	connect(resultsBrowser, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(checkUrl(const QUrl&)));

	layout->addLayout(queryLayout);
	layout->addWidget(resultsBrowser);
	
	setLayout(layout);

	if (database)
	{
		parser.set_stemmer(Xapian::Stem("ru"));
		parser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
		parser.set_default_op(Xapian::Query::OP_PHRASE);
		parser.set_database(*database);
		date_proc = new Xapian::DateValueRangeProcessor(0);
		parser.add_valuerangeprocessor(date_proc);
	}
}

HistorySearchTab::~HistorySearchTab()
{
	if (date_proc)
	{
		delete date_proc;
		date_proc = 0;
	}

	if (lastQueryAction)
	{
		delete lastQueryAction;
		lastQueryAction = 0;
	}
}

void HistorySearchTab::showMessage(const Xapian::Document & doc)
{
	QScopedPointer<Message> msg(HistoryLogger::createMessage(doc));
	
	QString nick;
	if (msg->type() == Message::Outgoing)
		nick = outgoingNickname;
	else
		nick = incomingNickname;
	
	QString prompt = "<a href=" + msg->dateTime().toString("dd/MM/yyyy") + '>' +  msg->dateTime().toString("yyyy-MM-dd") + "</a> ";
	prompt += msg->dateTime().toString("hh:mm:ss") + " <b>" + nick + "</b>: ";
	QTextCursor cursor = resultsBrowser->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertHtml(prompt);
	cursor.insertFragment(msg->documentFragment());
}

void HistorySearchTab::startSearch()
{
	Xapian::Enquire enquire(*database);
	std::string queryStr(queryEditor->text().toUtf8());
	Xapian::Query query = parser.parse_query(queryStr);
	
	enquire.set_query(query);
	Xapian::MSet matches = enquire.get_mset(0, 100000);

	resultsBrowser->clear();

	if (matches.size() == 0)
	{
		resultsBrowser->append("<font color=red>" + tr("*** Nothing was found ***") + "</font>");
	}

	Xapian::MSetIterator i;
	for (i = matches.begin(); i != matches.end(); ++i)
	{
		showMessage(i.get_document());
	}
}

void HistorySearchTab::checkUrl(const QUrl& url)
{
	QRegExp rx("\\d\\d/\\d\\d/\\d\\d\\d\\d");
	QString date = url.toString();
	if (rx.exactMatch(date))
	{
		queryEditor->setText(date + ".." + date);
		slotProcessQuery();
	}
}

void HistorySearchTab::searchLastQuery()
{
	queryEditor->setText(lastQueryAction->text());
	startSearch();
}

void HistorySearchTab::slotProcessQuery()
{
	QString query = queryEditor->text();
	
	if (lastQueryAction && query == lastQueryAction->text())
		return;

	if (query.isEmpty())
	{
		resultsBrowser->clear();
		return;
	}
	
	forwardHistoryMenu->clear();
	forwardHistoryMenu->parentWidget()->setDisabled(true);

	if (lastQueryAction)
		backHistoryMenu->insertAction(backHistoryMenu->actions().value(0), lastQueryAction);
	lastQueryAction = new QAction(query, this);

	backHistoryMenu->parentWidget()->setDisabled(backHistoryMenu->isEmpty());

	searchLastQuery();
}

void HistorySearchTab::slotBackButtonTriggered(QAction* queryAction)
{
	swapQueryActions(backHistoryMenu, queryAction, forwardHistoryMenu);

	searchLastQuery();
}

void HistorySearchTab::slotBackButtonClicked()
{
	slotBackButtonTriggered(backHistoryMenu->actions().first());
}

void HistorySearchTab::slotForwardButtonTriggered(QAction* queryAction)
{
	swapQueryActions(forwardHistoryMenu, queryAction, backHistoryMenu);

	searchLastQuery();
}

void HistorySearchTab::slotForwardButtonClicked()
{
	slotForwardButtonTriggered(forwardHistoryMenu->actions().first());
}

void HistorySearchTab::swapQueryActions(QMenu* fromMenu, QAction* queryAction, QMenu* toMenu)
{
	QAction* toFirstAction = toMenu->actions().value(0);
	toMenu->insertAction(toFirstAction, lastQueryAction);
	toFirstAction = lastQueryAction;

	QList<QAction*> actions = fromMenu->actions();
	
	int i = 0;
	while (actions.at(i) != queryAction)
	{
		toMenu->insertAction(toFirstAction, actions.at(i));
		toFirstAction = actions.at(i);
		fromMenu->removeAction(toFirstAction);
		i++;
	}
	
	lastQueryAction = queryAction;
	fromMenu->removeAction(lastQueryAction);

	fromMenu->parentWidget()->setDisabled(fromMenu->isEmpty());
	toMenu->parentWidget()->setDisabled(toMenu->isEmpty());
}
