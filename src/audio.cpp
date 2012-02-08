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

#include "audio.h"

#include <QDebug>
#include <QSettings>

#include "core/account.h"

Audio::Audio(QObject *parent)
	: QObject(parent), audio(0), m_account(0)
{
}

void Audio::play(SoundType soundType)
{
	if (m_account && !m_account->settings()->value("Sounds/Enable", true).toBool())
		return;

	if (!sounds.contains(soundType))
	{
		if (soundType != STOtprav)
			sounds.insert(soundType);
		audio = new SoundPlayer();
		connect(audio, SIGNAL(finished(SoundType)), this, SLOT(stop(SoundType)));
		qDebug() << "playing sound";
		audio->playSound(soundType);
	}
}

void Audio::stop(SoundType soundType)
{
	qDebug() << "removing current sound type";
	sounds.remove(soundType);
}

void Audio::setAccount(Account* account)
{
	m_account = account;
}
