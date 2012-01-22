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

#include "aboutdialog.h"

#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

	vers->setText(tr("Version: %1").arg(VERSION));

	connect(buttonOK, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(linkLabel, SIGNAL(linkActivated(QString)), this, SLOT(openHomeURL()));
	connect(licenseLabel, SIGNAL(linkActivated(QString)), this, SLOT(openLicenseURL()));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::openHomeURL()
{
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/myagent-im"));
}

void AboutDialog::openLicenseURL()
{
	QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses"));
}
