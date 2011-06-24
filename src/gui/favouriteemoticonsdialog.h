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
signals:
	void doubleClicked(QString id);
private slots:
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
private slots:
	void emoticonDraggingStarted();
private:
	QRect cursorRect;
};

#endif
