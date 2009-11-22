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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef ANIMATIONSUPPORT_H
#define ANIMATIONSUPPORT_H

#include <QDebug>

#include <QTextEdit>
#include <QTextDocumentFragment>
#include <QMimeData>

#include "animation.h"
#include "animatedtextdocument.h"
#include "rtfexporter.h"
#include "resourcemanager.h"
#include "emoticonformat.h"
#include "igetanimation.h"

template <class TextEdit>
class AnimationSupport : public TextEdit, IGetAnimation
{
public:
	AnimationSupport(QWidget* parent = 0);
	~AnimationSupport();

	void clear();

protected:
	virtual void resizeEvent(QResizeEvent* event );
	virtual QMimeData* createMimeDataFromSelection() const;

	void processDocumentChange(int position, int charsRemoved, int charsAdded);

	Animation* getAnimation(QString name);
	Animation* getAnimation(int posInDoc, const QString& name);
	
	struct AnimInfo
	{
		int position;
		int id;
		QString name;
		Animation* animation;
		
		operator int() const { return position; }
	};
	
	int objectId;
	QList<AnimInfo> positionInfos;
	QMap<QString, Animation*> animations;
};

template <class TextEdit>
AnimationSupport<TextEdit>::AnimationSupport(QWidget* parent)
	: TextEdit(parent)
{
	AnimatedTextDocument* doc = new AnimatedTextDocument(this);
	doc->setEditor(dynamic_cast<QTextEdit*>(this), dynamic_cast<IGetAnimation*>(this));
	this->setDocument(doc);
	
	//connect(doc, SIGNAL(contentsChange(int,int,int)), this, SLOT(processDocumentChange(int,int,int)));
	
	objectId = 1;
}

template <class TextEdit>
AnimationSupport<TextEdit>::~AnimationSupport()
{
	clear();
}

template <class TextEdit>
QMimeData* AnimationSupport<TextEdit>::createMimeDataFromSelection() const
{
	QMimeData* mimeData = QTextEdit::createMimeDataFromSelection();
	
	QTextDocumentFragment selectionFragment(this->textCursor());
	QTextDocument selectionDoc;
	QTextCursor selCursor(&selectionDoc);
	selCursor.insertFragment(selectionFragment);
	RtfExporter rtfExporter(&selectionDoc);
	
	mimeData->setData("text/rtf", rtfExporter.toRtf());
	
	return mimeData;
}

template <class TextEdit>
void AnimationSupport<TextEdit>::processDocumentChange(int position, int charsRemoved, int charsAdded)
{
	//qDebug() << "AnimationSupport<TextEdit>: position = " << position << ", charsRemoved = " << charsRemoved << ", charsAdded = " << charsAdded << ", time = " << QTime::currentTime().msec();

	typename QList<AnimInfo>::iterator a =
			qLowerBound(positionInfos.begin(), positionInfos.end(), position);

	if (charsRemoved > 0)
	{
		typename QList<AnimInfo>::iterator b =
				qLowerBound(a, positionInfos.end(), position + charsRemoved);
	
		for (typename QList<AnimInfo>::iterator i = a; i != b; ++i)
		{
			//qDebug() << "remove object id = " << i->id << " from animation = " << i->name;
			animations[i->name]->remove(i->id);
		}
	
		a = positionInfos.erase(a, b);
	}

	if (charsAdded > 0)
	{
		QTextCursor cursor(this->document());
		cursor.setPosition(position+1);
		
		QTextCharFormat fmt;
	
		int startObjectId = objectId;
		
		for (int i = 0; i < charsAdded; i++)
		{
			fmt = cursor.charFormat();
		
			if (fmt.objectType() == EmoticonFormat::EmoticonFormatType)
			{
				QString emoticonId = fmt.property(EmoticonFormat::EmoticonId).toString();
			
				if (!animations.contains(emoticonId))
				{
					Animation* anim = new Animation(this);
					animations[emoticonId] = anim;
					anim->load(emoticonId);
				}
			
				AnimInfo info;
				info.position = position + i;
				info.id = objectId++;
				info.name = emoticonId;
				info.animation = animations[emoticonId];
				
				//qDebug() << "AnimationSupport<TextEdit>::processDocumentChange insert anim info";
				a = positionInfos.insert(a, info);
				++a;
			}
			cursor.movePosition(QTextCursor::NextCharacter);
		}
	
		typename QList<AnimInfo>::iterator i = qLowerBound(positionInfos.begin(), a, position);
	
		while (i != a)
		{
			//qDebug() << i->position;
			cursor.setPosition(i->position);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			fmt = cursor.charFormat();
			fmt.setProperty(EmoticonFormat::EmoticonObjectId, startObjectId++);
			cursor.setCharFormat(fmt);
			cursor.clearSelection();
			++i;
		}
	}

	int d = charsAdded - charsRemoved;

	for (typename QList<AnimInfo>::iterator i = a; i < positionInfos.end(); ++i)
	{
		i->position += d;
	}
}

template <class TextEdit>
void AnimationSupport<TextEdit>::clear()
{
	TextEdit::clear();
	objectId = 1;
	positionInfos.clear();
	QMap<QString, Animation*>::iterator i;
	for (i = animations.begin(); i != animations.end(); ++i)
		delete i.value();

	animations.clear();
}

template <class TextEdit>
Animation* AnimationSupport<TextEdit>::getAnimation(QString name)
{
	//qDebug() << "AnimationSupport<TextEdit>: getAnimation by name";
	return animations.value(name);
}

template <class TextEdit>
Animation* AnimationSupport<TextEdit>::getAnimation(int posInDoc, const QString& name)
{
	typename QList<AnimInfo>::iterator a =
		qBinaryFind(positionInfos.begin(), positionInfos.end(), posInDoc);
	if (a == positionInfos.end())
	{
		Animation* anim = animations.value(name);
		if (!anim)
		{
			//qDebug() << "AnimationSupport<TextEdit>: getAnimation - creating animation";
			
			anim = new Animation(this);
			animations[name] = anim;
			anim->load(name);
			
			//qDebug() << "AnimationSupport<TextEdit>: animation created";
		}
		
		return anim;
	}
	else
		return a->animation;
}

template <class TextEdit>
void AnimationSupport<TextEdit>::resizeEvent(QResizeEvent* event)
{
	int nAnimations = positionInfos.size();
	for (int i = 0; i < nAnimations; i++)
		positionInfos.at(i).animation->hide();

	TextEdit::resizeEvent(event);
}

#endif
