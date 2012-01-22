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

#include "fileexistsdialog.h"

#include <QLabel>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

FileExistsDialog::FileExistsDialog(QString fileName, QWidget* parent, Qt::WindowFlags f)
 : QDialog(parent, f), m_oldFileName(fileName)
{
	label = new QLabel(tr("File %1 is already exists. What should we do?").arg(fileName));
	quint32 i;
	QFile file;
	for (i = 1; i <= sizeof(quint32); i++)
	{
		file.setFileName(fileName + QString("-") + QString::number(i));
		if (!file.exists())
			break;
	}
	m_newFileName = file.fileName();
	lineEdit = new QLineEdit(m_newFileName);
	rewriteButton = new QPushButton(tr("Rewrite file"));
	renameButton = new QPushButton(tr("Rename file"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(rewriteButton, SIGNAL(clicked()), this, SLOT(rewriteFile()));
	connect(renameButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle(tr("File exists"));
	QVBoxLayout* layout = new QVBoxLayout;
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(rewriteButton);
	buttonsLayout->addWidget(renameButton);
	buttonsLayout->addWidget(cancelButton);
	layout->addWidget(label);
	layout->addWidget(lineEdit);
	layout->addLayout(buttonsLayout);
	setLayout(layout);
}

void FileExistsDialog::rewriteFile()
{
	m_newFileName = m_oldFileName;
	accept();
}
