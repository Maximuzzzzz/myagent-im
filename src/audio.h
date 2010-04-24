#ifndef AUDIO_H
#define AUDIO_H

#include <QSet>
#include <QObject>
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>

enum SoundType
{
	STAuth,
	STLetter,
	STMessage,
	STOtprav,
	STRing,
};

class MediaObject : public Phonon::MediaObject
{
Q_OBJECT
public:
	MediaObject(SoundType st);
	~MediaObject();

private:
	SoundType soundType;

signals:
	void finishedPlaying(SoundType st);

private slots:
	void finish();
};

class Audio : public QObject
{
Q_OBJECT
public:
	static Audio& self();

	Audio();
	~Audio();
	void play(SoundType soundType);

private:
	Phonon::AudioOutput* output;
	Phonon::MediaObject* media;
	QString soundDescription(SoundType soundType);
	QSet<SoundType> sounds;

private slots:
	void finish(SoundType soundType);
};

#define audio Audio::self()

#endif
