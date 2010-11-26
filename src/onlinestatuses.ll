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
%option prefix="onlinestatuses"
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

#include "onlinestatuses.h"

#include <QDebug>
#include <QTextCodec>

#include "proto.h"
#include "resourcemanager.h"

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

	for (const_iterator i = begin(); i != end(); ++i)
		(*i)->addStatusesSet(m_onlineStatusIds);
}

int OnlineStatuses::statusesCount()
{
	int res = 0;
	for (const_iterator i = begin(); i != end(); ++i)
		res += (*i)->size();

	return res;
}

int OnlineStatusSet::size()
{
	int res = 0;
	for (const_iterator i = begin(); i != end(); ++i)
		if ((*i)->builtIn() != "1")
			res++;

	return res;
}

void OnlineStatusSet::addStatusesSet(QStringList & list)
{
	for (const_iterator i = begin(); i != end(); ++i)
		if ((*i)->builtIn() != "1")
			list << (*i)->id();
}
