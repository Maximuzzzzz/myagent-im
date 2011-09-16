/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QtGui>

#include "flowlayout.h"

FlowLayout::FlowLayout ( QWidget *parent, int margin, int spacing )
		: QLayout ( parent )
{
	setMargin ( margin );
	setSpacing ( spacing );
}

FlowLayout::FlowLayout ( int spacing )
{
	setSpacing ( spacing );
}

FlowLayout::~FlowLayout()
{
	QLayoutItem *item;
	while ( ( item = takeAt ( 0 ) ) )
		delete item;
}

void FlowLayout::addItem ( QLayoutItem *item )
{
	itemList.append ( item );
}

void FlowLayout::insertWidget(int i, QWidget* w)
{
	addChildWidget(w);
	itemList.insert(i, new QWidgetItem(w));
}

int FlowLayout::count() const
{
	return itemList.size();
}

QLayoutItem *FlowLayout::itemAt ( int index ) const
{
	qDebug() << Q_FUNC_INFO << index;
	return itemList.value ( index );
}

QLayoutItem *FlowLayout::takeAt ( int index )
{
	if ( index >= 0 && index < itemList.size() )
		return itemList.takeAt ( index );
	else
		return 0;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
	return Qt::Vertical | Qt::Horizontal;
}

bool FlowLayout::hasHeightForWidth() const
{
	return true;
}

int FlowLayout::heightForWidth ( int width ) const
{
	int height = doLayout ( QRect ( 0, 0, width, 0 ), true );
	return height;
}

void FlowLayout::setGeometry ( const QRect &rect )
{
	QLayout::setGeometry ( rect );
	doLayout ( rect, false );
}

QSize FlowLayout::sizeHint() const
{
	return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
	QSize size;
	QLayoutItem *item;
	Q_FOREACH ( item, itemList )
	size = size.expandedTo ( item->minimumSize() );

	int left, top, right, bottom;
	getContentsMargins ( &left, &top, &right, &bottom );
	size += QSize ( left + right, top + bottom );
	return size;
}

int FlowLayout::doLayout ( const QRect &rect, bool testOnly ) const
{
	int left, top, right, bottom;
	getContentsMargins ( &left, &top, &right, &bottom );

	int x = rect.x() + left;
	int y = rect.y() + top;
	int lineHeight = 0;

	QLayoutItem *item;
	Q_FOREACH ( item, itemList )
	{
		int nextX = x + item->sizeHint().width() + spacing();
		if ( nextX - spacing() > (rect.right() - right) && lineHeight > 0 )
		{
			x = rect.x() + left;
			y = y + lineHeight + spacing();
			nextX = x + item->sizeHint().width() + spacing();
			//lineHeight = 0;
		}

		if ( !testOnly )
			item->setGeometry ( QRect ( QPoint ( x, y ), item->sizeHint() ) );

		x = nextX;
		lineHeight = qMax ( lineHeight, item->sizeHint().height() );
	}
	return y + lineHeight - rect.y();
}

int FlowLayout::indexOfItemAtPos(const QPoint & pos)
{
	for (int i = 0; i < itemList.size(); i++)
	{
		if (itemList.at(i)->geometry().adjusted(0, 0, spacing(), spacing()).contains(pos))
			return i;
	}

	return itemList.size();
}
