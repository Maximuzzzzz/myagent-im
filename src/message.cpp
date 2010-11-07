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

#include "message.h"

#include <QTextDocument>

#include <QDebug>

#include "rtfparser.h"
#include "plaintextparser.h"
#include "proto.h"

Message::Message(Type type, quint32 flags, QString plainText, QByteArray rtfText, quint32 backgroundColor, QDateTime dateTime)
	: m_type(type), m_dateTime(dateTime), m_flags(flags), m_plainText(plainText), m_rtfText(rtfText), m_backgroundColor(backgroundColor)
{
/*	m_type = type;
	m_flags = flags;
	m_plainText = plainText;
	m_rtfText = rtfText;
	m_backgroundColor = backgroundColor;
	m_dateTime = dateTime;*/
}

Message::~Message()
{
}

void Message::setId(quint32 id)
{
	m_id = id;
}

QTextDocumentFragment Message::documentFragment(int defR, int defG, int defB, int defSize, QString fontFamily) const
{
	QTextDocument doc;
	
	if (m_flags & MESSAGE_FLAG_ALARM)
	{
		QTextCursor cursor(&doc);
		QString html = "<font color=green>";
		
		if (m_type == Error)
			html.append(m_plainText);
		else if (m_type == Incoming)
			html.append(tr("Interlocutor has tried to awake you"));
		else
			html.append(tr("You has tried to awake interlocutor"));
		
		cursor.insertHtml(html + "</font><br>");
	}
	else if (m_flags & MESSAGE_FLAG_RTF)
	{
		RtfParser rtfParser;
		if (defR > -1)
			rtfParser.parse(m_rtfText, &doc, defR, defG, defB, defSize, fontFamily);
		else
			rtfParser.parse(m_rtfText, &doc); /*TODO: add font family and all values will be taken from chatwindow*/
	}
	else
	{
		PlainTextParser parser;
		parser.parse(m_plainText, &doc);
		QTextCursor cursor(&doc);
		cursor.movePosition(QTextCursor::End);
		cursor.insertBlock();
	}

	return QTextDocumentFragment(&doc);
}

QTextDocumentFragment Message::documentFragment(QFont defFont, QColor defFontColor, QColor defBkColor) const
{
	return documentFragment(defFontColor.red(), defFontColor.green(), defFontColor.blue(), defFont.pointSize(), defFont.family());
}
