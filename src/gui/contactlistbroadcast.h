#ifndef CONTACTLISTBROADCAST_H
#define CONTACTLISTBROADCAST_H

#include <QListView>

class ContactList;
class ContactListBroadcastModel;

class ContactListBroadcast : public QListView
{
	Q_OBJECT
public:
	ContactListBroadcast(QByteArray contactEmail, ContactList* cl, QWidget* parent = 0);
	~ContactListBroadcast();

	QList<QByteArray> & receivers();

private:
	ContactList* m_contactList;
	ContactListBroadcastModel* broadcastModel;
};

#endif // CONTACTLISTBROADCAST_H
