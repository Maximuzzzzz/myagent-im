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

#ifndef MESSAGEEDIT_H
#define MESSAGEEDIT_H

#include "animatedtextedit.h"

class SpellHighlighter;

class MessageEdit : public AnimatedTextEdit
{
	Q_OBJECT
public:
	MessageEdit(QWidget* parent = 0);
	void setCheckSpelling(bool on);

private slots:
 	void applySuggestion();
 	void addToDictionary();

protected:
	void contextMenuEvent(QContextMenuEvent *e);
private:
	bool checkSpelling;
	SpellHighlighter* spellHighlighter;
	QPoint lastClick;
};

#endif
