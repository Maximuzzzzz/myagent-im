/***************************************************************************
 *   Copyright (C) 2008 by Alexander Volkov                                *
 *   volkov0aa@gmail.com                                                   *
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

#ifndef AVATAR_H
#define AVATAR_H

#include <QObject>
#include <QPixmap>
#include <QUrl>
#include <QHttp>
#include <QBuffer>
#include <QDateTime>

class Avatar : public QObject, public QPixmap
{
Q_OBJECT
public:
	enum State { Null, Loading, Loaded };
	
	Avatar(QObject* parent = 0);
	virtual ~Avatar();
	
	void load(const QString& dirname, const QString& email);
	
	QString email() const { return m_email; }
	State state() const { return m_state; }
	
Q_SIGNALS:
	void updated();
	
protected:
	void reset();
	void setLoading();
	void setLoaded();
	QString avatarBaseHref() const;
	virtual QString avatarHref() const;
	virtual QString emptyAvatarFilename() const;
	virtual QString loadingAvatarFilename() const;
	virtual QString imageName() const;
	
private Q_SLOTS:
	void httpDone(bool error);

private:
	void requestHttpHeader(QUrl url);
	void requestImage();
	
	QHttp m_http;
	QBuffer m_buffer;
	bool m_bRequestingHeader;
	QString m_httpPath;
	QString m_dirname;
	QDateTime m_fileDateTime;
	State m_state;
	
	QString m_email;
};

#endif
