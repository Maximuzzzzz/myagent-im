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

#ifndef CENTEREDMESSAGEBOX_H
#define CENTEREDMESSAGEBOX_H

#include <QMessageBox>

class CenteredMessageBox
{
public:
	static QMessageBox::StandardButton critical(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton information(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton question(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton warning(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
private:
	static inline QMessageBox::StandardButton messageBox(QMessageBox::Icon icon, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);
};

#endif
