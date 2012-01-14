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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "popupwindow.h"

#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QDateTime>

NotifyTextBrowser::NotifyTextBrowser(QWidget* parent)
 : QTextBrowser(parent)
{
	qDebug() << Q_FUNC_INFO;
	setMouseTracking(true);
}

NotifyTextBrowser::~NotifyTextBrowser()
{
}

bool NotifyTextBrowser::event(QEvent * event)
{
	if (event->type() == QEvent::Enter)
	{
		qobject_cast<PopupWindow*>(parent())->m_notToClose = true;
		Q_EMIT qobject_cast<PopupWindow*>(parent())->mouseEntered();
	}
	else if (event->type() == QEvent::Leave)
	{
		qobject_cast<PopupWindow*>(parent())->m_notToClose = false;
		Q_EMIT qobject_cast<PopupWindow*>(parent())->mouseLeaved();
	}
	return QTextBrowser::event(event);
}

void NotifyTextBrowser::mousePressEvent(QMouseEvent * event)
{
	if (event->type() == QEvent::MouseButtonPress)
		Q_EMIT qobject_cast<PopupWindow*>(parent())->activated();
}


PopupWindow::PopupWindow(QRect position, QWidget *parent)
	: QWidget(parent), m_type(None)
{
	qDebug() << Q_FUNC_INFO;

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);
	setAttribute(Qt::WA_QuitOnClose, false);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	setLayout(verticalLayout);

	verticalLayout->setSpacing(0);
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	textBrowser = new NotifyTextBrowser;
	textBrowser->setContentsMargins(0, 0, 0, 0);
	textBrowser->setMinimumSize(QSize(0, 50));
	textBrowser->setFocusPolicy(Qt::ClickFocus);
	textBrowser->setContextMenuPolicy(Qt::NoContextMenu);
	textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
	textBrowser->setOpenLinks(false);

	verticalLayout->addWidget(textBrowser);

	setGeometry(position);

	m_closed = false;
	m_notToClose = false;
	timer.singleShot(3000, this, SLOT(closeWindow()));
}

PopupWindow::~PopupWindow()
{
	qDebug() << Q_FUNC_INFO;
	timer.stop();
	Q_EMIT closePopupWindow();
}

void PopupWindow::closeWindow()
{
	qDebug() << Q_FUNC_INFO;
	m_closed = true;
	if (!m_notToClose)
	{
		close();
		Q_EMIT closePopupWindow();
	}
}

void PopupWindow::setUnreadLettersText(const quint32 letters)
{
	qDebug() << Q_FUNC_INFO;
	m_type = LettersUnread;
	QString html = "<table cellpadding=\"2\" border = 0><tr><td><img src=\":icons/notify/unread.png\" width=\"50\" height=\"50\" align=\"left\"><td><font size=95%><font color=#000088><strong>";
	if (letters == 1)
		html += tr("You have <nobr>%1 unread</nobr> letter").arg(letters);
	else
		html += tr("You have <nobr>%1 unread</nobr> letters").arg(letters);
	html += "</strong></font></font></table>";

	textBrowser->setHtml(html);
}

void PopupWindow::setLetterReceived(const QString & from, const QString & subject, const QDateTime &dateTime)
{
	qDebug() << Q_FUNC_INFO;
	m_type = NewLetter;
	QString html = "<table cellpadding=\"2\" border = 0><tr><td><img src=\":icons/notify/unread.png\" width=\"50\" height=\"50\" align=\"left\"><td><font size=95%><font color=#000088><strong>";
	html += from + "</strong><br>";
	if (subject.isEmpty())
		html += tr("&lt;Without subject&gt;");
	else
		html += subject;
	html += "</font><br><font color=#888888>";
	html += tr("Received at %1").arg(dateTime.toString("dd MMM yy, hh:mm"));
	html += "</font></font></table>";

	textBrowser->setHtml(html);
}

void PopupWindow::setMessageReceived(const QByteArray& fromEmail, const QString& fromNick, const QString& to, const QDateTime& dateTime)
{
	qDebug() << Q_FUNC_INFO;
	m_type = NewMessage;
	m_from = fromEmail;
	QString html = "<table bgcolor=#f9f1c8 cellpadding=0 border=0><tr><td><img src=\":icons/message_32x32.png\" width=\"32\" height=\"32\" align=\"left\" valign=\"top\"><td><img src=\":icons/message_16x16.png\" width=16 height=16 align=\"left\"><font size=95%><font color=#888888>";
	html += tr("Message from");
	html += "</font><br><font color=#ff0000><strong>" + fromNick + "</strong></font><br>";
	html += "<font color=#888888>" + tr("Received at %1").arg(dateTime.toString("dd MMM yy, hh:mm")) + "<br>";
	html += tr("for %1").arg(to) + "</font></font></table>";

	textBrowser->setHtml(html);
}
