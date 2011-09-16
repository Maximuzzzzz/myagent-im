#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QTimer>
#include <QTextBrowser>
#include <QDateTime>
#include <QEvent>

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

	PopupWindow(QRect position, QWidget *parent = 0);
	~PopupWindow();
	void setUnreadLettersText(const quint32 letters);
	void setLetterReceived(const QString & from, const QString & subject, const QDateTime dateTime);
	void setMessageReceived(const QByteArray & fromEmail, const QString & fromNick, const QString & to, const QDateTime dateTime);

	bool isClosed() { return m_closed; }
	void setNotToClose(bool really) { m_notToClose = really; }
	Type type() { return m_type; }

	QByteArray & from() { return m_from; }

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
