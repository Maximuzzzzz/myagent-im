#ifndef IFLASHPLAYER_H
#define IFLASHPLAYER_H

class QString;
class QRect;

class IFlashPlayer
{
public:
	virtual ~IFlashPlayer() {}

	virtual void play(const QString& filePath, const QRect& geometry) = 0;
};

#endif // IFLASHPLAYER_H
