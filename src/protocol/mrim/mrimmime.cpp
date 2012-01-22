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

#include "mrimmime.h"

#include <QDebug>
#include <QTextCodec>

#include "datetime.h"

MrimMIME::MrimMIME()
	: m_hasPlainText(false), m_hasRtfText(false), m_isMultipart(false)
{
}

MrimMIME::MrimMIME(const QByteArray & data)
{
	initData(data);
}

void MrimMIME::initData(const QByteArray & data)
{
	qDebug() << "MrimMIME::initData";
	qDebug() << data;

	QList<QByteArray> strings = data.split(0x0a);
	int i;

	m_isMultipart = m_hasPlainText = m_hasRtfText = false;
	QByteArray boundary;
	QByteArray currType;
	QByteArray currCodec;
	QByteArray transferEncoding;
	QHash<QByteArray, QByteArray> currStr;

	bool isHeader = true;
	m_plainTextCharset = m_plainText = "";
	for (i = 0; i < strings.size(); i++)
	{
		if (strings[i].trimmed().size() == 0 && isHeader)
			continue;

		qDebug() << strings[i];
		currStr = values(strings[i]);
		if (currStr.find("From") != currStr.end())
		{
			m_from = currStr.value("From");
			qDebug() << "m_from = " << m_from;
		}
		else if (currStr.find("Sender") != currStr.end())
		{
			m_sender = currStr.value("Sender");
			qDebug() << "m_sender = " << m_sender;
		}
		else if (currStr.find("Date") != currStr.end())
		{
			m_dateTime = parseRFCDate(currStr.value("Date"));
			qDebug() << "m_dateTime = " << m_dateTime;
		}
		else if (currStr.find("MIME-Version") != currStr.end())
		{
			m_mimeVersion = currStr.value("MIME-Version");
			qDebug() << "m_mimeVersion = " << m_mimeVersion;
		}
		else if (currStr.find("Subject") != currStr.end())
		{
			m_subject = subjectDecode(currStr.value("Subject"));
			qDebug() << "m_subject = " << m_subject;
		}
		else if (currStr.find("Content-Type") != currStr.end())
		{
			currType = currStr.value("Content-Type");
			qDebug() << "currType = " << currType;
			if (currType == "multipart/alternative")
			{
				m_isMultipart = true;
				boundary = QByteArray("--") + currStr.value("boundary");
				qDebug() << "m_isMultipart = " << m_isMultipart;
				qDebug() << "boundary = " << boundary;
			}
			else if (currType == "text/plain")
			{
				m_hasPlainText = true;
				m_plainTextCharset = currStr.value("charset");
				qDebug() << "m_hasPlainText = " << m_hasPlainText;
				qDebug() << "m_plainTextCharset = " << m_plainTextCharset;
			}
			else if (currType == "application/x-mrim-rtf")
			{
				m_hasRtfText = true;
				qDebug() << "m_hasRtfText = " << m_hasRtfText;
			}
			else if (currType == "application/x-mrim-auth-req")
			{
				m_hasRtfText = true;
				qDebug() << "m_hasRtfText = " << m_hasRtfText;
			}
		}
		else if (currStr.find("Content-Transfer-Encoding") != currStr.end())
		{
			transferEncoding = currStr.value("Content-Transfer-Encoding");
			qDebug() << "transferEncoding = " << transferEncoding;
		}
		else if (currStr.find("X-MRIM-Version") != currStr.end())
		{
			m_xMrimVersion = currStr.value("X-MRIM-Version");
			qDebug() << "m_xMrimVersion = " << m_xMrimVersion;
		}
		else if (currStr.find("X-MRIM-Flags") != currStr.end())
		{
			m_xMrimFlags = currStr.value("X-MRIM-Flags").toUInt(NULL, 16);
			qDebug() << "m_xMrimFlags = " << m_xMrimFlags;
		}
		else if (currStr.find("X-MRIM-Multichat-Type") != currStr.end())
		{
			m_xMrimMultichatType = currStr.value("X-MRIM-Multichat-Type").toUInt(NULL, 10);
			qDebug() << "m_xMrimMultichatType = " << m_xMrimMultichatType;
		}
		else if (strings[i].startsWith(boundary) && boundary.length() > 0)
		{
			qDebug() << "starts with boundary";
			isHeader = true;
		}
		else if (!strings[i].isEmpty())
		{
			qDebug() << "Seems isn't header";
			if (currType == "text/plain")
			{
				if (transferEncoding == "base64")
					m_plainText = QByteArray::fromBase64(strings[i]);
				else
					m_plainText = strings[i];
				qDebug() << "m_plainText = " << m_plainText.toHex();
			}
			else if (currType == "application/x-mrim-rtf" || currType == "application/x-mrim-auth-req")
			{
				m_rtfBase64 = strings[i];
				qDebug() << "m_rtfBase64 = " << m_rtfBase64;
			}
			isHeader = false;
		}
	}
}

QHash<QByteArray, QByteArray> MrimMIME::values(QByteArray str)
{
	QHash<QByteArray, QByteArray> result;
	int index = str.indexOf(':');
	int index2;
	do
	{
		if (str.indexOf(';') > 0)
			index2 = str.indexOf(';');
		else
			index2 = str.size();
		result.insert(str.left(index).trimmed(), str.mid(index + 1, index2 - index - 1).trimmed());
		str = str.right(str.size() - index2 - 1);
		index = str.indexOf("=");
	}
	while (str.trimmed().size() > 0);

	return result;
}

QString MrimMIME::subjectDecode(QByteArray subject)
{
	qDebug() << "MrimMIME::subjectDecode" << subject;
	if (!subject.startsWith("=?"))
		return subject;
	subject = subject.mid(2, subject.size() - 4);
	QTextCodec* codec = QTextCodec::codecForName(subject.left(subject.indexOf("?")));
	if (subject.mid(subject.indexOf("?") + 1, 1) == "B")
	{
		subject = QByteArray::fromBase64(subject.mid(subject.indexOf("?") + 3, subject.size() - subject.indexOf("?") + 3));
		return codec->toUnicode(subject);
	}
	else
	{
		qDebug() << "unknown encoding";
		return "";
	}
}
