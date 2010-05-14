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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QIcon>
#include <QSettings>

#include "emoticons.h"
#include "locations.h"
#include "audio.h"

class ResourceManager : public QObject
{
Q_OBJECT
public:
	static ResourceManager& self();

	~ResourceManager();

	QString basePath();
	QSettings* settings() { return m_settings; }
	Emoticons& emoticons() { return m_emoticons; }
	const Locations& locations() const { return m_locations; }
	QString emoticonsResourcePrefix();
	const QString getSoundsPath() const { return soundsPath; }
	void setAudio(Audio* a);
	Audio* getAudio() { return audio; }

private:
	ResourceManager(QObject* parent = 0);
	ResourceManager(const ResourceManager& rm);

	QSettings* m_settings;
	Emoticons m_emoticons;
	Locations m_locations;
	QString soundsPath;
	Audio* audio;
};

#define theRM ResourceManager::self()

#endif
