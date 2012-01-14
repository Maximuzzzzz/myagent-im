#include "audio.h"

#include <QDebug>
#include <QSettings>

#include "account.h"

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
