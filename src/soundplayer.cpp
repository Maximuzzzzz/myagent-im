#include <QDebug>

#include "resourcemanager.h"
#include "soundplayer.h"

SoundPlayer::SoundPlayer()
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

	qDebug() << "File: " << (theRM.soundsResourcePrefix() + ":" + mediaFile + ".ogg");
	if (mediaFile == "")
		qDebug() << "Error: Media file not found";
	else
		media->setCurrentSource(Phonon::MediaSource(theRM.soundsResourcePrefix() + ":" + mediaFile + ".ogg"));
	Phonon::createPath(media, &output);
	connect(media, SIGNAL(finished()), this, SLOT(finish()));
	media->play();
}

void SoundPlayer::finish()
{
	qDebug() << "Emitting \"finished\"";
	deleteLater();
	emit finished(currentSound);
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
