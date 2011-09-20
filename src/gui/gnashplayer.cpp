#include "gnashplayer.h"

#include <QX11EmbedContainer>
#include <QProcess>

class GnashProcess : public QProcess
{
public:
	GnashProcess(QObject* parent)
		: QProcess(parent)
	{
	}

	~GnashProcess()
	{
		if (state() != QProcess::NotRunning)
		{
			terminate();
			if (!waitForFinished())
			{
				kill();
			}
		}
	}
};

GnashPlayer::GnashPlayer(QObject *parent)
	: QObject(parent)
{
}

void GnashPlayer::play(const QString &filePath, const QRect &geometry)
{
	QX11EmbedContainer* container = new QX11EmbedContainer;
	container->setAttribute(Qt::WA_DeleteOnClose);
	container->setGeometry(geometry);
	container->show();

	GnashProcess* process = new GnashProcess(container);
	connect(process, SIGNAL(finished(int)), container, SLOT(close()));

	QString executable = "klash";
	QStringList arguments;
	arguments << "-x" << QString::number(container->winId());
	arguments << "-1";
	arguments << filePath;

	process->start(executable, arguments);
	if (!process->waitForStarted())
	{
		container->close();
		return;
	}
}
