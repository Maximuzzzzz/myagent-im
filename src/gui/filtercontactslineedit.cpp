#include "filtercontactslineedit.h"

#include <QLabel>

FilterContactsLineEdit::FilterContactsLineEdit(QWidget* parent)
	: LineEdit(parent)
{
	QLabel* searchIconLabel = new QLabel(this);
	searchIconLabel->setPixmap(QPixmap(":icons/search_icon.png"));

	addWidget(searchIconLabel, LeftSide);

	setInactiveText(tr("Filter contacts"));
}
