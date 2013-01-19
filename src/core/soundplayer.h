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

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>

class QMediaPlayer;

enum SoundType
{
	STAuth,
	STLetter,
	STMessage,
	STOtprav,
	STRing,
	STConference
};

class SoundPlayer : public QObject
{
Q_OBJECT
public:
	SoundPlayer(QObject* parent = 0);
	~SoundPlayer();
	bool playSound(SoundType soundType);

Q_SIGNALS:
	void finished(SoundType soundType);

private:
	QList<SoundType> sounds;
	QString soundDescription(SoundType soundType);
	SoundType currentSound;
	QMediaPlayer* mediaPlayer;

private Q_SLOTS:
	void mediaStateChanged();
};

#endif
