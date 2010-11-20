%{

enum
{
	SKIP = 1,
	SMILES_,
	SMILESET_BEGIN,
	SMILESET_END,
	OBJ_BEGIN,
	OBJ_END,
	ICON_,
	STRING,
	ATTR
};

%}

%option nounput
%option nostack
%option prefix="emoticons"
%option noyywrap

%s ROOT
%s SMILES
%s ITEMS1
%s SMILESET
%s ITEMS2
%s OBJ
%s ICON

%%

"<ROOT>" { BEGIN(ROOT); }
<ROOT>"</ROOT>" { BEGIN(INITIAL); }
<ROOT>"<SMILES>" { BEGIN(SMILES); return SMILES_; }
<SMILES>"</SMILES>" { BEGIN(ROOT); }
<SMILES>"<ITEMS".*">" { BEGIN(ITEMS1); }
<ITEMS1>"</ITEMS>" { BEGIN(SMILES); }
<ITEMS1>"<"[^>/]+">" { BEGIN(SMILESET); return SMILESET_BEGIN; }
<SMILESET>"</"[^>]+">" { BEGIN(ITEMS1); return SMILESET_END; }
<SMILESET>"<ITEMS>" { BEGIN(ITEMS2); }
<ITEMS2>"</ITEMS>" { BEGIN(SMILESET); }
<ITEMS2>"<OBJ"[0-9]*">" { BEGIN(OBJ); return OBJ_BEGIN; }
<OBJ>"</OBJ"[0-9]*">" { BEGIN(ITEMS2); return OBJ_END; }
<OBJ>"<ICON>" { BEGIN(ICON); return ICON_; }
<ICON>"</ICON>" { BEGIN(OBJ); }
[a-z]+/[ ]*= { return ATTR; }
\".*\" { return STRING; }
.|\n { return SKIP; }

%%

#include <QDebug>
#include <QTextCodec>
#include <QByteArray>
#include <QSettings>

#include "emoticons.h"
#include "resourcemanager.h"

void Emoticons::load(QString filename, QSettings* settings)
{
	enum States { Initial, Smiles, Smileset, Obj, Icon };
	States state = Initial;

	enum Attrs { Empty, Path, Id, Alt, Tip, Src, Title };
	Attrs attr = Empty;

	QTextCodec* codec = QTextCodec::codecForName("cp1251");

	yyin = fopen(codec->fromUnicode(filename).constData(), "r");

	if (!yyin)
		return;

	QString smilesPath, smilesetPath;
	EmoticonSet*  emoticonSet = NULL;
	EmoticonInfo* emoticonInfo = NULL;

	for (;;)
	{
		int lexem = yylex();
		if (!lexem) break;

		switch (lexem)
		{
		case SMILES_:
			state = Smiles;
			break;
		case SMILESET_BEGIN:
			state = Smileset;
			if (emoticonSet)
				delete emoticonSet;
			emoticonSet = new EmoticonSet;
			break;
		case SMILESET_END:
			emoticonSets.append(emoticonSet);
			emoticonSet = NULL;
			break;
		case OBJ_BEGIN:
			state = Obj;
			if (emoticonInfo)
				delete emoticonInfo;
			emoticonInfo = new EmoticonInfo;
			break;
		case OBJ_END:
			idToEmoticonMap[emoticonInfo->id()] = emoticonInfo;
			if (emoticonSet)
				emoticonSet->emoticonInfos.append(emoticonInfo);
			emoticonInfo = NULL;
			break;
		case ICON_:
			state = Icon;
			break;
		case STRING:
			{
				QByteArray ba(yytext);
				QString str = codec->toUnicode(yytext);
				str = str.mid(1, str.size()-2);
				str.replace("\\\"", "\"");

				if (state == Smiles)
				{
					if (attr == Path)
						smilesPath = str;
				}
				else if (state == Smileset)
				{
					if (attr == Path)
						smilesetPath = smilesPath + str;
					else if (attr == Title)
						emoticonSet->title_ = str;
				}
				else if (state == Obj)
				{
					switch (attr)
					{
					case Id:
						emoticonInfo->id_ = str;
						break;
					case Alt:
						emoticonInfo->alt_ = str;
						break;
					case Tip:
						emoticonInfo->tip_ = str;
						break;
					default:
						break;
					}
				}
				else if (state == Icon)
				{
					if (attr == Src)
						emoticonInfo->iconPath_ = smilesetPath + str;
				}
			}
			break;
		case ATTR:
			{
				//qDebug() << "ATTR = " << yytext;
				QByteArray attrName(yytext);
				if (attrName == "path")
					attr = Path;
				else if (attrName == "id")
					attr = Id;
				else if (attrName == "tip")
					attr = Tip;
				else if (attrName == "alt")
					attr = Alt;
				else if (attrName == "src")
					attr = Src;
				else if (attrName == "title")
					attr = Title;
				else
					attr = Empty;
			}
			break;
		case SKIP:
			//qDebug() << "SKIP = " << yytext;
			break;
		default:
			//qDebug() << "unkn lexem = " << yytext;
			break;
		}
	}

	QStringList defaultFavouriteEmoticons;
	defaultFavouriteEmoticons << "426" << "422" << "429" << "403" << "413" << "404" << "400" << "405";
	defaultFavouriteEmoticons << "408" << "417" << "409" << "427" << "407" << "411" << "430" << "420";
	defaultFavouriteEmoticons << "424" << "402" << "401" << "406" << "415" << "416" << "428" << "419";
	defaultFavouriteEmoticons << "418" << "410" << "425" << "414" << "412" << "<###20###img039>" << "<###20###img038>" << "<###20###img000>";
	defaultFavouriteEmoticons << "<###20###img009>" << "<###20###img007>" << "<###20###img016>" << "<###20###img026>" << "<###20###img015>" << "431" << "432" << "308";

	m_favouriteEmoticons = settings->value("favouriteEmoticons", defaultFavouriteEmoticons).toStringList();
	//m_favouriteEmoticons = defaultFavouriteEmoticons;

	m_maxSetSize = 0;
	for (const_iterator i = begin(); i != end(); ++i)
	{
		int setSize = (*i)->size();
		if (m_maxSetSize < setSize)
			m_maxSetSize = setSize;
	}
}

void Emoticons::setFavouriteEmoticons(const QStringList& emoticonIds)
{
	theRM.settings()->setValue("favouriteEmoticons", emoticonIds);
	m_favouriteEmoticons = emoticonIds;
}
