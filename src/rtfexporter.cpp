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

#include "rtfexporter.h"

#include <QDebug>

#include <QTextCodec>
#include <QFontInfo>

#include "emoticonformat.h"

RtfExporter::RtfExporter(const QTextDocument* _doc)
	: doc(_doc)
{
}

RtfExporter::~RtfExporter()
{
	doc = NULL;
}

QByteArray RtfExporter::toRtf()
{
	controlCodeApplied = true;
	firstParsing = true;
	currentFont = -1;

	QTextBlock currentBlock = doc->begin();
	while (currentBlock.isValid())
	{
		processBlock(currentBlock);
		currentBlock = currentBlock.next();
	}

	QByteArray head = "{\\rtf1\\ansi\\ansicpg1251\\deff0\\deflang1049{\\fonttbl";
	for (int i = 0; i < fonts.size(); i++)
	{
		head += "{\\f" + QByteArray::number(i) + "\\fnil\\fcharset" + fonts.at(i) + ";}";
	}
	head += "}";

	head += "{\\colortbl ;";
	for (int i = 0; i < colors.size(); i++)
	{
		head += "\\red" + QByteArray::number(qRed(colors.at(i)));
		head += "\\green" + QByteArray::number(qGreen(colors.at(i)));
		head += "\\blue" + QByteArray::number(qBlue(colors.at(i))) + ";";
	}
	head += "}";

	head += "\\viewkind4\\uc1\\pard";

	return head + rtf + "}";
}

void RtfExporter::processBlock(const QTextBlock& block)
{
	if (block.text().isEmpty() && !block.next().isValid())
		return;
	
	QTextBlock::iterator it;
	for (it = block.begin(); !(it.atEnd()); ++it)
	{
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid())
			processFragment(currentFragment);
	}
	
	rtf += "\\par";
	controlCodeApplied = true;
}

void RtfExporter::processFragment(const QTextFragment& fragment)
{
	QTextCharFormat fmt = fragment.charFormat();
	
	if (fmt.objectType() == EmoticonFormat::EmoticonFormatType)
	{
		//qDebug() << "RtfExporter emoticon";
		
		if (controlCodeApplied)
		{
			rtf += " ";
			controlCodeApplied = false;
		}
		
		QString id = fmt.property(EmoticonFormat::EmoticonId).toString();
		QByteArray baID = id.toLatin1();
		
		if (id.startsWith('<') && id.endsWith('>'))
			rtf += baID;
		else
			rtf += "<SMILE>id=" + baID + "</SMILE>";
	}
	else
		processCharFormat(fmt, fragment.text());
}

void RtfExporter::processCharFormat(const QTextCharFormat& format, const QString& text)
{
	if (firstParsing)
		currentCharFormat = format;

	int pointSize = format.font().pointSize();
	int currentPointSize = currentCharFormat.font().pointSize();
	if (pointSize == -1 || firstParsing)
	{
		QFontInfo fi(format.font());
		pointSize = fi.pointSize();
		QFontInfo fi2(currentCharFormat.font());
		currentPointSize = fi.pointSize();
	}
	
	if (pointSize != currentPointSize || firstParsing)
	{
		rtf += "\\fs" + QByteArray::number(2*pointSize);
		controlCodeApplied = true;
	}
	
	if (format.fontItalic() != currentCharFormat.fontItalic() || (firstParsing && format.fontItalic()))
	{
		rtf += format.fontItalic() ? "\\i" : "\\i0";
		controlCodeApplied = true;
	}
	
	if (format.fontUnderline() != currentCharFormat.fontUnderline() || (firstParsing && format.fontUnderline()))
	{
		rtf += format.fontUnderline() ? "\\ul" : "\\ulnone";
		controlCodeApplied = true;
	}
	
	bool newBold = format.font().bold();
	if (newBold != currentCharFormat.font().bold() || (firstParsing && newBold))
	{
		rtf += newBold ? "\\b" : "\\b0";
		controlCodeApplied = true;
	}
	
	QRgb newColor = format.foreground().color().rgb();
	if (newColor != currentCharFormat.foreground().color().rgb() || firstParsing)
	{
		rtf += "\\cf" + QByteArray::number(indexOfColor(newColor) + 1);
		controlCodeApplied = true;
	}
	
	if (format.background().style() != 0)
	{
		QRgb newBackColor = format.background().color().rgb();
		if (newBackColor != currentCharFormat.background().color().rgb())
		{
			rtf += "\\cb" + QByteArray::number(indexOfColor(newBackColor) + 1);
			controlCodeApplied = true;
		}
	}

	firstParsing = false;
	currentCharFormat = format;
	processText(text);
}

void RtfExporter::processText(const QString& text)
{
	int i = 0;

	for (; i < text.size(); i++)
		checkChar(text[i]);
}

void RtfExporter::checkChar(QChar ch)
{
	QList<QByteArray> codecs;
	QList<QByteArray> charsets;
	codecs << "cp1251";
	charsets << "204";
	QTextCodec* codec;
	QString currCharset;

	int i;
	bool foundCodec = false;
	for (i = 0; i < codecs.size(); i++)
	{
		codec = QTextCodec::codecForName(codecs[i]);
		if (codec->canEncode(ch))
		{
			currCharset = charsets[i];
			foundCodec = true;
			break;
		}
	}

	currCharset = "204";

	int fontIndex;
	fontIndex = indexOfFont(currCharset + " " + currentCharFormat.fontFamily());
	if (currentFont != fontIndex)
	{
		rtf += "\\f" + QByteArray::number(fontIndex);
		currentFont = fontIndex;
	}

	if (!foundCodec)
	{
		rtf += ((controlCodeApplied) ? QByteArray(" ") : QByteArray("")) + QByteArray("\\u") + QByteArray::number(ch.unicode()) + QByteArray("?");
		controlCodeApplied = false;
		return;
	}

	if (ch.toLatin1() != 0)
		if (ch == '\\')
			rtf += ((controlCodeApplied) ? QByteArray(" ") : QByteArray("")) + "\\\\";
		else
			rtf += ((controlCodeApplied) ? QByteArray(" ") : QByteArray("")) + ch.toLatin1();
	else
	{
		uchar c = codec->fromUnicode(ch).at(0);
		rtf += QByteArray("\\'") + QByteArray::number(c, 16);
	}
	controlCodeApplied = false;
}

int RtfExporter::indexOfFont(QString fontFamily)
{
	QByteArray baFontFamily = fontFamily.toLatin1();
	
	int i = fonts.indexOf(baFontFamily);
	if (i == -1)
	{
		qDebug() << "RtfExporter: fonts: append font family " << fontFamily;
		fonts.append(baFontFamily);
		i = fonts.size() - 1;
	}
	
	return i;
}

int RtfExporter::indexOfColor(QRgb rgb)
{
	int i = colors.indexOf(rgb);
	
	if (i == -1)
	{
		colors.append(rgb);
		i = colors.size() - 1;
	}
	
	return i;
}
