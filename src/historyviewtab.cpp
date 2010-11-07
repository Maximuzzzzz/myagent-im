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

#include "historyviewtab.h"

#include <QCalendarWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include "animatedtextbrowser.h"
#include "message.h"
#include "historylogger.h"

HistoryViewTab::HistoryViewTab(Xapian::Database* db, const QString& incomingName, const QString& outgoingName, QWidget *parent)
	: QWidget(parent), database(db), incomingNickname(incomingName), outgoingNickname(outgoingName)
{
	QHBoxLayout* layout = new QHBoxLayout;

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
//	dateLayout->addWidget(saveButton);
	dateLayout->setSizeConstraint(QLayout::SetFixedSize);

	viewBrowser = new AnimatedTextBrowser;

	layout->addLayout(dateLayout);
	layout->addWidget(viewBrowser);

	setLayout(layout);

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
	Message* msg = HistoryLogger::createMessage(doc);
	
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

	delete msg;
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
	{
		showMessage(i.get_document());
	}
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

	Xapian::Enquire enquire(*database);
/*	Xapian::Query query();
	enquire.set_query(query);
	quint32 j, k;
	Xapian::MSet matches;
	Xapian::MSetIterator i;
	k = 0;
	while (j >= 100000 - 1)
	{
		matches = enquire.get_mset(0 + k * 100000, 100000 + k * 100000 - 1);
		j = 0;
		for (i = matches.begin(); i != matches.end(); ++i)
		{
			Message* msg = HistoryLogger::createMessage(doc);
			//TODO: Save to file
			j++;
		}
		k++;
	}*/
}
