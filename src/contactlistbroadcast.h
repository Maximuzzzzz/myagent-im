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
	ContactListBroadcast(ContactList* cl, QWidget* parent = 0);
	~ContactListBroadcast();

private:
	ContactList* m_contactList;
};

#endif // CONTACTLISTBROADCAST_H
