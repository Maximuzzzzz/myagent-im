#include "taskchangephones.h"

#include <QDebug>

#include "mrimclient.h"

namespace Tasks
{

ChangePhones::ChangePhones(Contact* contact, const QStringList& phones, MRIMClient* client, QObject *parent)
	: SimpleBlockingTask<ChangePhones>(client, parent), m_contact(contact), m_phones(phones)
{
}

}

bool Tasks::ChangePhones::exec()
{
	if (!block())
		return false;
	
	connect(mc, SIGNAL(contactModified(quint32, quint32)), this, SLOT(checkResult(quint32, quint32)));
	
	return checkCall(mc->changeContactPhones(m_contact, m_phones));
}

void Tasks::ChangePhones::checkResult(quint32 msgseq, quint32 status)
{
	if (isMyResponse(msgseq))
	{
		emit done(status, false);
		deleteLater();
	}
}
