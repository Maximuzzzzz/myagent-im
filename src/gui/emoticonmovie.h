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

#ifndef EMOTICONMOVIE_H
#define EMOTICONMOVIE_H

#include <QMovie>

class EmoticonMovie : public QMovie
{
Q_OBJECT
public:
	EmoticonMovie(QObject *parent = 0);
	~EmoticonMovie();
	
	static bool isAnimated(const QString& fileName);
	void load(const QString& id, int from = 0);
	QString id() const { return id_; }
	
public Q_SLOTS:
	void start();
	void setPaused(bool paused);

protected:
	void setFileName(const QString& fileName);
private:
	QString id_;
	bool animated;
};

#endif
