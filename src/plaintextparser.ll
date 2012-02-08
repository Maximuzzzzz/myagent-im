%{

#define TXT		1
#define IMG		2
#define SMILE	3
#define URL		4
#define S_TXT	5

%}

%option nounput
%option nostack
%option prefix="plaintextparser"
%option noyywrap

%%

"<SMILE>"[ ]*"id"[ ]*"="[ ]*[0-9]+[^<]*"</SMILE>"	{ return SMILE; }
"<smile>"[ ]*"id"[ ]*"="[ ]*[0-9]+[^<]*"</smile>"	{ return SMILE; }
"<##"[^>]+">"					{ return IMG; }
"http://"[^ \r\n\t\\]+			{ return URL; }
[^ \\{}\r\n\t<]+				{ return TXT; }
[ \r\n\t]+						{ return TXT; }
[<]+							{ return TXT; }
.								{ return S_TXT; }
%%

#include "plaintextparser.h"

#include <QDebug>
#include <QTextCodec>
#include <QTextCharFormat>
#include <QUrl>

#include "resourcemanager.h"
#include "core/emoticons.h"
#include "emoticonformat.h"

PlainTextParser::PlainTextParser()
{
	codec = QTextCodec::codecForName("cp1251");
}

PlainTextParser::~PlainTextParser()
{
}

void PlainTextParser::setImage(const char* str)
{
	QString id(str);
	const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(id);
	if (!info)
	{
		qDebug() << "PlainTextParser::setImage: insert text " << id;
		cursor.insertText(id);
	}
	else
	{
		EmoticonFormat fmt(cursor.charFormat(), id);
		cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
	}
}

void PlainTextParser::setSmile(const char* str)
{
	qDebug() << "PlainTextParser::setSmile";
	QString smile = codec->toUnicode(str);
	qDebug() << "text = " << smile;
	int index = smile.indexOf('=');
	index++;
	while (smile.at(index).isSpace()) index++;
	int pos = index;
	while (smile.at(index).isDigit()) index++;

	QString id = smile.mid(pos, index - pos);
	const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(id);
	if (!info)
	{
		qDebug() << "PlainTextParser::setSmile: insert text " << smile;
		cursor.insertText(smile);
	}
	else
	{
		EmoticonFormat fmt(cursor.charFormat(), id);
		cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
	}
}

void PlainTextParser::setUrl(const char* str)
{
	qDebug() << "PlainTextParser::setUrl";
	
	QString text = codec->toUnicode(str);
	qDebug() << "url = " << text;
	QUrl url(text);
	if (!url.isValid())
	{
		qDebug() << "url is invalid";
		setText(str);
		return;
	}
	QTextCharFormat cf = cursor.charFormat();
	cursor.insertHtml("<a href=\"" + text + "\">" + text + "</a>");
	cursor.setCharFormat(cf);
}

void PlainTextParser::setText(const char* str)
{
	QString text = codec->toUnicode(str);
	//qDebug() << "insert text " << text;
	cursor.insertText(text);
}

void PlainTextParser::parse(const QString& text, QTextDocument* doc)
{
	QByteArray baText = codec->fromUnicode(text);

	cursor = QTextCursor(doc);
	
	YY_BUFFER_STATE yy_current_buffer = yy_scan_bytes(baText.data(), baText.size());
	for (;;)
	{
		int res = yylex();
		if (!res) break;
		
		switch (res)
		{
		case IMG:
			setImage(yytext);
			break;
		case SMILE:
			setSmile(yytext);
			break;
		case TXT:
			setText(yytext);
			break;
		case URL:
			setUrl(yytext);
			break;
		}
	}
	
	yy_delete_buffer(yy_current_buffer);
	yy_current_buffer = NULL;
}
