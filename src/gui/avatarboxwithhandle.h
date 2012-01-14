#ifndef AVATARBOXWITHHANDLE_H
#define AVATARBOXWITHHANDLE_H

#include <QWidget>

class AvatarBox;
class QToolButton;

class AvatarBoxWithHandle : public QWidget
{
Q_OBJECT
public:
	AvatarBoxWithHandle(const QString& avatarsPath, const QString & email, QWidget* parent = 0);

	void toggle(bool visible);
Q_SIGNALS:
	void toggled(bool);

private Q_SLOTS:
	void toggleAvatarBox(bool);
private:
	AvatarBox* avatarBox;
	QToolButton* handleButton;
};

#endif
