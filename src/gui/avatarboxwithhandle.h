#ifndef AVATARBOXWITHHANDLE_H
#define AVATARBOXWITHHANDLE_H

#include <QWidget>
#include <QIcon>

class AvatarBox;
class QToolButton;

class AvatarBoxWithHandle : public QWidget
{
Q_OBJECT
public:
	AvatarBoxWithHandle(const QString& avatarsPath, const QString & email, QWidget* parent = 0);

	void toggle(bool visible);
signals:
	void toggled(bool);

private slots:
	void toggleAvatarBox(bool);
private:
	AvatarBox* avatarBox;
	QToolButton* handleButton;
};

#endif
