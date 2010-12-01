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

#ifndef HISTORYVIEWTAB_H
#define HISTORYVIEWTAB_H

#include <QWidget>
#include <QDate>
#include <QPushButton>
#include <QProgressBar>
#include <xapian.h>

class AnimatedTextBrowser;
class QCalendarWidget;
class QListWidget;

class HistoryViewTab : public QWidget
{
Q_OBJECT
public:
	HistoryViewTab(Xapian::Database* db, const QString& incomingName, const QString& outgoingName, QWidget* parent = 0);
	~HistoryViewTab();
private slots:
	void updateDates(int year, int month);
	void newDateSelectedInList();
	void newDateSelectedInCalendar();
	void slotSaveHistoryLog();
private:
	QList<QDate> monthChatDays(int year, int month);
	void showMessage(const Xapian::Document& doc);
	void showMessagesForDate(const QDate& date);

	Xapian::Database* database;
	QString incomingNickname;
	QString outgoingNickname;

	QCalendarWidget* calendarWidget;
	QListWidget* datesListWidget;
	AnimatedTextBrowser* viewBrowser;
	QPushButton* saveButton;
	QProgressBar* progressBar;
};

#endif
