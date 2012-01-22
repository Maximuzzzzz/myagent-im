/***************************************************************************
 *   Copyright (C) 2011 by Alexander Volkov <volkov0aa@gmail.com>          *
 *                                                                         *
 *   This file is part of instant messenger MyAgent-IM                     *
 *                                                                         *
 *   MyAgent-IM is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   MyAgent-IM is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

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
