#ifndef CONTACTLISTCONFERENCE_H
#define CONTACTLISTCONFERENCE_H

#include <QListView>

class ContactListConference : public QListView
{
	Q_OBJECT
public:
	ContactListConference(QWidget* parent = 0);
	~ContactListConference();
};

#endif // CONTACTLISTCONFERENCE_H
