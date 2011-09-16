#ifndef AUDIO_H
#define AUDIO_H

#include "soundplayer.h"
#include "account.h"

#include <QSet>

class Audio: QObject
{
Q_OBJECT
public:
	void play(SoundType soundType);

public Q_SLOTS:
	void stop(SoundType soundType);
	void setAccount(Account* account);

private:
	QSet<SoundType> sounds;
	SoundPlayer* audio;
	Account* m_account;
};

#endif
