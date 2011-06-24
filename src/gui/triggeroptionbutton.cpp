#include "triggeroptionbutton.h"

#include "resourcemanager.h"

TriggerOptionButton::TriggerOptionButton(const QString& iconPrefix, const QString& optionName, QWidget* parent)
	: QToolButton(parent), m_optionName(optionName)
{
	QIcon icon;

	QPixmap pm(":/icons/" + iconPrefix + "_d.png");
	QSize iconSize = pm.size();

	icon.addPixmap(pm, QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_p.png"), QIcon::Normal, QIcon::On);
	icon.addPixmap(QPixmap(":/icons/" + iconPrefix + "_ph.png"), QIcon::Active, QIcon::On);

	setIcon(icon);
	setIconSize(iconSize);

	setCheckable(true);
	setAutoRaise(true);
	setStyleSheet("QToolButton { border: 0px; padding: 0px }");

	setChecked(theRM.settings()->value(optionName, false).toBool());

	connect(this, SIGNAL(toggled(bool)), this, SLOT(updateSettings(bool)));
}

void TriggerOptionButton::updateSettings(bool enableOption)
{
	theRM.settings()->setValue(m_optionName, enableOption);
}
