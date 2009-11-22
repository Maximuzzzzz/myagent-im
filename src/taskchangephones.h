#ifndef TASKSTASKCHANGEPHONES_H
#define TASKSTASKCHANGEPHONES_H

#include "simpleblockingtask.h"

#include <QStringList>

class Contact;

namespace Tasks
{

class ChangePhones : public SimpleBlockingTask<Tasks::ChangePhones>
{
Q_OBJECT
public:
	ChangePhones(Contact* contact, const QStringList& phones, MRIMClient* client, QObject* parent = 0);
	
	bool exec();
	
	QStringList phones() const { return m_phones; }

private slots:
	void checkResult(quint32 msgseq, quint32 status);

private:
	Contact* m_contact;
	QStringList m_phones;
};

}

#endif
