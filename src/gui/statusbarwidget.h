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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QWidget>

class QLabel;

class StatusBarWidget : public QWidget
{
Q_OBJECT
public:
	StatusBarWidget();
	~StatusBarWidget();

public Q_SLOTS:
	void clearStatus();
	void setStatus(const QString& status);
	void setStatus(QByteArray status);
	void setActive(bool state);

Q_SIGNALS:
	void clicked();

protected:
	virtual void mousePressEvent(QMouseEvent* event);

private:
	QLabel* statusLabel;
	bool active;
};

#endif
