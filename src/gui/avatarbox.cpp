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

#include "avatarbox.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "gui/avatarwidget.h"
#include "gui/linkbutton.h"

AvatarBox::AvatarBox(const QString& avatarsPath, const QString & email, QWidget * parent)
	: QGroupBox(parent), m_avatarsPath(avatarsPath), m_email(email)
{
	qDebug() << "AvatarBox::AvatarBox";
	QVBoxLayout* layout = new QVBoxLayout;
	
	avatarWidget = new AvatarWidget;
	
	QString username = email.section('@', 0, 0);
	QString domain = email.section('@', 1, 1).section('.', 0, 0);

	QHBoxLayout* projectsLayout = new QHBoxLayout;
	projectsLayout->setSpacing(4);

	LinkButton* my = new LinkButton("http://r.mail.ru/cln3587/my.mail.ru/" + domain + "/" + username, linkIcon("my"));
	LinkButton* photo = new LinkButton("http://r.mail.ru/cln3565/foto.mail.ru/" + domain + "/" + username, linkIcon("foto"));
	LinkButton* video = new LinkButton("http://r.mail.ru/cln3567/video.mail.ru/" + domain + "/" + username, linkIcon("video"));
	LinkButton* blog = new LinkButton("http://r.mail.ru/cln3566/blogs.mail.ru/" + domain + "/" + username, linkIcon("blogs"));

	projectsLayout->addWidget(my);
	projectsLayout->addWidget(photo);
	projectsLayout->addWidget(video);
	projectsLayout->addWidget(blog);
	
	layout->addWidget(avatarWidget);
	layout->addLayout(projectsLayout);
	
	setLayout(layout);
	setFixedSize(sizeHint());
}

QIcon AvatarBox::linkIcon(const QString &linkType)
{
	QIcon icon;
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_d.png"), QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":icons/projects/main_top_" + linkType + "_p.png"), QIcon::Active, QIcon::On);
	
	return icon;
}

void AvatarBox::showEvent(QShowEvent * event)
{
	qDebug() << "AvatarBox::showEvent";
	if (avatarWidget->isNull())
		avatarWidget->load(m_avatarsPath, m_email);
	
	QGroupBox::showEvent(event);
}
