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

#include "radiogroupbox.h"

#include <QStylePainter>
#include <QStyleOptionGroupBox>
#include <QMouseEvent>

RadioGroupBox::RadioGroupBox(const QString& title, QWidget* parent)
	: QGroupBox(title, parent)
{
	init();
}

RadioGroupBox::RadioGroupBox(QWidget* parent)
	: QGroupBox(parent)
{
	init();
}

void RadioGroupBox::init()
{
	down = false;
	setCheckable(true);
	setChecked(false);
}

RadioGroupBox::~RadioGroupBox()
{
}

void RadioGroupBox::paintEvent(QPaintEvent* /*event*/)
{
	QStylePainter p(this);
	QStyleOptionGroupBox option;
	initStyleOption(&option);
	
	QRect textRect = style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxLabel, this);
	QRect checkBoxRect = style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxCheckBox, this);
	if (option.subControls & QStyle::SC_GroupBoxFrame)
	{
		QStyleOptionFrameV2 frame;
		frame.QStyleOption::operator=(option);
		frame.features = option.features;
		frame.lineWidth = option.lineWidth;
		frame.midLineWidth = option.midLineWidth;
		frame.rect = style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxFrame, this);
		p.save();
		QRegion region(option.rect);
		if (!option.text.isEmpty()) {
			bool ltr = option.direction == Qt::LeftToRight;
			QRect finalRect;
			if (option.subControls & QStyle::SC_GroupBoxCheckBox) {
				finalRect = checkBoxRect.united(textRect);
				finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
			} else {
				finalRect = textRect;
			}
			region -= finalRect;
		}
		p.setClipRegion(region);
		p.drawPrimitive(QStyle::PE_FrameGroupBox, frame);
		p.restore();
	}

            // Draw title
	if ((option.subControls & QStyle::SC_GroupBoxLabel) && !option.text.isEmpty()) {
		QColor textColor = option.textColor;
		if (textColor.isValid())
			p.setPen(textColor);
		int alignment = int(option.textAlignment);
		if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
			alignment |= Qt::TextHideMnemonic;

		p.drawItemText(textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
					option.palette, option.state & QStyle::State_Enabled, option.text,
					textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

		if (option.state & QStyle::State_HasFocus)
		{
			QStyleOptionFocusRect fropt;
			fropt.QStyleOption::operator=(option);
			fropt.rect = textRect;
			p.drawPrimitive(QStyle::PE_FrameFocusRect, fropt);
		}
	}

	if (option.subControls & QStyle::SC_GroupBoxCheckBox) {
		QStyleOptionButton box;
		box.QStyleOption::operator=(option);
		box.rect = checkBoxRect;
		p.drawPrimitive(QStyle::PE_IndicatorRadioButton, box);
	}
}

void RadioGroupBox::initStyleOption(QStyleOptionGroupBox* option) const
{
	QGroupBox::initStyleOption(option);
	option->subControls |= QStyle::SC_GroupBoxCheckBox;
	
	if (down)
		option->state |= QStyle::State_Sunken;
	else
		option->state &= ~QStyle::State_Sunken;
}

bool RadioGroupBox::hitButtonOrLabel(const QPoint& pos) const
{
	QStyleOptionGroupBox box;
	initStyleOption(&box);
	QStyle::SubControl pressedControl = style()->hitTestComplexControl(QStyle::CC_GroupBox, &box, pos, this);
	if (pressedControl & (QStyle::SC_GroupBoxCheckBox | QStyle::SC_GroupBoxLabel))
		return true;
	else
		return false;
}

void RadioGroupBox::mousePressEvent(QMouseEvent * e)
{
	if (e->button() != Qt::LeftButton)
	{
		e->ignore();
		return;
	}
	
	if (hitButtonOrLabel(e->pos()))
	{
		down = true;
		repaint();
	} else
		e->ignore();
}

void RadioGroupBox::mouseMoveEvent(QMouseEvent * e)
{
	if (!(e->buttons() & Qt::LeftButton)) {
		e->ignore();
		return;
	}

	if (hitButtonOrLabel(e->pos()) != down) {
		down = !down;
		repaint();
		e->accept();
	} else if (!hitButtonOrLabel(e->pos())) {
		e->ignore();
	}
}

void RadioGroupBox::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() != Qt::LeftButton) {
		e->ignore();
		return;
	}

	if (!down) {
		e->ignore();
		return;
	}

	down = false;
	
	if (hitButtonOrLabel(e->pos()))
	{
		setChecked(true);
		repaint();
	} else
		e->ignore();
}

void RadioGroupBox::setChecked(bool b)
{
	if (!b)
	{
		QGroupBox::setChecked(false);
		repaint();
		return;
	}
	
	if (!parent())
		return;
	
	QList<RadioGroupBox*> children = parent()->findChildren<RadioGroupBox*>();
	
	if (children.size() < 2)
		return;
	
	QList<RadioGroupBox*>::iterator it = children.begin();
	while (it != children.end())
	{
		if (*it != this)
			(*it)->setChecked(false);
		
		++it;
	}
	
	QGroupBox::setChecked(true);
	repaint();
}

void RadioGroupBox::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Select || event->key() == Qt::Key_Space))
	{
		down = true;
		
		QStyleOptionGroupBox box;
		initStyleOption(&box);
		update(style()->subControlRect(QStyle::CC_GroupBox, &box, QStyle::SC_GroupBoxCheckBox, this));
		return;
	}
	
	QGroupBox::keyPressEvent(event);
}

void RadioGroupBox::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Select || event->key() == Qt::Key_Space)
	{
		down = false;
		
		setChecked(true);
		repaint();
		return;
	}
	
	QGroupBox::keyReleaseEvent(event);
}

bool RadioGroupBox::event(QEvent * event)
{
	return QWidget::event(event);
}
