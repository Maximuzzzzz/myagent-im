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

#ifndef SMSEDIT_H
#define SMSEDIT_H

#include <QPlainTextEdit>

class SmsEdit : public QPlainTextEdit
{
Q_OBJECT
public:
	explicit SmsEdit(int reservedSymbols, QWidget* parent = 0);
	~SmsEdit();

	// number of remaining symbols
	int freeSpace() const;

	bool setAutoTranslit(bool b);

	QString text() const;
Q_SIGNALS:
	void freeSpaceChanged(int n);
protected:
	virtual void insertFromMimeData(const QMimeData* source);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void contextMenuEvent(QContextMenuEvent* event);
private Q_SLOTS:
	void checkText();
private:
	inline int maxStandardLength() const;
	inline int maxUnicodeLength() const;
private:
	int m_reservedSymbols;
	int m_freeSpace;
	bool m_hasNonGsmSymbols;
	bool m_autoTranslit;
};

#endif
