/***************************************************************************
 *   Copyright (C) 2008 by Alexander Volkov                                *
 *   volkov0aa@gmail.com                                                   *
 *                                                                         *
 *   This file is part of instant messenger MyAgent-IM                     *
 *   This class is based on ChatEdit from Psi                              *
 *       (Copyright (C) 2001-2003  Justin Karneges, Michail Pishchagin)    *
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

#include "messageedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "spellchecker/spellchecker.h"
#include "spellchecker/spellhighlighter.h"

MessageEdit::MessageEdit(QWidget* parent)
	: AnimatedTextEdit(parent)
{
	checkSpelling = false;
	spellHighlighter = 0;
}

void MessageEdit::setCheckSpelling(bool on)
{
	checkSpelling = on;
	if (checkSpelling)
	{
		if (!spellHighlighter)
			spellHighlighter = new SpellHighlighter(document());
	}
	else
	{
		delete spellHighlighter;
		spellHighlighter = 0;
	}
}

void MessageEdit::contextMenuEvent(QContextMenuEvent * e)
{
	lastClick = e->pos();
	if (checkSpelling && textCursor().selectedText().isEmpty() && SpellChecker::instance()->available())
	{
		// Check if the word under the cursor is misspelled
		QTextCursor tc = cursorForPosition(e->pos());
		tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
		tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		QString selectedWord = tc.selectedText();
		if (!selectedWord.isEmpty() && !SpellChecker::instance()->isCorrect(selectedWord))
		{
			QList<QString> suggestions = SpellChecker::instance()->suggestions(selectedWord);
			if (!suggestions.isEmpty() || SpellChecker::instance()->writable())
			{
				QMenu spellMenu;
				if (!suggestions.isEmpty())
				{
					foreach(QString suggestion, suggestions)
					{
						QAction* suggestionAction = spellMenu.addAction(suggestion);
						connect(suggestionAction,SIGNAL(triggered()),SLOT(applySuggestion()));
					}
					spellMenu.addSeparator();
				}
				if (SpellChecker::instance()->writable())
				{
					QAction* addAction = spellMenu.addAction(tr("Add to dictionary"));
					connect(addAction,SIGNAL(triggered()),SLOT(addToDictionary()));
				}
				spellMenu.exec(QCursor::pos());
				e->accept();
				return;
			}
		}
	}

	// Do normal menu
	AnimatedTextEdit::contextMenuEvent(e);
	e->accept();
}

void MessageEdit::applySuggestion()
{
	QAction* suggestionAction = qobject_cast<QAction*>(sender());
	int currentPosition = textCursor().position();
	
	// Replace the word
	QTextCursor	tc = cursorForPosition(lastClick);
	tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
	tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
	int oldLength = tc.position() - tc.anchor();
	tc.insertText(suggestionAction->text());
	tc.clearSelection();

	// Put the cursor where it belongs
	int newLength = suggestionAction->text().length();
	tc.setPosition(currentPosition - oldLength + newLength);
	setTextCursor(tc);
}

void MessageEdit::addToDictionary()
{
	QTextCursor	tc = cursorForPosition(lastClick);
	int current_position = textCursor().position();

	// Get the selected word
	tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
	tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
	SpellChecker::instance()->add(tc.selectedText());
	
	// Put the cursor where it belongs
	tc.clearSelection();
	tc.setPosition(current_position);
	setTextCursor(tc);
}
