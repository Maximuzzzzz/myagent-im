#include "audio.h"
#include "resourcemanager.h"
#include <QDebug>
#include <Phonon/MediaObject>

MediaObject::MediaObject(SoundType st)
{
	qDebug() << "MediaObject::MediaObject()";
	soundType = st;
	connect(this, SIGNAL(finished()), this, SLOT(finish()));
}

MediaObject::~MediaObject()
{
	qDebug() << "MediaObject::~MediaObject()";
}

void MediaObject::finish()
{
	qDebug() << "Emitting \"finished\"";
	emit finishedPlaying(soundType);
	deleteLater();
}

//---------------------------------------------------------------------------------------------

Audio::Audio()
{
	output = new Phonon::AudioOutput();
}

Audio::~Audio()
{
}

Audio & Audio::self()
{
	static Audio a;
	return a;
}

void Audio::play(SoundType soundType)
{
	qDebug() << "Audio::playSound(\"" + soundDescription(soundType) + ".ogg\")";

	if (!sounds.contains(soundType))
	{
		sounds.insert(soundType);
		media = new MediaObject(soundType);
		connect(media, SIGNAL(finishedPlaying(SoundType)), this, SLOT(finish(SoundType)));
		QString mediaFile = soundDescription(soundType);
		
		qDebug() << "Playing sound:" + theRM.getSoundsPath() + "/" + mediaFile + ".ogg";
		if (mediaFile == "")
			qDebug() << "Error: Media file not found";
		else
			media->setCurrentSource(Phonon::MediaSource(theRM.getSoundsPath() + "/" + mediaFile + ".ogg"));
		Phonon::createPath(media, output);
		media->play();
	}
}

void Audio::finish(SoundType soundType)
{
	qDebug() << "Finishing";
	sounds.remove(soundType);
}

QString Audio::soundDescription(SoundType soundType)
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
		default:
			return "";
	}
}
