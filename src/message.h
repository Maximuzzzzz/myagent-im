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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDateTime>
#include <QTextDocumentFragment>
#include <QCoreApplication>

#include <QFont>
#include <QColor>

class Message
{
Q_DECLARE_TR_FUNCTIONS(Message)
public:
	enum Type { Incoming, Outgoing, Error };
	
	Message(Type type, quint32 flags, QString plainText, QByteArray rtfText, quint32 backgroundColor, QByteArray confUser = "", QDateTime dateTime = QDateTime::currentDateTime());
	
	~Message();
	
	quint32 flags() const { return m_flags; }
	const QString plainText() const { return m_plainText; }
	const QByteArray rtfText() const { return m_rtfText; }
	const QDateTime dateTime() const { return m_dateTime; }
	Type type() const { return m_type; }
	quint32 getId() const { return m_id; }
	QByteArray getConfUser() const { return m_confUser; }
	bool isConfMessage() const { return m_confUser != ""; }

	void setId(quint32 id);

	QTextDocumentFragment documentFragment(int defR = -1, int defG = -1, int defB = -1, int defSize = -1, QString fontFamily = "") const;
	QTextDocumentFragment documentFragment(QFont defFont, QColor defFontColor, QColor defBkColor) const;

private:
	Type m_type;
	QDateTime m_dateTime;
	quint32 m_flags;
	QString m_plainText;
	QByteArray m_rtfText; // phone number for sms
	quint32 m_backgroundColor;
	quint32 m_id;
	QByteArray m_confUser;

	bool m_ignoreStyle;
};

#endif
