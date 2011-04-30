#ifndef CONTACTLISTBROADCAST_H
#define CONTACTLISTBROADCAST_H

#include <QWidget>
#include <QStandardItemModel>
#include <QListView>

#include "contactlist.h"

class ContactListBroadcast : public QListView
{
	Q_OBJECT
public:
	ContactListBroadcast(QByteArray contactEmail, ContactList* cl, QWidget* parent = 0);
	~ContactListBroadcast();

	QList<QByteArray> & receivers() { return broadcastModel->broadcastList(); }

private:
	ContactList* m_contactList;
	ContactListBroadcastModel* broadcastModel;
};

#endif // CONTACTLISTBROADCAST_H
