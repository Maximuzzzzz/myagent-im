#ifndef CONTACTLISTCONFERENCEWITHHANDLE_H
#define CONTACTLISTCONFERENCEWITHHANDLE_H

#include <QWidget>

class QToolButton;
class QLabel;

class Contact;
class Account;
class ContactListConference;

class ContactListConferenceWithHandle : public QWidget
{
	Q_OBJECT
public:
	ContactListConferenceWithHandle(Contact* conference, Account* acc, QWidget* parent = 0);

	void toggle(bool visible);

Q_SIGNALS:
	void toggled(bool);

private Q_SLOTS:
	void toggleConferenceList(bool checked);
	void setMembersCount(quint32 cnt);

private:
	ContactListConference* contactListConference;
	QToolButton* handleButton;
	QLabel* members;
	QLabel* iconLabel;
	QLabel* addMembersLabel;
};

#endif // CONTACTLISTCONFERENCEWITHHANDLE_H
