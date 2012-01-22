/***************************************************************************
 *   Copyright (C) 2008 by Alexander Volkov                                *
 *   volkov0aa@gmail.com                                                   *
 *                                                                         *
 *   This file is part of instant messenger MyAgent-IM                     *
 *                                                                         *
 *   MyAgent-IM is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   MyAgent-IM is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "animatedtextdocument.h"

#include <QDebug>

#include <QScrollBar>
#include <QAbstractTextDocumentLayout>

#include "gui/animation.h"
#include "emoticonformat.h"
#include "igetanimation.h"

class EmoticonFormatHandler : public QObject, public QTextObjectInterface
{
Q_OBJECT
Q_INTERFACES(QTextObjectInterface)

public:
	EmoticonFormatHandler(QObject* parent = 0) : QObject(parent) {  }

	QSizeF intrinsicSize(QTextDocument* doc, int posInDoc, const QTextFormat &fmt)
	{
		if (fmt.objectType() != EmoticonFormat::EmoticonFormatType)
		{
			qDebug() << "EmoticonFormatHandler::size: not animation format";
			return QSizeF();
		}
		
		Animation* animation = getAnimation(doc, posInDoc, fmt);
		if (!animation)
			return QSizeF();
		
		return QSizeF(animation->size().width() + 1, animation->size().height());
	}

	void drawObject(QPainter* p, const QRectF &rect, QTextDocument* doc,
					int posInDoc, const QTextFormat &fmt)
	{
		Q_UNUSED(p);
		
		if (fmt.objectType() != EmoticonFormat::EmoticonFormatType)
		{
			qDebug() << "EmoticonFormatHandler::draw: not animation format";
			return;
		}
		
		Animation* animation = getAnimation(doc, posInDoc, fmt);
		if (!animation)
			return;
		int id = fmt.property(EmoticonFormat::EmoticonObjectId).toInt();
		
		AnimatedTextDocument* animDoc = qobject_cast<AnimatedTextDocument*>(doc);
		QRect newRect = rect.toRect();
		newRect.translate(1, -animDoc->getEditorScrollBarPosition());
		
		bool selected = false;
		if (animDoc->selectionContains(posInDoc))
			selected = true;
		
		animation->insert(id, newRect, selected);
	}
private:
	Animation* getAnimation(QTextDocument* doc, int posInDoc, const QTextFormat &fmt)
	{
		AnimatedTextDocument* animDoc = qobject_cast<AnimatedTextDocument*>(doc);
		QString name = fmt.property(EmoticonFormat::EmoticonId).toString();
		//qDebug() << "EmoticonFormatHandler: try to get animation, name = " << name;
		Animation* animation = animDoc->getAnimation(posInDoc, name);
		if (!animation)
			qDebug() << "EmoticonFormatHandler: can't get animation, pos = " << posInDoc;
		return animation;
	}
};

AnimatedTextDocument::AnimatedTextDocument(QObject* parent)
	: QTextDocument(parent)
{
	EmoticonFormatHandler* handler = new EmoticonFormatHandler(this);
	documentLayout()->registerHandler(EmoticonFormat::EmoticonFormatType, handler);
}

AnimatedTextDocument::~ AnimatedTextDocument()
{
	editor = NULL;
}

Animation* AnimatedTextDocument::getAnimation(QString name)
{
	return animGetter->getAnimation(name);
}

Animation* AnimatedTextDocument::getAnimation(int posInDoc, const QString& name)
{
	return animGetter->getAnimation(posInDoc, name);
}

int AnimatedTextDocument::getEditorScrollBarPosition()
{
	if (editor == NULL)
		qDebug() << "editor is null";
	QScrollBar* scrollBar = editor->verticalScrollBar();
	if (!scrollBar)
		return 0;
	return scrollBar->sliderPosition();
}

bool AnimatedTextDocument::selectionContains(int posInDoc)
{
	QTextCursor cursor = editor->textCursor();
	if (cursor.hasSelection())
		if (cursor.selectionStart() <= posInDoc && posInDoc < cursor.selectionEnd())
			return true;
	
	return false;
}

#include "animatedtextdocument.moc"
