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
	explicit ContactListBroadcastModel(QByteArray contactEmail, ContactList* contactList = 0, QObject* parent = 0);
	~ContactListBroadcastModel();

	Contact* contactFromIndex(const QModelIndex& index);
	QList<QByteArray> & broadcastList();

private Q_SLOTS:
	void rebuild();
	void addContact(Contact* c);
	void slotRemoveContactItem(Contact* c);

private:
	ContactList* contactList;
	QMap<Contact*, ContactListItem*> contactsMap;
	QByteArray m_contactEmail;
	QList<QByteArray> res;
};
#endif // CONTACTLISTBROADCASTMODEL_H
