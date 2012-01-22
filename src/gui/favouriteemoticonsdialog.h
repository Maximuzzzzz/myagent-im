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

#ifndef FAVOURITEEMOTICONSDIALOG_H
#define FAVOURITEEMOTICONSDIALOG_H

#include <QDialog>
#include <QFrame>

class FavouriteEmoticonsDialog : public QDialog
{
	Q_OBJECT
	class FavoritesEmoticonsWidget;
public:
	FavouriteEmoticonsDialog();
Q_SIGNALS:
	void doubleClicked(QString id);
private Q_SLOTS:
	void slotAccepted();
	void slotDoubleClicked(QString id);
private:
	FavoritesEmoticonsWidget* favouriteEmoticonsWidget;
};

class FavouriteEmoticonsDialog::FavoritesEmoticonsWidget : public QFrame
{
	Q_OBJECT
public:
	FavoritesEmoticonsWidget(int totalEmoticons);

	QStringList favouriteEmoticons() const;
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	virtual void dragLeaveEvent ( QDragLeaveEvent * event );
	virtual void dragMoveEvent(QDragMoveEvent* event);
	virtual void paintEvent(QPaintEvent* event);
private Q_SLOTS:
	void emoticonDraggingStarted();
private:
	QRect cursorRect;
};

#endif
