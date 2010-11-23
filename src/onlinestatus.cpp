#include "onlinestatus.h"

#include <QDebug>
#include <QTextCodec>

#include "proto.h"
#include "resourcemanager.h"

const OnlineStatus OnlineStatus::unknown = OnlineStatus("status_gray");
const OnlineStatus OnlineStatus::offline = OnlineStatus("status_0");
const OnlineStatus OnlineStatus::invisible = OnlineStatus("status_3");
const OnlineStatus OnlineStatus::away = OnlineStatus("status_2");
const OnlineStatus OnlineStatus::online = OnlineStatus("status_1");
const OnlineStatus OnlineStatus::connecting = OnlineStatus("status_connecting");
const OnlineStatus OnlineStatus::unauthorized = OnlineStatus("status_gray");
const OnlineStatus OnlineStatus::chatOnline = OnlineStatus("status_chat");
const OnlineStatus OnlineStatus::dndOnline = OnlineStatus("status_dnd");
const OnlineStatus OnlineStatus::otherOnline = OnlineStatus("");
const OnlineStatus OnlineStatus::wrongData = OnlineStatus("wrong_data");

OnlineStatus::OnlineStatus(QByteArray idStatus, QString statusDescr)
	: m_idStatus(idStatus), m_statusDescr(statusDescr)
{
	m_defaultIdStatuses << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
	m_defaultDescrStatuses << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
	m_onlineStatuses = theRM.onlineStatuses();
	setMType();
	if (statusDescr != "" || m_type == OtherOnline)
		if (idStatus == "status_chat")
			m_statusDescr = tr("Ready to talk");
		else if (idStatus == "status_dnd")
			m_statusDescr = tr("Don't disturb");
		else
			m_statusDescr = statusDescr;
	else
	{
		m_statusDescr = description();
		qDebug() << m_statusDescr << description();
	}
}

QString OnlineStatus::description() const
{
	switch (m_type)
	{
		case Offline:
			return tr("Offline");
		case Invisible:
			return tr("Invisible");
		case Away:
			return tr("Away");
		case Online:
			return tr("Online");
		case OtherOnline:
			return tr("Extended");
		case Connecting:
			return tr("Connecting");
		default:
			return tr("Unknown");
	}
}

QIcon OnlineStatus::statusIcon() const
{
	switch (m_type)
	{
		case Unknown:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_gray")->icon());
		case Offline:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_0")->icon());
		case Invisible:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_3")->icon());
		case Away:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_2")->icon());
		case Online:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_1")->icon());
		case OtherOnline:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo(m_idStatus)->icon());
		case Connecting:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("status_connecting")->icon());
		case Unauthorized:
			return QIcon(theRM.statusesResourcePrefix() + ":" + m_onlineStatuses->getOnlineStatusInfo("wrong_data")->icon());
		default:
			return QIcon();
	}
}

void OnlineStatus::setIdStatus(QByteArray status)
{
	m_idStatus = status;
	setMType();
	if (m_type != OtherOnline)
	{
		m_statusDescr = description();
		qDebug() << m_statusDescr << description();
	}
}

quint32 OnlineStatus::protocolStatus() const
{
	switch (m_type)
	{
		case Offline:
			return STATUS_OFFLINE;
		case Invisible:
			return STATUS_FLAG_INVISIBLE | STATUS_ONLINE;
		case Away:
			return STATUS_AWAY;
		case Online:
			return STATUS_ONLINE;
		case OtherOnline:
			return STATUS_OTHER_ONLINE;
		case Connecting:
			return STATUS_UNDETERMINATED;
		default:
			return STATUS_UNDETERMINATED;
	}
}

OnlineStatus OnlineStatus::fromProtocolStatus(quint32 st)
{
	if (st & STATUS_FLAG_INVISIBLE)
		return invisible;
	else if (st == STATUS_OFFLINE)
		return offline;
	else if (st == STATUS_AWAY)
		return away;
	else if (st == STATUS_ONLINE)
		return online;
	else if (st == STATUS_OTHER_ONLINE)
		return otherOnline;
	else
	{
		qDebug() << "unknown proto status: " << st;
		return unknown;
	}
}

bool OnlineStatus::connected() const
{
	switch (m_type)
	{
	case Online:
	case Invisible:
	case Away:
	case OtherOnline:
		return true;
	default:
		return false;
	}
}

void OnlineStatus::setMType()
{
	if (m_idStatus == "")
		m_type = OtherOnline;
	else if (m_onlineStatuses->getOnlineStatusInfo(m_idStatus) == NULL)
	{
		m_idStatus = "wrong_data";
		m_statusDescr = tr("Wrong status data");
		m_type = OtherOnline;
	}
	else if (m_idStatus == "status_0" || m_idStatus == "STATUS_OFFLINE")
		m_type = Offline;
	else if (m_idStatus == "status_1" || m_idStatus == "STATUS_ONLINE")
		m_type = Online;
	else if (m_idStatus == "status_2" || m_idStatus == "STATUS_AWAY")
		m_type = Away;
	else if (m_idStatus == "status_3" || m_idStatus == "STATUS_INVISIBLE")
		m_type = Invisible;
	else if (m_idStatus == "status_connecting")
		m_type = Connecting;
	else if (m_idStatus == "status_gray")
		m_type = Unknown;
	else
		m_type = OtherOnline;
}

void OnlineStatus::setDescr(QString descr)
{
	m_statusDescr = descr;
}
