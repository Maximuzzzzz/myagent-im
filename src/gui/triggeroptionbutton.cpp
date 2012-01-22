/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#include "triggeroptionbutton.h"

#include <QSettings>

#include "resourcemanager.h"

TriggerOptionButton::TriggerOptionButton(const QString& iconPrefix, const QString& optionName, QWidget* parent)
	: QToolButton(parent), m_optionName(optionName)
{
	QIcon icon;

	QPixmap pm(":/icons/" + iconPrefix + "_d.png");
	QSize iconSize = pm.size();

	icon.addPixmap(pm, QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_p.png"), QIcon::Normal, QIcon::On);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_ph.png"), QIcon::Active, QIcon::On);

	setIcon(icon);
	setIconSize(iconSize);

	setCheckable(true);
	setAutoRaise(true);
	setStyleSheet("QToolButton { border: 0px; padding: 0px }");

	setChecked(theRM.settings()->value(optionName, false).toBool());

	connect(this, SIGNAL(toggled(bool)), this, SLOT(updateSettings(bool)));
}

void TriggerOptionButton::updateSettings(bool enableOption)
{
	theRM.settings()->setValue(m_optionName, enableOption);
}
