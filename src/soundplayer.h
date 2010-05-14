#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QTimer>
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

class SoundPlayer : public QObject
{
Q_OBJECT
public:
	SoundPlayer();
	~SoundPlayer();
	void playSound(SoundType soundType);

signals:
	void finished(SoundType soundType);

private:
	QList<SoundType> sounds;
	Phonon::MediaObject* media;
	Phonon::AudioOutput output;
	QString soundDescription(SoundType soundType);
	SoundType currentSound;

private slots:
	void finish();
};

#endif
