#include "mults.h"

#include <QDebug>
#include <QTextCodec>

/*#include "protocol/mrim/proto.h"
#include "resourcemanager.h"*/

void Mults::load(/*QString filename*/)
{
	qDebug() << Q_FUNC_INFO;
	MultSet* set = new MultSet;
	MultInfo* multInfo = NULL;

	int i;
	for (i = 0; i < 18; i++)
	{
		multInfo = new MultInfo;

		/*multInfo->alt_ = "mult_" + QString::number(i);*/
		QTextCodec* c8 = QTextCodec::codecForName("UTF8");
		/*QTextCodec* c16 = QTextCodec::codecForName("UTF16");*/
		switch (i)
		{
			case 0:
				multInfo->id_ = "flas_1";
				multInfo->fileName_ = "beer";
				multInfo->frames_ = "104";
				multInfo->alt_ = c8->toUnicode("Пивка?;)");
				break;
			case 1:
				multInfo->id_ = "flash_5";
				multInfo->fileName_ = "bodun";
				multInfo->frames_ = "136";
				multInfo->alt_ = c8->toUnicode("Отходняк...");
				break;
			case 2:
				multInfo->id_ = "flash_4";
				multInfo->fileName_ = "devochka";
				multInfo->frames_ = "77";
				multInfo->alt_ = c8->toUnicode("Хны...!");
				break;
			case 3:
				multInfo->id_ = "flash_0";
				multInfo->fileName_ = "guby";
				multInfo->frames_ = "91";
				multInfo->alt_ = c8->toUnicode("Поцелуй");
				break;
			case 4:
				multInfo->id_ = "flash_12";
				multInfo->fileName_ = "kot_cool";
				multInfo->frames_ = "160";
				multInfo->alt_ = c8->toUnicode("Круто!");
				break;
			case 5:
				multInfo->id_ = "flash_13";
				multInfo->fileName_ = "kot_goodbye";
				multInfo->frames_ = "140";
				multInfo->alt_ = c8->toUnicode("Пока-пока");
				break;
			case 6:
				multInfo->id_ = "flash_15";
				multInfo->fileName_ = "kot_nedutza";
				multInfo->frames_ = "115";
				multInfo->alt_ = c8->toUnicode("Не дуться!");
				break;
			case 7:
				multInfo->id_ = "flash_16";
				multInfo->fileName_ = "kot_obida";
				multInfo->frames_ = "110";
				multInfo->alt_ = c8->toUnicode("Не дуться!");
				break;
			case 8:
				multInfo->id_ = "flash_14";
				multInfo->fileName_ = "kot_spasibo";
				multInfo->frames_ = "116";
				multInfo->alt_ = c8->toUnicode("Спасибо!");
				break;
			case 9:
				multInfo->id_ = "flash_17";
				multInfo->fileName_ = "kot_wow";
				multInfo->frames_ = "136";
				multInfo->alt_ = c8->toUnicode("Ух ты!");
				break;
			case 10:
				multInfo->id_ = "flash_8";
				multInfo->fileName_ = "mad_dog";
				multInfo->frames_ = "156";
				multInfo->alt_ = c8->toUnicode("А я сошла с ума...");
				break;
			case 11:
				multInfo->id_ = "flash_11";
				multInfo->fileName_ = "rabotaet";
				multInfo->frames_ = "140";
				multInfo->alt_ = c8->toUnicode("Я занят");
				break;
			case 12:
				multInfo->id_ = "flash_9";
				multInfo->fileName_ = "rosy";
				multInfo->frames_ = "170";
				multInfo->alt_ = c8->toUnicode("Миллион алых роз!");
				break;
			case 13:
				multInfo->id_ = "flash_6";
				multInfo->fileName_ = "serdze";
				multInfo->frames_ = "158";
				multInfo->alt_ = c8->toUnicode("Сердце");
				break;
			case 14:
				multInfo->id_ = "flash_7";
				multInfo->fileName_ = "smeh";
				multInfo->frames_ = "212";
				multInfo->alt_ = c8->toUnicode("Жжёшь!");
				break;
			case 15:
				multInfo->id_ = "flash_3";
				multInfo->fileName_ = "sobaka";
				multInfo->frames_ = "236";
				multInfo->alt_ = c8->toUnicode("Хочу к тебе!");
				break;
			case 16:
				multInfo->id_ = "flash_10";
				multInfo->fileName_ = "sobaka_strelyaet";
				multInfo->frames_ = "146";
				multInfo->alt_ = c8->toUnicode("Пристрелю!");
				break;
			case 17:
				multInfo->id_ = "flash_2";
				multInfo->fileName_ = "zadolbal";
				multInfo->frames_ = "106";
				multInfo->alt_ = c8->toUnicode("Задолбал!");
				break;
		}
		idToMultMap[multInfo->id()] = multInfo;
		set->multInfos.append(multInfo);
		multInfo = NULL;
	}
	multSets.append(set);
	set = NULL;

/*	qDebug() << "OnlineStatuses::load" << filename;

	enum States { Initial, Status, Statusset, Obj };
	States state = Initial;

	enum Attrs { Empty, Path, Id, Icon, BuiltIn, Available };
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
					case Available:
						onlineStatusInfo->available_ = str;
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
				else if (attrName == "available")
					attr = Available;
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
	}*/

	for (const_iterator i = begin(); i != end(); ++i)
		(*i)->addMultSet(m_multIds);
}

/*int OnlineStatuses::multsCount()
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
}*/

void MultSet::addMultSet(QStringList & list)
{
	for (const_iterator i = begin(); i != end(); ++i)
		list << (*i)->id();
}
