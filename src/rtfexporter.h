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

#ifndef RTFEXPORTER_H
#define RTFEXPORTER_H

#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextFragment>
#include <QColor>

class RtfExporter
{
public:
	RtfExporter(const QTextDocument* _doc);
	~RtfExporter();
	
	QByteArray toRtf();
private:
	void processBlock(const QTextBlock &block);
	void processFragment(const QTextFragment &fragment);
	void processCharFormat(const QTextCharFormat& format);
	void initCharFormat(const QTextCharFormat& format);
	void processText(const QString& text);
	
	int indexOfFont(QString fontFamily);
	int indexOfColor(QRgb rgb);
	
	const QTextDocument* doc;
	QTextCharFormat currentCharFormat;
	bool controlCodeApplied;
	QByteArray rtf;
	QList<QByteArray> fonts;
	QList<QRgb> colors;
};

#endif
