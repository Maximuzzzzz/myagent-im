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

#include "historyviewtab.h"

#include <QCalendarWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextCodec>
#include <QPushButton>
#include <QProgressBar>

#include <xapian/matchspy.h>
#include <xapian/document.h>
#include <xapian/query.h>
#include <xapian/database.h>

#include "gui/animatedtextbrowser.h"
#include "core/message.h"
#include "core/historylogger.h"
#include "rtfparser.h"

HistoryViewTab::HistoryViewTab(Xapian::Database* db, const QString& incomingName, const QString& outgoingName, QWidget *parent)
	: QWidget(parent), database(db), incomingNickname(incomingName), outgoingNickname(outgoingName)
{
	QVBoxLayout* layout = new QVBoxLayout;

	QHBoxLayout* layoutMain = new QHBoxLayout;

	QVBoxLayout* dateLayout = new QVBoxLayout;

	calendarWidget = new QCalendarWidget;
	calendarWidget->setFixedSize(calendarWidget->sizeHint());
	calendarWidget->setFirstDayOfWeek(Qt::Monday);
	calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	connect(calendarWidget, SIGNAL(currentPageChanged(int,int)), this, SLOT(updateDates(int,int)));
	connect(calendarWidget, SIGNAL(selectionChanged()), this, SLOT(newDateSelectedInCalendar()));
	
	datesListWidget = new QListWidget;
	datesListWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
	connect(datesListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(newDateSelectedInList()));

	saveButton = new QPushButton(tr("Save all log..."));
	connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(slotSaveHistoryLog()));

	dateLayout->addWidget(calendarWidget);
	dateLayout->addWidget(datesListWidget);
	dateLayout->addWidget(saveButton);
	dateLayout->setSizeConstraint(QLayout::SetFixedSize);

	viewBrowser = new AnimatedTextBrowser;

	layoutMain->addLayout(dateLayout);
	layoutMain->addWidget(viewBrowser);

	progressBar = new QProgressBar;

	layout->addLayout(layoutMain);
	layout->addWidget(progressBar);

	setLayout(layout);

	progressBar->setVisible(false);

	updateDates(calendarWidget->yearShown(), calendarWidget->monthShown());
	newDateSelectedInCalendar();
}

HistoryViewTab::~HistoryViewTab()
{
}

void HistoryViewTab::updateDates(int year, int month)
{
	datesListWidget->clear();
	QList<QDate> chatDates = monthChatDays(year, month);
	QStringList dateStrings;
	QTextCharFormat fmt;
	fmt.setBackground(Qt::yellow);
	for (QList<QDate>::const_iterator it = chatDates.begin(); it != chatDates.end(); ++it)
	{
		datesListWidget->addItem(it->toString("yyyy-MM-dd"));
		calendarWidget->setDateTextFormat(*it, fmt);
	}
}

QList<QDate> HistoryViewTab::monthChatDays(int year, int month)
{
	if (!database)
		return QList<QDate>();
	
	class DateSpy : public Xapian::MatchDecider
	{
	public:
		virtual bool operator() (const Xapian::Document &doc) const
		{
			QString date = QString::fromStdString(doc.get_value(0));
			dates.insert(date);
			return true;
		}
		
		mutable QSet<QString> dates;
	};
	
	Xapian::Enquire enquire(*database);
	QDate startDate = QDate(year, month, 1);
	QDate finishDate = QDate(year, month, startDate.daysInMonth());
	const char* dateFormat = "yyyyMMdd";
	Xapian::Query query(Xapian::Query::OP_VALUE_RANGE, 0, startDate.toString(dateFormat).toStdString(), finishDate.toString(dateFormat).toStdString());
	enquire.set_query(query);
	DateSpy* spy = new DateSpy;
	enquire.get_mset(0, 1000000, 0, 0, spy);
	QStringList datesList = spy->dates.toList();
	delete spy;
	datesList.sort();
	
	QList<QDate> result;
	for (QStringList::const_iterator it = datesList.begin(); it != datesList.end(); ++it)
	{
		result.append(QDate::fromString(*it, dateFormat));
	}
	return result;
}

void HistoryViewTab::newDateSelectedInList()
{
	qDebug() << "HistoryViewTab::newDateSelectedInList";
	QList<QListWidgetItem*> selection = datesListWidget->selectedItems();
	if (selection.isEmpty())
		return;
	QString dateString = selection.at(0)->text();
	QDate date = QDate::fromString(dateString, "yyyy-MM-dd");
	calendarWidget->setSelectedDate(date);
	showMessagesForDate(date);
}

void HistoryViewTab::newDateSelectedInCalendar()
{
	qDebug() << "HistoryViewTab::newDateSelectedInCalendar";
	viewBrowser->clear();
	QDate date = calendarWidget->selectedDate();
	QList<QListWidgetItem*> dateItems = datesListWidget->findItems(date.toString("yyyy-MM-dd"), Qt::MatchFixedString);
	if (dateItems.size() == 1)
	{
		datesListWidget->setCurrentItem(dateItems.at(0));
		return;
	}
	datesListWidget->setCurrentItem(datesListWidget->currentItem(), QItemSelectionModel::Clear);
}

void HistoryViewTab::showMessage(const Xapian::Document & doc)
{
	QScopedPointer<Message> msg(HistoryLogger::createMessage(doc));
	
	QString nick;
	if (msg->type() == Message::Outgoing)
		nick = outgoingNickname;
	else
		nick = incomingNickname;
	
	QString prompt = msg->dateTime().toString("hh:mm:ss") + " <b>" + nick + "</b>: ";
	QTextCursor cursor = viewBrowser->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertHtml(prompt);
	cursor.insertFragment(msg->documentFragment());
}

void HistoryViewTab::showMessagesForDate(const QDate & date)
{
	qDebug() << "HistoryViewTab::showMessagesForDate";
	Xapian::Enquire enquire(*database);
	std::string dateString = date.toString("yyyyMMdd").toStdString();
	Xapian::Query query(Xapian::Query::OP_VALUE_RANGE, 0, dateString, dateString);
	enquire.set_query(query);
	Xapian::MSet matches = enquire.get_mset(0, 100000);
	
	viewBrowser->clear();
	for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i)
		showMessage(i.get_document());
}

void HistoryViewTab::slotSaveHistoryLog()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("File to save"), QString(), tr("HTML file %1").arg("(*.html)"));
	QFile fileToSave(fileName);

	if (!fileToSave.open(QIODevice::WriteOnly))
	{
		//TODO: message about error
		return;
	}

	//QDataStream out(&fileToSave);
	fileToSave.write("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head>\n<meta content=\"text/html; charset=UTF-8\" http-equiv=\"Content-Type\">\n<title>@MAIL.RU</title>\n<style>\n.headerin {font-weight: bold; FONT-SIZE: 12px; COLOR: red; FONT-FAMILY: Tahoma, Verdana, Arial, Sans Serif}\n.headerout {font-weight: bold; FONT-SIZE: 12px; COLOR: blue; FONT-FAMILY: Tahoma, Verdana, Arial, Sans Serif}\n.msg {FONT-SIZE: 12px; COLOR: black; FONT-FAMILY: Tahoma, Verdana, Arial, Sans Serif}\n</style>\n</head>\n<body scroll=auto style=\"background-color: rgb(255, 255, 255);\"leftmargin=4 topmargin=4 marginwidth=4 marginheight=4link=\"#000099\" vlink=\"#000099\" alink=\"#cc0000\">\n<table>\n");

	Xapian::docid docid;
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
//	QTextCodec* codecUTF16 = QTextCodec::codecForName("utf-16le");
	progressBar->setMaximum(0x7fffffff);
	progressBar->setVisible(true);
	for (docid = 0; docid <= database->get_lastdocid(); docid++)
	{
		Xapian::Document doc;
		try
		{
			doc = database->get_document(docid);
		}
		catch(...)
		{
			qDebug() << "document with docid = " << docid << " is missing in database";
			continue;
		}
		fileToSave.write("<TR>\n<TD>\n<FONT class=\"header");
		if (doc.get_value(3) == "o")
			fileToSave.write("out\">" + codec->fromUnicode(outgoingNickname) + " ("); //TODO: must be nickname, not email
		else
			fileToSave.write("in\">" + codec->fromUnicode(incomingNickname) + " (");
		fileToSave.write(codec->fromUnicode(QString::fromStdString(doc.get_value(0)+doc.get_value(1))) + "):</FONT><BR/>\n"); //make readable date+time
		std::string message(doc.get_data()); //parse rtf and take only text
		RtfParser rtfParser;
		QString mess;
		rtfParser.parseToHTML(message.c_str(), mess);
		fileToSave.write(codec->fromUnicode(mess) + "\n</TD>\n</TR>\n");
		progressBar->setValue(double(docid / database->get_lastdocid()) * 0x7fffffff);
	}
	progressBar->setValue(0);
	progressBar->setVisible(false);

	fileToSave.write("</table>\n</body>\n</html>");
}
