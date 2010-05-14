#include "audio.h"

#include <QDebug>

void Audio::play(SoundType soundType)
{
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
