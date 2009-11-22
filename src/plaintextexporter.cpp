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

#include "plaintextexporter.h"

#include "emoticonformat.h"
#include "resourcemanager.h"

PlainTextExporter::PlainTextExporter(const QTextDocument* _doc)
	: doc(_doc)
{
}

PlainTextExporter::~PlainTextExporter()
{
	doc = NULL;
}

QString PlainTextExporter::toText()
{
	QTextBlock currentBlock = doc->begin();
	while (currentBlock.length())
	{
		processBlock(currentBlock);
		
		currentBlock = currentBlock.next();
	}
	
	return text;
}

void PlainTextExporter::processBlock(const QTextBlock& block)
{	
	QTextBlock::iterator it;
	for (it = block.begin(); !(it.atEnd()); ++it)
	{
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid())
			processFragment(currentFragment);
	}
	
	if (block.next().length())
		text += '\n';
}

void PlainTextExporter::processFragment(const QTextFragment& fragment)
{
	QTextCharFormat fmt = fragment.charFormat();
	
	if (fmt.objectType() == EmoticonFormat::EmoticonFormatType)
	{
		QString id = fmt.property(EmoticonFormat::EmoticonId).toString();
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(id);
		
		if (!info)
		{
			text += id;
			return;
		}

		if (!info->alt().isEmpty())
			text += info->alt();
		else if (!info->tip().isEmpty())
		{
			if (info->tip().at(0).isLetter())
				text += ":" + info->tip() + ":";
			else
				text += info->tip();
		}
	}
	else
		text += fragment.text();
}
