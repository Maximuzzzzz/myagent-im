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

#include <QDebug>
#include <QCoreApplication>
#include <QLocale>

#include "resourcemanager.h"
#include "protocol/mrim/proto.h"

const int ResourceManager::maxDefaultStatuses = 10;
const int ResourceManager::minDefaultStatuses = 5;
const QString ResourceManager::defDateFormat = "dd.MM.yyyy hh:mm:ss";
const int ResourceManager::popupWindowWidth = 225;
const int ResourceManager::popupWindowHeight = 63;

ResourceManager::ResourceManager(QObject *parent)
	: QObject(parent)
{
	m_settings = new QSettings(basePath() + "/settings.txt", QSettings::IniFormat, this);
#ifdef DATADIR
	QString emoticonsPath = QLatin1String(DATADIR) + "/emoticons";
	QString onlineStatusesPath = QLatin1String(DATADIR) + "/emoticons";
	QString soundsPath = QLatin1String(DATADIR) + "/sounds";
	QString flashPath = QLatin1String(DATADIR) + "/emoticons/flash";
#else
	QString emoticonsPath = QCoreApplication::applicationDirPath() + "/emoticons";
	QString onlineStatusesPath = QCoreApplication::applicationDirPath() + "/emoticons";
	QString soundsPath = QCoreApplication::applicationDirPath() + "/sounds";
	QString flashPath = QCoreApplication::applicationDirPath() + "/emoticons/flash";
#endif
	QDir::addSearchPath(emoticonsResourcePrefix(), emoticonsPath);
	QDir::addSearchPath(statusesResourcePrefix(), onlineStatusesPath);
	QDir::addSearchPath(soundsResourcePrefix(), soundsPath);
	QDir::addSearchPath(flashResourcePrefix(), flashPath);
	m_locale = QLocale::system().name();
	qDebug() << "loc:" << (emoticonsPath + "/skin/" + m_locale + "/skin.txt");
	QFile f(emoticonsPath + "/skin/" + m_locale + "/skin.txt");
	if (!f.exists())
		m_locale = "en_US";
	m_onlineStatuses.load(onlineStatusesPath + "/skin/" + m_locale + "/skin.txt");
	m_emoticons.load(emoticonsPath + "/skin/" + m_locale + "/skin.txt", m_settings);
	QDir userSkin(emoticonsPath + "/skin/" + m_locale + "/user");
	QStringList userSkinFiles = userSkin.entryList(QDir::Files);
	QStringList::iterator it = userSkinFiles.begin();
	for (; it != userSkinFiles.end(); ++it)
		m_onlineStatuses.load(emoticonsPath + "/skin/" + m_locale + "/user/" + (*it));
	for (it = userSkinFiles.begin(); it != userSkinFiles.end(); ++it)
		m_emoticons.load(emoticonsPath + "/skin/" + m_locale + "/user/" + (*it), m_settings);
	m_mults.load();
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

QString ResourceManager::soundsResourcePrefix()
{
	return "sounds";
}

QString ResourceManager::flashResourcePrefix()
{
	return "flash";
}

void ResourceManager::setAudio(Audio* a)
{
	audio = a;
}

void ResourceManager::setAccount(Account* acc)
{
	m_account = acc;
}

OnlineStatus ResourceManager::loadOnlineStatus(QByteArray email)
{
	QDir dir = basePath() + "/" + email;
	if (!dir.exists())
		return OnlineStatus("", "");

	QSettings* userSettings = new QSettings(dir.absolutePath() + "/settings.txt", QSettings::IniFormat, this);

	QByteArray stId = m_settings->value(email + "/lastOnlineStatus", "").toByteArray();
	QString stDescr = m_settings->value(email + "/lastOnlineStatusDescr", "").toString();

	if (!stId.isEmpty())
	{
		m_settings->remove(email + "/lastOnlineStatus"); //compatible with old version
		m_settings->remove(email + "/lastOnlineStatusDescr"); //for deleting garbage

		if (userSettings->value("Statuses/lastOnlineStatus", "").toByteArray() == "")
			userSettings->setValue("Statuses/lastOnlineStatus", stId);
		if (userSettings->value("Statuses/lastOnlineStatusDescr", "").toString() == "")
			userSettings->setValue("Statuses/lastOnlineStatusDescr", stDescr);

		const OnlineStatusInfo* statusInfo = m_onlineStatuses.getOnlineStatusInfo(stId);
		if (statusInfo && statusInfo->builtIn() == "1")
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

QByteArray ResourceManager::loadPass(QByteArray email)
{
	QDir dir = basePath() + "/" + email;
	if (!dir.exists())
		return QByteArray("");

	QSettings* userSettings = new QSettings(dir.absolutePath() + "/settings.txt", QSettings::IniFormat, this);
	return QByteArray::fromBase64(userSettings->value("Account/Password", QByteArray("").toBase64()).toByteArray());
}

int ResourceManager::removeFolder(QString path)
{
	QDir dir(path);

	int res = 0;
	QStringList lstDirs  = dir.entryList(QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot);
	QStringList lstFiles = dir.entryList(QDir::Files);

	Q_FOREACH (QString entry, lstFiles)
	{
		QString entryAbsPath = dir.absolutePath() + "/" + entry;
		QFile::remove(entryAbsPath);
	}

	Q_FOREACH (QString entry, lstDirs)
	{
		QString entryAbsPath = dir.absolutePath() + "/" + entry;
		removeFolder(entryAbsPath);
	}

	if (!QDir().rmdir(dir.absolutePath()))
		res = 1;
	return res;
}
