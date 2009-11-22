#include "avatarboxwithhandle.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QStyle>

#include "avatarbox.h"

AvatarBoxWithHandle::AvatarBoxWithHandle(const QString & avatarsPath, const QString & email, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);

	QVBoxLayout* avatarLayout = new QVBoxLayout;
	avatarBox = new AvatarBox(avatarsPath, email);
	
	avatarLayout->addWidget(avatarBox);
	avatarLayout->addStretch();

	QVBoxLayout* handleLayout = new QVBoxLayout;
	
	QIcon handleIcon;
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_left.png"), QIcon::Normal, QIcon::Off);
	handleIcon.addPixmap(QPixmap(":icons/chatwindow/avatar_button_right.png"), QIcon::Normal, QIcon::On);

	handleButton = new QToolButton;
	handleButton->setIcon(handleIcon);
	handleButton->setCheckable(true);
	connect(handleButton, SIGNAL(clicked(bool)), this, SLOT(toggleAvatarBox(bool)));
	avatarBox->setVisible(false);
	handleButton->setAutoRaise(true);
	int d = handleButton->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	handleButton->setFixedSize((handleButton->icon().actualSize(QSize(16, 16)) + QSize(2*d, 2*d)));

	handleLayout->addWidget(handleButton);
	handleLayout->addStretch();

	layout->addLayout(avatarLayout);
	layout->addLayout(handleLayout);

	setLayout(layout);
}

void AvatarBoxWithHandle::toggleAvatarBox(bool checked)
{
	qDebug() << "AvatarBoxWithHandle::toggleAvatarBox checked = " << checked;
	avatarBox->setVisible(checked);
	emit toggled(checked);
}

void AvatarBoxWithHandle::toggle(bool visible)
{
	avatarBox->setVisible(visible);
	handleButton->setChecked(visible);
}
