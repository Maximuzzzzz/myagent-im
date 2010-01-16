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

ResourceManager::ResourceManager(QObject *parent)
	: QObject(parent)
{
	//Q_INIT_RESOURCE(res);
	//userPath = "";
	m_settings = new QSettings(basePath() + "/settings.txt", QSettings::IniFormat, this);
#ifdef DATADIR
	QString emoticonsPath = QLatin1String(DATADIR) + "/emoticons";
	soundsPath = QLatin1String(DATADIR) + "/sounds";
#else
	QString emoticonsPath = QCoreApplication::applicationDirPath() + "/emoticons";
	soundsPath = QCoreApplication::applicationDirPath() + "/sounds";
#endif
	QDir::addSearchPath(emoticonsResourcePrefix(), emoticonsPath);
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

/*void ResourceManager::setUserPath(QString path)
{
	userPath = path;
}*/
