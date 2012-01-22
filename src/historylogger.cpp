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

#include "historylogger.h"

#include <QDebug>

#include <xapian/database.h>
#include <xapian/termgenerator.h>
#include <xapian/stem.h>

#include "chatsession.h"
#include "contact.h"
#include "message.h"
#include "protocol/mrim/proto.h"

#include <iostream>

HistoryLogger::HistoryLogger(ChatSession* s)
	: QObject(s), session(s)
{
	QString path = session->contact()->path() + "/history";
	database = new Xapian::WritableDatabase(path.toLocal8Bit().constData(), Xapian::DB_CREATE_OR_OPEN);
	if (database->get_doccount() > 0)
	{
		Xapian::docid lastid = database->get_lastdocid();
		Xapian::docid N = lastid < 5? lastid : 5;
		Xapian::docid docid = lastid - N + 1;

		while (docid <= lastid)
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
			Message* message = createMessage(doc);
			session->appendMessage(message);

			docid++;
		}
	}

	connect(session, SIGNAL(messageAppended(const Message*)), this, SLOT(saveMessage(const Message*)));
}

HistoryLogger::~HistoryLogger()
{
	qDebug() << "HistoryLogger::~HistoryLogger()";
	try
	{
		database->flush();
	}
	catch(...)
	{
		qDebug() << "can't flush database";
	}

	delete database;
}

void HistoryLogger::saveMessage(const Message* message)
{
	if (message->flags() & MESSAGE_FLAG_ALARM)
		return;
	
	Xapian::Document doc;
	
	quint32 flags = message->flags();
	std::string plainText(message->plainText().toUtf8());
	std::string confUser(message->getConfUser().constData());

	std::string data;
	if (flags & MESSAGE_FLAG_RTF)
		data = message->rtfText().constData();
	else
		data = plainText;

	std::cout << "HistoryLogger::saveMessage data = " << data << std::endl;
	doc.set_data(data);

	Xapian::TermGenerator termGen;
	termGen.set_stemmer(Xapian::Stem("ru"));
	termGen.set_document(doc);
	termGen.index_text(plainText);

	doc.add_value(0, message->dateTime().toString("yyyyMMdd").toStdString());
	doc.add_value(1, message->dateTime().toString("hhmmss").toStdString());
	doc.add_value(2, QString::number(flags, 16).toStdString());
	doc.add_value(3, message->type() == Message::Outgoing? "o" : "i");
	doc.add_value(4, confUser);

	database->add_document(doc);
	database->flush();
}

Message* HistoryLogger::createMessage(const Xapian::Document & doc)
{
	QDateTime dateTime = QDateTime::fromString(QString::fromStdString(doc.get_value(0)+doc.get_value(1)), "yyyyMMddhhmmss");
	quint32 flags = QString::fromStdString(doc.get_value(2)).toUInt(0, 16);
	Message::Type type = doc.get_value(3) == "o"? Message::Outgoing : Message::Incoming;
	QByteArray confUser = QByteArray(doc.get_value(4).c_str());
	std::string message(doc.get_data());
	//std::cout << "HistoryLogger::createMessage data = " << message << std::endl;

	Message* msg;
	if (flags & MESSAGE_FLAG_RTF)
		msg = new Message(type, flags, " ", QByteArray(message.c_str()), 0x00FFFFFF, confUser, dateTime);
	else
	{
		QString text = QString::fromUtf8(message.c_str(), message.size());
		msg = new Message(type, flags, text, " ", 0x00FFFFFF, confUser, dateTime);
	}
	return msg;
}
