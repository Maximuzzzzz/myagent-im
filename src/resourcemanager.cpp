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

#include <QDir>
#include <QDebug>
#include <QCoreApplication>

#include "resourcemanager.h"
#include "proto.h"

const int ResourceManager::maxDefaultStatuses = 10;
const int ResourceManager::minDefaultStatuses = 5;

ResourceManager::ResourceManager(QObject *parent)
	: QObject(parent)
{
	m_settings = new QSettings(basePath() + "/settings.txt", QSettings::IniFormat, this);
#ifdef DATADIR
	QString emoticonsPath = QLatin1String(DATADIR) + "/emoticons";
	QString onlineStatusesPath = QLatin1String(DATADIR) + "/emoticons";
	soundsPath = QLatin1String(DATADIR) + "/sounds";
#else
	QString emoticonsPath = QCoreApplication::applicationDirPath() + "/emoticons";
	QString onlineStatusesPath = QCoreApplication::applicationDirPath() + "/emoticons";
	soundsPath = QCoreApplication::applicationDirPath() + "/sounds";
#endif
	QDir::addSearchPath(emoticonsResourcePrefix(), emoticonsPath);
	QDir::addSearchPath(statusesResourcePrefix(), onlineStatusesPath);
	m_onlineStatuses.load(onlineStatusesPath + "/skin.txt");
	m_emoticons.load(emoticonsPath + "/skin.txt", m_settings);
	m_locations.load(":/region.txt");
}

ResourceManager::~ResourceManager()
{
	qDebug() << "ResourceManager::~ResourceManager()";
}

ResourceManager & ResourceManager::self()
{
	static ResourceManager rm;
	return rm;
}

QString ResourceManager::basePath()
{
#ifdef Q_WS_X11
	QDir dir = QDir::home();
#else
	QDir dir = QCoreApplication::applicationDirPath();
#endif
	if (!dir.exists())
	{
		qDebug() << "Home dir doesn\'t exist";
		return "";
	}
	if (!dir.exists(".myagent"))
		if (!dir.mkdir(".myagent"))
	{
		qDebug() << "Can\'t create .myagent subdir";
		return "";
	}
	dir.cd(".myagent");
	return dir.absolutePath();
}

QString ResourceManager::emoticonsResourcePrefix()
{
	return "smiles";
}

QString ResourceManager::statusesResourcePrefix()
{
	return "statuses";
}

void ResourceManager::setAudio(Audio* a)
{
	audio = a;
}

OnlineStatus ResourceManager::loadOnlineStatus(QByteArray email)
{
	QDir dir = basePath() + "/" + email;
	if (!dir.exists())
		return OnlineStatus("", "");

	QSettings* userSettings = new QSettings(dir.absolutePath() + "/settings.txt", QSettings::IniFormat, this);

	QByteArray stId = m_settings->value(email + "/lastOnlineStatus", "").toByteArray();
	QString stDescr = m_settings->value(email + "/lastOnlineStatusDescr", "").toString();

	if (stId != "")
	{
		m_settings->remove(email + "/lastOnlineStatus"); //compatible with old version
		m_settings->remove(email + "/lastOnlineStatusDescr"); //for deleting garbage

		if (userSettings->value("Statuses/lastOnlineStatus", "").toByteArray() == "")
			userSettings->setValue("Statuses/lastOnlineStatus", stId);
		if (userSettings->value("Statuses/lastOnlineStatusDescr", "").toString() == "")
			userSettings->setValue("Statuses/lastOnlineStatusDescr", stDescr);

		if (m_onlineStatuses.getOnlineStatusInfo(stId)->BuiltIn() == "1")
		{
			userSettings->remove("Statuses/statusPointer");
			return OnlineStatus(stId);
		}

		return OnlineStatus(stId, stDescr);
	}
	else
	{
		stId = userSettings->value("Statuses/lastOnlineStatus", "").toByteArray();
		stDescr = userSettings->value("Statuses/lastOnlineStatusDescr", "").toString();
	}
	return OnlineStatus(stId, stDescr);
}
