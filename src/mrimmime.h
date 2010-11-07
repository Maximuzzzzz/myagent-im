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

#ifndef MRIMMIME_H
#define MRIMMIME_H

#include <QObject>
#include <QHash>

#include "datetime.h"

class MrimMIME : public QObject
{
public:
	MrimMIME();
	MrimMIME(const QByteArray & data);

	QByteArray from() { return m_from; }
	QByteArray sender() { return m_sender; }
	QString subject() { return m_subject; }
	QDateTime dateTime() { return m_dateTime; }
	QByteArray mimeVersion() { return m_mimeVersion; }
	QByteArray plainText() { return m_plainText; }
	QByteArray plainTextCharset() { return m_plainTextCharset; }
	QByteArray rtfBase64() { return m_rtfBase64; }
	QByteArray xMrimVersion() { return m_xMrimVersion; }
	quint32 xMrimFlags() { return m_xMrimFlags; }
	quint32 xMrimMultichatType() { return m_xMrimMultichatType; }
	bool hasPlainText() { return m_hasPlainText; }
	bool hasRtfText() { return m_hasRtfText; }
	bool isMultipart() { return m_isMultipart; }

private:
	void initData(const QByteArray & data);
	QHash<QByteArray, QByteArray> values(QByteArray str);
	QString subjectDecode(QByteArray subject);

	QByteArray m_from;
	QByteArray m_sender;
	QString m_subject;
	QDateTime m_dateTime;
	QByteArray m_mimeVersion;
	QByteArray m_plainText;
	QByteArray m_plainTextCharset;
	QByteArray m_rtfBase64;
	QByteArray m_xMrimVersion;
	quint32 m_xMrimFlags;
	quint32 m_xMrimMultichatType;
	bool m_hasPlainText;
	bool m_hasRtfText;
	bool m_isMultipart;
};

#endif
