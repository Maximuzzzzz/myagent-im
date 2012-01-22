/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QTextBrowser>

class NotifyTextBrowser : public QTextBrowser
{
Q_OBJECT
public:
	NotifyTextBrowser(QWidget* parent = 0);
	~NotifyTextBrowser();

Q_SIGNALS:
	void mouseEntered();
	void mouseLeaved();

protected:
	virtual bool event(QEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);

};

class PopupWindow : public QWidget
{
Q_OBJECT
	friend class NotifyTextBrowser;

public:
	enum Type
	{
		None,
		LettersUnread,
		NewLetter,
		NewMessage
	};

	explicit PopupWindow(QRect position, QWidget *parent = 0);
	~PopupWindow();

	void setUnreadLettersText(const quint32 letters);
	void setLetterReceived(const QString & from, const QString & subject, const QDateTime& dateTime);
	void setMessageReceived(const QByteArray& fromEmail, const QString& fromNick, const QString& to, const QDateTime& dateTime);

	bool isClosed() { return m_closed; }
	void setNotToClose(bool really) { m_notToClose = really; }
	Type type() { return m_type; }

	QByteArray from() const { return m_from; }

Q_SIGNALS:
	void closePopupWindow();
	void mouseEntered();
	void mouseLeaved();
	void activated();

protected:
/*	bool eventFilter(QObject *obj, QEvent *event);*/

private Q_SLOTS:
	void closeWindow();

private:
	NotifyTextBrowser* textBrowser;
	bool m_closed;
	bool m_notToClose;
	QTimer timer;
	Type m_type;
	QByteArray m_from;
};

#endif // POPUPWINDOW_H
