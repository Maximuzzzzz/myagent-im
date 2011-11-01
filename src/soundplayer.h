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
	STConference
};

class SoundPlayer : public QObject
{
Q_OBJECT
public:
	SoundPlayer(QObject* parent = 0);
	~SoundPlayer();
	void playSound(SoundType soundType);

Q_SIGNALS:
	void finished(SoundType soundType);

private:
	QList<SoundType> sounds;
	Phonon::MediaObject* media;
	Phonon::AudioOutput output;
	QString soundDescription(SoundType soundType);
	SoundType currentSound;

private Q_SLOTS:
	void finish();
};

#endif
