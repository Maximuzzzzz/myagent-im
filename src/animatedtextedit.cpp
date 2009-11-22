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

#include "animatedtextedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "rtfparser.h"

AnimatedTextEdit::AnimatedTextEdit(QWidget* parent)
	: AnimationSupport<QTextEdit> (parent)
{
	connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(processDocumentChange(int,int,int)));
}

void AnimatedTextEdit::contextMenuEvent(QContextMenuEvent* event)
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

void AnimatedTextEdit::insertFromMimeData(const QMimeData * source)
{
	if (this->isReadOnly())
		return;
	
	qDebug() << "AnimatedTextEdit::insertFromMimeData formats = " << source->formats();

	if (source->hasFormat(QLatin1String("text/rtf")) && this->acceptRichText())
	{
		qDebug() << "insertFromMimeData: " << source->data("text/rtf");
		QTextDocument selectionDoc;
		RtfParser rtfParser;
		rtfParser.parse(source->data("text/rtf"), &selectionDoc);
		this->textCursor().insertFragment(QTextDocumentFragment(&selectionDoc));
		this->ensureCursorVisible();
	}
	else if (source->hasText())
	{
		this->textCursor().insertText(source->text());
	}
	else
	{
		qDebug() << "object type = " << this->textCursor().charFormat().objectType();
		QTextEdit::insertFromMimeData(source);
	}
}
