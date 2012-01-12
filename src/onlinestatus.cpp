#include "onlinestatus.h"

#include <QDebug>
#include <QTextCodec>

#include "protocol/mrim/proto.h"
#include "resourcemanager.h"

const OnlineStatus OnlineStatus::unknown = OnlineStatus("status_gray");
const OnlineStatus OnlineStatus::offline = OnlineStatus("status_0");
const OnlineStatus OnlineStatus::online = OnlineStatus("status_1");
const OnlineStatus OnlineStatus::away = OnlineStatus("status_2");
const OnlineStatus OnlineStatus::invisible = OnlineStatus("status_3");
const OnlineStatus OnlineStatus::connecting = OnlineStatus("status_connecting");
const OnlineStatus OnlineStatus::unauthorized = OnlineStatus("status_gray");

const OnlineStatus OnlineStatus::otherOnline = OnlineStatus("");
const OnlineStatus OnlineStatus::wrongData = OnlineStatus("wrong_data");

static QList<QByteArray> getDefIdStatuses()
{
	static QList<QByteArray> defaultIdStatuses;
	if (defaultIdStatuses.isEmpty())
	{
		defaultIdStatuses << "status_22"
				  << "status_17"
				  << "status_21"
				  << "status_14"
				  << "status_27"
				  << "status_23"
				  << "status_5"
				  << "status_6"
				  << "status_4"
				  << "status_52";
	}

	return defaultIdStatuses;
}


QByteArray OnlineStatus::getDefIdStatus(int at)
{
	QList<QByteArray> defaultIdStatuses = getDefIdStatuses();

	return (defaultIdStatuses.size() > at) ? defaultIdStatuses.at(at) : "";
}

QString OnlineStatus::getDefDescrStatus(int at)
{
	static QStringList defaultDescrStatuses;
	if (defaultDescrStatuses.isEmpty())
	{
		defaultDescrStatuses << OnlineStatus::tr("Working")
				     << OnlineStatus::tr("Smoking")
				     << OnlineStatus::tr("Coffee")
				     << OnlineStatus::tr("In love")
				     << OnlineStatus::tr("Education")
				     << OnlineStatus::tr("Dreaming")
				     << OnlineStatus::tr("Home")
				     << OnlineStatus::tr("Breakfast")
				     << OnlineStatus::tr("Sick")
				     << OnlineStatus::tr("All people are alike, only I'm a star");
	}

	return (defaultDescrStatuses.size() > at) ? defaultDescrStatuses.at(at) : "";
}

OnlineStatus::OnlineStatus()
	: m_type(Null)
{
}

OnlineStatus::OnlineStatus(QByteArray id, QString description)
	: m_id(id), m_statusDescr(description)
{
	if (m_id.isEmpty())
		m_type = Null;
	else if (m_id == "status_0" || m_id == "STATUS_OFFLINE")
		m_type = Offline;
	else if (m_id == "status_1" || m_id == "STATUS_ONLINE")
		m_type = Online;
	else if (m_id == "status_2" || m_id == "STATUS_AWAY")
		m_type = Away;
	else if (m_id == "status_3" || m_id == "STATUS_INVISIBLE")
		m_type = Invisible;
	else if (m_id == "status_connecting")
		m_type = Connecting;
	else if (m_id == "status_gray")
		m_type = Unknown;
	else if (theRM.onlineStatuses()->getOnlineStatusInfo(m_id) == 0)
	{
		m_id = "wrong_data";
		m_statusDescr = tr("Wrong status data");
		m_type = OtherOnline;
	}
	else
		m_type = OtherOnline;

	if (m_type == OtherOnline && m_statusDescr.isEmpty())
	{
		if (m_id == "status_chat")
			m_statusDescr = tr("Free for chat");
		else if (m_id == "status_dnd")
			m_statusDescr = tr("Do not disturb");
		else if (m_id == "status_dating")
			m_statusDescr = tr("Get acquainted");
		else
		{
			int i = getDefIdStatuses().indexOf(m_id);
			if (i != -1)
				m_statusDescr = getDefDescrStatus(i);
			else
				m_statusDescr = tr("Extended");
		}
	}
}

QByteArray OnlineStatus::id() const
{
	if (m_type == OtherOnline)
		return m_id;
	else switch (m_type)
	{
	case Unknown:      return "status_gray";
	case Offline:      return "status_0";
	case Online:       return "status_1";
	case Away:         return "status_2";
	case Invisible:    return "status_3";
	case Connecting:   return "status_connecting";
	case Unauthorized: return "wrong_data";
	default: return QByteArray();
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
			return m_statusDescr;
		case Connecting:
			return tr("Connecting");
		default:
			return tr("Unknown");
	}
}

QIcon OnlineStatus::statusIcon() const
{
	const OnlineStatusInfo* statusInfo = theRM.onlineStatuses()->getOnlineStatusInfo(id());

	if (statusInfo)
		return QIcon(theRM.statusesResourcePrefix() + ':' + statusInfo->icon());
	else
	{
		qDebug() << Q_FUNC_INFO << "can't find status info for status:"
			 << QString("(%1, %2, %3)").arg(m_type).arg(QString::fromLatin1(m_id)).arg(m_statusDescr);
		return QIcon();
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

bool OnlineStatus::operator ==(OnlineStatus another) const
{
	return (m_type == another.m_type) && (m_id == another.m_id) && (m_statusDescr == another.m_statusDescr);
}

bool OnlineStatus::operator !=(OnlineStatus another) const
{
	return !(*this == another);
}

bool OnlineStatus::operator<(OnlineStatus another) const
{
	if (m_type < another.m_type)
		return true;
	else if (m_type > another.m_type)
		return false;
	else if (m_id < another.m_id)
		return true;
	else if (m_id > another.m_id)
		return false;
	else
		return m_statusDescr < another.m_statusDescr;
}

bool OnlineStatus::builtIn() const
{
	const OnlineStatusInfo* statusInfo = theRM.onlineStatuses()->getOnlineStatusInfo(id());
	if (statusInfo)
		return statusInfo->builtIn() == "1";
	else
		return false;
}

bool OnlineStatus::available() const
{
	const OnlineStatusInfo* statusInfo = theRM.onlineStatuses()->getOnlineStatusInfo(id());
	if (statusInfo)
		return statusInfo->available() == "1";
	else
		return false;
}
