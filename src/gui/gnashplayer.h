#ifndef GNASHPLAYER_H
#define GNASHPLAYER_H

#include <QObject>

#include "iflashplayer.h"

class GnashPlayer : public QObject, public IFlashPlayer
{
	Q_OBJECT
public:
	explicit GnashPlayer(QObject *parent = 0);

	void play(const QString& filePath, const QRect& geometry);
};

#endif // GNASHPLAYER_H
