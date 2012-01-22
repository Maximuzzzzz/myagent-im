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

#include "centeredmessagebox.h"

#include "centerwindow.h"

QMessageBox::StandardButton CenteredMessageBox::messageBox(QMessageBox::Icon icon, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	QMessageBox mb(icon, title, text, buttons);
	mb.setDefaultButton(defaultButton);
	centerWindow(&mb);
	return (QMessageBox::StandardButton)mb.exec();
}

QMessageBox::StandardButton CenteredMessageBox::critical(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Critical, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::information(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Information, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::question(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::warning(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Warning, title, text, buttons, defaultButton);
}
