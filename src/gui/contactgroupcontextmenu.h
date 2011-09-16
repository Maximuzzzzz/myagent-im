#ifndef CONTACTGROUPCONTEXTMENU_H
#define CONTACTGROUPCONTEXTMENU_H

#include <QMenu>

#include "onlinestatus.h"

class Account;
class ContactGroup;
class QAction;

class ContactGroupContextMenu : public QMenu
{
Q_OBJECT
public:
	ContactGroupContextMenu(Account* account, QWidget* parent = 0);
	
	void setGroup(ContactGroup* group);

private Q_SLOTS:
	void checkOnlineStatus(OnlineStatus status);

	void removeGroup();
	void removeGroupError(QString error);
	void renameGroup();
	void renameGroupError(QString error);

private:
	Account* m_account;
	ContactGroup* m_group;

	QAction* removeGroupAction;
	QAction* renameGroupAction;
};

#endif
