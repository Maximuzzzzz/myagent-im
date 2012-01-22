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

#include "smsedit.h"

#include <QDebug>

#include <QMenu>

#include "qgsmcodec.h"

namespace
{
	QTextCodec* gsmCodec(const QString& gsmCharset)
	{
		QString cs = gsmCharset.toLower();
		QTextCodec *codec = 0;
		
		if (cs == "gsm")
		{
			// 7-bit GSM character set.
			static QTextCodec *gsm = 0;
			if ( !gsm )
				gsm = new QGsmCodec();
			codec = gsm;
		}
		else if ( cs == "gsm-noloss" )
		{
			// 7-bit GSM character set, with no loss of quality.
			static QTextCodec *gsmNoLoss = 0;
			if ( !gsmNoLoss )
				gsmNoLoss = new QGsmCodec( true );
			codec = gsmNoLoss;
		}
		else
			codec = QTextCodec::codecForName(gsmCharset.toLatin1());

		return codec;
	}

	QString transliterate(QChar c)
	{
		quint16 code = c.unicode();
		
		if (code == 0x0401)
			return "Yo";
		else if (code == 0x0451)
			return "yo";

		if (code < 0x0410 || 0x04ff < code)
			return c;
		
		static const char* translitTable[] =
		{
			"A", "B", "V", "G", "D", "E", "Zh", "Z", "I", "J", "K", "L", "M", "N", "O", "P",
			"R", "S", "T", "U", "F", "X", "Ts", "Ch", "Sh", "Sch", "''", "Y", "'", "E'", "Yu", "Ya",
			"a", "b", "v", "g", "d", "e", "zh", "z", "i", "j", "k", "l", "m", "n", "o", "p",
			"r", "s", "t", "u", "f", "x", "ts", "ch", "sh", "sch", "''", "y", "'", "e'", "yu", "ya"
		};
		
		return translitTable[code - 0x0410];
	}
	
	QString transliterate(const QString& text, int maxSize = 0, int* processed = 0)
	{
		QString result;
		
		int i = 0;
		for (; i < text.size(); i++)
		{
			QString newChar = transliterate(text.at(i));
			if ((maxSize != 0) && (result.size() + newChar.size() > maxSize))
				break;
			result.append(newChar);
		}
		
		if (processed)
			*processed = i;
		return result;
	}
}

SmsEdit::SmsEdit(int reservedSymbols, QWidget* parent)
	: QPlainTextEdit(parent), m_reservedSymbols(reservedSymbols), m_freeSpace(0), m_hasNonGsmSymbols(false),
		m_autoTranslit(false)
{
	checkText();
	connect(this, SIGNAL(textChanged()), this, SLOT(checkText()));
}

SmsEdit::~SmsEdit()
{
	//qDebug() << "SmsEdit::~SmsEdit()";
}

inline int SmsEdit::maxStandardLength() const
{
	return 160 - m_reservedSymbols;
}

inline int SmsEdit::maxUnicodeLength() const
{
	return 70 - m_reservedSymbols;
}

void SmsEdit::checkText()
{
	qDebug() << "SmsEdit::checkText()";
	QString text = toPlainText();

	if (m_autoTranslit)
		text = transliterate(text);

	qDebug() << "text = " << text;
	
	QTextCodec* codec = gsmCodec("gsm-noloss");

	int maxLength;	
	if (codec->canEncode(text))
	{
		m_hasNonGsmSymbols = false;
		maxLength = maxStandardLength();
	}
	else
	{
		qDebug() << "tranliterated text = " << transliterate(text);
		m_hasNonGsmSymbols = true;
		maxLength = maxUnicodeLength();
	}
	
	int length = text.length();
	
	int freeSpace = maxLength - length;
	qDebug () << "freeSpace = " << freeSpace << ", m_freeSpace = " << m_freeSpace;
	if (freeSpace >= 0)
	{
		if (m_freeSpace != freeSpace)
		{
			m_freeSpace = freeSpace;
			Q_EMIT freeSpaceChanged(m_freeSpace);
		}
	}
	else
		undo();
}

int SmsEdit::freeSpace() const
{
	return m_freeSpace;
}

void SmsEdit::insertFromMimeData(const QMimeData * source)
{
	if (this->isReadOnly())
		return;

	if (source->hasText())
	{
		QString text = source->text();
		int maxLen;
		QString tmp = transliterate(text, m_freeSpace, &maxLen);
		text.truncate(maxLen);
		insertPlainText(text);
	}
}

void SmsEdit::keyPressEvent(QKeyEvent * event)
{
	//qDebug() << "SmsEdit::keyPressEvent text = " << event->text() << ", key = " << QString::number(event->key(), 16) << ", modifiers = " << QString::number(event->modifiers(), 16);

	QString text = event->text();

	if (!text.isEmpty() && (text.at(0).isPrint() || text.at(0) == QLatin1Char('\t')))
	{
		if (m_freeSpace == 0)
		{
			event->accept();
			return;
		}
		else if (m_autoTranslit)
		{
			if (m_freeSpace < transliterate(text).size())
			{
				event->accept();
				return;
			}
		}
		else if (!m_hasNonGsmSymbols)
		{
			QTextCodec* codec = gsmCodec("gsm-noloss");
			if (!codec->canEncode(text))
			{
				if (m_freeSpace < (maxStandardLength() - maxUnicodeLength()))
				{
					event->accept();
					return;
				}
			}
		}
	}
	
	QPlainTextEdit::keyPressEvent(event);
}

void SmsEdit::contextMenuEvent(QContextMenuEvent * event)
{
	QMenu *menu = createStandardContextMenu();
	QList<QAction*> menuActions = menu->actions();
	int N = menuActions.size();
	if (N > 2)
	{
		menu->removeAction(menuActions.at(N-1));
		menu->removeAction(menuActions.at(N-2));
	}
	menu->exec(event->globalPos());
	delete menu;
}

bool SmsEdit::setAutoTranslit(bool b)
{
	if (m_autoTranslit == b)
		return true;
	
	QString text = toPlainText();

	if (b)
		text = transliterate(text);
	
	QTextCodec* codec = gsmCodec("gsm-noloss");

	int maxLength;	
	if (codec->canEncode(text))
		maxLength = maxStandardLength();
	else
		maxLength = maxUnicodeLength();
	
	if (text.length() > maxLength)
		return false;

	m_autoTranslit = b;
	checkText();
	return true;
}

QString SmsEdit::text() const
{
	QString txt = toPlainText();
	if (m_autoTranslit)
		txt = transliterate(txt);

	return txt;
}
