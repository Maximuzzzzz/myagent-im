#ifndef CONFERENCELISTMODEL_H
#define CONFERENCELISTMODEL_H

#include <QStandardItemModel>

class Contact;
class ContactListItem;

class ConferenceListModel : public QStandardItemModel
{
Q_OBJECT
public:
	ConferenceListModel();
	~ConferenceListModel();

public Q_SLOTS:
	void addContact(QByteArray & email);
	void slotRemoveContactItem(Contact* c);

private:
	QMap<Contact*, ContactListItem*> contactsMap;
	QByteArray m_contactEmail;

};

#endif // CONFERENCELISTMODEL_H
