#ifndef FAVOURITEEMOTICONSDIALOG_H
#define FAVOURITEEMOTICONSDIALOG_H

#include <QDialog>
#include <QFrame>

class FavouriteEmoticonsDialog : public QDialog
{
	Q_OBJECT
	class FavoritesEmoticonsWidget;
public:
	FavouriteEmoticonsDialog();
Q_SIGNALS:
	void doubleClicked(QString id);
private Q_SLOTS:
	void slotAccepted();
	void slotDoubleClicked(QString id);
private:
	FavoritesEmoticonsWidget* favouriteEmoticonsWidget;
};

class FavouriteEmoticonsDialog::FavoritesEmoticonsWidget : public QFrame
{
	Q_OBJECT
public:
	FavoritesEmoticonsWidget(int totalEmoticons);

	QStringList favouriteEmoticons() const;
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	virtual void dragLeaveEvent ( QDragLeaveEvent * event );
	virtual void dragMoveEvent(QDragMoveEvent* event);
	virtual void paintEvent(QPaintEvent* event);
private Q_SLOTS:
	void emoticonDraggingStarted();
private:
	QRect cursorRect;
};

#endif
