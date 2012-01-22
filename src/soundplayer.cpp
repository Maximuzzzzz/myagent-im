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

#include "soundplayer.h"

#include <QDebug>

#include "resourcemanager.h"

SoundPlayer::SoundPlayer(QObject *parent)
	: QObject(parent), media(0), currentSound(STAuth)
{
}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::playSound(SoundType soundType)
{
	qDebug() << "Audio::playSound(\"" + soundDescription(soundType) + ".ogg\")";
	currentSound = soundType;
	media = new Phonon::MediaObject(this);
	QString mediaFile = soundDescription(soundType);

	qDebug() << "File: " << (theRM.soundsResourcePrefix() + ':' + mediaFile + ".ogg");
	if (mediaFile.isEmpty())
		qDebug() << "Error: Media file not found";
	else
		media->setCurrentSource(Phonon::MediaSource(theRM.soundsResourcePrefix() + ':' + mediaFile + ".ogg"));
	Phonon::createPath(media, &output);
	connect(media, SIGNAL(finished()), this, SLOT(finish()));
	media->play();
}

void SoundPlayer::finish()
{
	qDebug() << "Emitting \"finished\"";
	deleteLater();
	Q_EMIT finished(currentSound);
}

QString SoundPlayer::soundDescription(SoundType soundType)
{
	switch (soundType)
	{
		case STAuth:
			return "auth";
		case STLetter:
			return "letter";
		case STMessage:
			return "message";
		case STOtprav:
			return "otprav";
		case STRing:
			return "ring";
		case STConference:
			return "conference";
		default:
			return "";
	}
}
