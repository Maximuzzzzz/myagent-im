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

#ifndef MULTSELECTOR_H
#define MULTSELECTOR_H

#include <QFrame>

class MessageEditor;

class MultSelector : public QFrame
{
Q_OBJECT
public:
	MultSelector(QWidget *parent = 0);
	~MultSelector();

Q_SIGNALS:
	void selected(MessageEditor* editor, QString id);
	void closed();

public Q_SLOTS:
	void appear(MessageEditor* editor, bool visible);

private Q_SLOTS:
	void slotClicked(QString id);
/*	void correctSize();*/
	void setCurrentLayout();

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
	QWidget* createMultIconsWidget();
/*	void setCurrentLayout();*/

private:
	QWidget* mults;
	MessageEditor* m_selectorSender;
};

#endif // MULTSELECTOR_H
