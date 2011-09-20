#ifndef SWFDECFLASHPLAYER_H
#define SWFDECFLASHPLAYER_H

#include <QObject>

#include "iflashplayer.h"

class SwfdecQtWidget;
class SwfdecQtPlayer;

class SwfdecFlashPlayer : public QObject, public IFlashPlayer
{
public:
	SwfdecFlashPlayer(QObject* parent = 0);

	void play(const QString& filePath, const QRect& geometry);

private Q_SLOTS:
	void multSignal(QString name);

private:
	SwfdecQtWidget* playerWidget;
	SwfdecQtPlayer* player;

	quint32 playerSteps;
	quint32 maxPlayerSteps;
};

#endif // SWFDECPLAYER_H
