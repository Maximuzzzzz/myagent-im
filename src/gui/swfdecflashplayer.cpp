#include "swfdecflashplayer.h"

#include "libswf/swfdecqtplayer.h"
#include "libswf/swfdecqtwidget.h"

SwfdecFlashPlayer::SwfdecFlashPlayer(QObject* parent)
	: QObject(parent)
{
	player = 0;
	playerWidget = new SwfdecQtWidget(this);
}

void SwfdecFlashPlayer::play(const QString &filePath, const QRect &geometry)
{
	/*if (player)
		delete player;*/ //TODO: Segmentation fault
	player = new SwfdecQtPlayer(filePath, QByteArray(), this);
	playerWidget->setPlayer(player);
	playerWidget->setGeometry(geometry);
	playerWidget->show();
	connect(player, SIGNAL(unknownSignal(QString)), this, SLOT(multSignal(QString)));
	playerSteps = 0;
	maxPlayerSteps = multInfo->frames().toInt();

	player->setPlaying(true);
}

void SwfdecFlashPlayer::multSignal(QString name)
{
	if (name != "next-event")
		return;

	if (++playerSteps == maxPlayerSteps)
	{
		qDebug() << "Mult finished with" << playerSteps << "frames";
		playerSteps = 0;
		player->setPlaying(false);
		playerWidget->hide();
	}
}
