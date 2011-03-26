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

#include "inputlinedialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QRegExpValidator>

#include "centerwindow.h"

InputLineDialog::InputLineDialog(const QString & title, const QIcon & icon, const QString & labelText, const QString & regExp, const QString & text, QWidget * parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setWindowTitle(title);
	setWindowIcon(icon);

	QVBoxLayout* layout = new QVBoxLayout;
	QLabel* label = new QLabel(labelText);
	lineEdit = new QLineEdit;
	lineEdit->setValidator(new QRegExpValidator(QRegExp(regExp), lineEdit));
	lineEdit->setText(text);
	lineEdit->selectAll();
	connect(lineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(checkText()));
	
	QDialogButtonBox* buttonBox = new QDialogButtonBox;
	okButton = new QPushButton(tr("Ok"));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"));
	buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	checkText();

	layout->addWidget(label);
	layout->addWidget(lineEdit);
	layout->addWidget(buttonBox);
	
	setLayout(layout);
	setFixedSize(sizeHint());
	centerWindow(this);
}

void InputLineDialog::checkText()
{
	okButton->setEnabled(lineEdit->hasAcceptableInput());
}

QString InputLineDialog::text() const
{
	return lineEdit->text();
}
