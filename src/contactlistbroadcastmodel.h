#ifndef CONTACTLISTBROADCASTMODEL_H
#define CONTACTLISTBROADCASTMODEL_H

#include <QStandardItemModel>

class ContactList;
class ContactListItem;
class Contact;

class ContactListBroadcastModel : public QStandardItemModel
{
Q_OBJECT
public:
	ContactListBroadcastModel(QByteArray contactEmail, ContactList* contactList = NULL);
	~ContactListBroadcastModel();

	Contact* contactFromIndex(const QModelIndex& index);
	QList<QByteArray> broadcastList();

private slots:
	void rebuild();
	void addContact(Contact* c);
	void slotRemoveContactItem(Contact* c);

private:
	ContactList* contactList;
	QMap<Contact*, ContactListItem*> contactsMap;
	QByteArray m_contactEmail;
};
#endif // CONTACTLISTBROADCASTMODEL_H
