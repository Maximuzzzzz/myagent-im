#ifndef CONTACTLISTCONFERENCEWITHHANDLE_H
#define CONTACTLISTCONFERENCEWITHHANDLE_H

#include <QWidget>

#include "contactlist.h"
#include "contactlistconference.h"

class QToolButton;

class ContactListConferenceWithHandle : public QWidget
{
	Q_OBJECT
public:
	ContactListConferenceWithHandle(ContactList* cl, QWidget* parent = 0);

	void toggle(bool visible);

signals:
	void toggled(bool);

private slots:
	void toggleConferenceList(bool checked);

private:
	ContactListConference* contactListConference;
	QToolButton* handleButton;
};

#endif // CONTACTLISTCONFERENCEWITHHANDLE_H
