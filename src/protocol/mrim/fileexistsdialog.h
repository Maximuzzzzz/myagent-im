/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
 *   Copyright (C) 2012 by Alexander Volkov <volkov0aa@gmail.com>          *
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

#ifndef FILEEXISTSDIALOG_H
#define FILEEXISTSDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;

class FileExistsDialog : public QDialog
{
Q_OBJECT
public:
	explicit FileExistsDialog(QString fileName, QWidget* parent = 0, Qt::WindowFlags f = 0);
	QString fileName() const { return m_newFileName; }

private Q_SLOTS:
	void rewriteFile();

private:
	QLabel* label;
	QLineEdit* lineEdit;
	QPushButton* rewriteButton;
	QPushButton* renameButton;
	QPushButton* cancelButton;

	QString m_newFileName;
	QString m_oldFileName;
};

#endif // FILEEXISTSDIALOG_H
