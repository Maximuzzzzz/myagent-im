#ifndef CONTACTLISTCONFERENCE_H
#define CONTACTLISTCONFERENCE_H

#include <QListView>

#include "onlinestatus.h"

class Contact;
class Account;
class ConferenceListModel;

class ContactListConference : public QListView
{
	Q_OBJECT
public:
	ContactListConference(Contact* conference, Account* acc, QWidget* parent = 0);
	~ContactListConference();

Q_SIGNALS:
	void setMembersCount(quint32);

private Q_SLOTS:
	void addContact(const QByteArray& contact);
	void onlineStatusChanged(OnlineStatus);

private:
	Account* m_account;
	Contact* m_conf;
	bool accountWasConnected;

	ConferenceListModel* m_model;
	QList<Contact*> m_contacts;

	quint32 membersCount;
};

#endif // CONTACTLISTCONFERENCE_H
