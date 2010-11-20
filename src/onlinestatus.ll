%{

enum
{
	SKIP = 1,
	STATUSES_,
	STATUSSET_BEGIN,
	STATUSSET_END,
	OBJ_BEGIN,
	OBJ_END,
	STRING,
	ATTR
};

%}

%option nounput
%option nostack
%option prefix="onlinestatus"
%option noyywrap

%s ROOT
%s STATUSES
%s ITEMS1
%s STATUSSET
%s ITEMS2
%s OBJ

%%

"<ROOT>" { BEGIN(ROOT); }
<ROOT>"</ROOT>" { BEGIN(INITIAL); }
<ROOT>"<STATUSES>" { BEGIN(STATUSES); return STATUSES_; }
<STATUSES>"</STATUSES>" { BEGIN(ROOT); }
<STATUSES>"<ITEMS".*">" { BEGIN(ITEMS1); }
<ITEMS1>"</ITEMS>" { BEGIN(STATUSES); }
<ITEMS1>"<"[^>/]+">" { BEGIN(STATUSSET); return STATUSSET_BEGIN; }
<STATUSSET>"</"[^>]+">" { BEGIN(ITEMS1); return STATUSSET_END; }
<STATUSSET>"<ITEMS>" { BEGIN(ITEMS2); }
<ITEMS2>"</ITEMS>" { BEGIN(STATUSSET); }
<ITEMS2>"<OBJ"[0-9]*">" { BEGIN(OBJ); return OBJ_BEGIN; }
<OBJ>"</OBJ"[0-9]*">" { BEGIN(ITEMS2); return OBJ_END; }
[a-z]+/[ ]*= { return ATTR; }
\".*\" { return STRING; }
.|\n { return SKIP; }

%%

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

void OnlineStatuses::load(QString filename)
{
	qDebug() << "OnlineStatuses::load" << filename;

	enum States { Initial, Status, Statusset, Obj };
	States state = Initial;

	enum Attrs { Empty, Path, Id, Icon, BuiltIn };
	Attrs attr = Empty;

	QTextCodec* codec = QTextCodec::codecForName("cp1251");

	yyin = fopen(codec->fromUnicode(filename).constData(), "r");

	if (!yyin)
		return;

	QString statusPath, statussetPath;
	OnlineStatusSet* onlineStatusSet = NULL;
	OnlineStatusInfo* onlineStatusInfo = NULL;

	for (;;)
	{
		int lexem = yylex();
		if (!lexem) break;

		switch (lexem)
		{
		case STATUSES_:
			state = Status;
			break;
		case STATUSSET_BEGIN:
			state = Statusset;
			if (onlineStatusSet)
				delete onlineStatusSet;
			onlineStatusSet = new OnlineStatusSet;
			break;
		case STATUSSET_END:
			onlineStatusSets.append(onlineStatusSet);
			onlineStatusSet = NULL;
			break;
		case OBJ_BEGIN:
			state = Obj;
			if (onlineStatusInfo)
				delete onlineStatusInfo;
			onlineStatusInfo = new OnlineStatusInfo;
			break;
		case OBJ_END:
			idToOnlineStatusMap[onlineStatusInfo->id()] = onlineStatusInfo;
			if (onlineStatusSet)
				onlineStatusSet->onlineStatusInfos.append(onlineStatusInfo);
			onlineStatusInfo = NULL;
			break;
		case STRING:
			{
				QByteArray ba(yytext);
				QString str = codec->toUnicode(yytext);
				str.replace("\\\"", "\"");
				str = str.mid(1, str.size()-2);

				if (state == Status)
				{
					if (attr == Path)
						statusPath = str;
				}
				else if (state == Statusset)
				{
					if (attr == Path)
						statussetPath = statusPath + str;
				}
				else if (state == Obj)
				{
					switch (attr)
					{
					case Id:
						onlineStatusInfo->id_ = str;
						break;
					case Icon:
						onlineStatusInfo->icon_ = statussetPath + str;
						//qDebug() << "onlineStatusInfo->icon_" << onlineStatusInfo->icon_;
						break;
					case BuiltIn:
						onlineStatusInfo->builtin_ = str;
						break;
					default:
						break;
					}
				}
			}
			break;
		case ATTR:
			{
				QByteArray attrName(yytext);
				//qDebug() << attrName;
				if (attrName == "path")
					attr = Path;
				else if (attrName == "id")
					attr = Id;
				else if (attrName == "icon")
					attr = Icon;
				else if (attrName == "builtin")
					attr = BuiltIn;
				else
					attr = Empty;
			}
			break;
		case SKIP:
			break;
		default:
			qDebug() << "default";
			break;
		}
	}
}

OnlineStatus::OnlineStatus(QByteArray idStatus, QString statusDescr)
	: m_idStatus(idStatus), m_statusDescr(statusDescr)
{
	setMType();
	if (statusDescr != "" || m_type == OtherOnline)
		if (idStatus == "status_chat")
			m_statusDescr = tr("Ready to talk");
		else if (idStatus == "status_dnd")
			m_statusDescr = tr("Don't disturb");
		else
			m_statusDescr = statusDescr;
	else
		m_statusDescr = description();
	m_onlineStatuses = theRM.onlineStatuses();
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
		m_type = Unknown;
	else if (m_idStatus == "status_0")
		m_type = Offline;
	else if (m_idStatus == "status_1")
		m_type = Online;
	else if (m_idStatus == "status_2")
		m_type = Away;
	else if (m_idStatus == "status_3")
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
