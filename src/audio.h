#ifndef AUDIO_H
#define AUDIO_H

#include "soundplayer.h"

#include <QSet>

class Audio: QObject
{
Q_OBJECT
public:
	void play(SoundType soundType);

public slots:
	void stop(SoundType soundType);

private:
	QSet<SoundType> sounds;
	SoundPlayer* audio;
};

#endif
