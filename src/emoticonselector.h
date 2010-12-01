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

#ifndef EMOTICONSELECTOR_H
#define EMOTICONSELECTOR_H

#include <QFrame>
#include <QTabWidget>
#include <QGridLayout>

class QToolBox;
class EmoticonMovie;
class EmoticonSelector;

class EmoticonSelectorPage : public QWidget
{
Q_OBJECT
public:
	EmoticonSelectorPage(EmoticonSelector* parent, QStringList set, bool isFav = false);
        ~EmoticonSelectorPage();

	void correctSize();

private slots:
	void setupFavouriteEmoticons();

private:
	QWidget* createEmoticonsWidget(/*int emoticonsPerRow*/);
	void setCurrentLayout();
	QStringList m_set;

private:
	typedef QList<EmoticonMovie*> MovieList;
	MovieList movieList;
	QWidget* emotions;
	EmoticonSelector* m_parentSelector;
	QGridLayout* gridLayout;
	bool m_isFav;
};

class EmoticonSelector : public QFrame
{
Q_OBJECT
public:
	EmoticonSelector(QWidget* parent = 0);
	~EmoticonSelector();

public slots:
	void  slotClicked(QString id);

signals:
	void selected(QString id);
	void closed();

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
	QList<EmoticonSelectorPage*> pages;
	QTabWidget* tabs;
};

#endif
