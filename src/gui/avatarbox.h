#ifndef AVATARBOX_H
#define AVATARBOX_H

#include <QGroupBox>

class AvatarWidget;

class AvatarBox : public QGroupBox
{
Q_OBJECT
public:
	AvatarBox(const QString& avatarsPath, const QString& email, QWidget* parent = 0);
protected:
	virtual void showEvent(QShowEvent* event);
private:
	QIcon linkIcon(const QString &linkType);

	AvatarWidget* avatarWidget;
	QString m_avatarsPath;
	QString m_email;
};

#endif
