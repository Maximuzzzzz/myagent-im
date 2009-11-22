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

#ifndef RADIOGROUPBOX_H
#define RADIOGROUPBOX_H

#include <QGroupBox>

class QRadioButton;

class RadioGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	RadioGroupBox(const QString& title, QWidget* parent = 0);
	RadioGroupBox(QWidget* parent = 0);
	~RadioGroupBox();

	void setChecked(bool b);
protected:
	void initStyleOption(QStyleOptionGroupBox* option)const;
	
	virtual void paintEvent (QPaintEvent* event);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent* e);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual bool event(QEvent* event);
private:
	void init();
	bool hitButtonOrLabel(const QPoint &pos) const;
	
	bool down;
};

#endif
