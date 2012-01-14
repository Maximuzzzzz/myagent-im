#ifndef POPUPWINDOWSSTACK_H
#define POPUPWINDOWSSTACK_H

#include <QObject>

#include "gui/popupwindow.h"

class SystemTrayIcon;
class Contact;

class PopupWindowsStack : public QObject
{
Q_OBJECT
public:
	enum Direction
	{
		Up,
		Down
	};

	PopupWindowsStack(SystemTrayIcon* sysTray);
	~PopupWindowsStack();

public Q_SLOTS:
	void showNewMessage(Contact * from, const QString & to, const QDateTime& dateTime);
	void showNewLetter(const QString & from, const QString & subject, const QDateTime& dateTime);
	void showLettersUnread(const quint32 cnt);

	void deleteAllWindows();
	void showAllUnclosedWindows();
	void closeAllUnclosedWindows();

Q_SIGNALS:
	void mouseEntered();
	void mouseLeaved();
	void popupWindowActivated(PopupWindow::Type type);
	void messageActivated(const QByteArray& email);
	void allPopupWindowsRemoved();

private Q_SLOTS:
	void retranslateWindowsGeometry();
	void windowClosed();
	void slotPopupWindowActivated();

	PopupWindow* newWindow();
	QRect newCoords(quint32 n = 0);

private:
	SystemTrayIcon* m_sysTray;
	QList<PopupWindow*> existingWindows;
	QList<PopupWindow*> shownWindows;
	Direction direction;
};

#endif // POPUPWINDOWSSTACK_H
