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

#include "contactinfolistwidget.h"

#include <QDebug>

#include <QHeaderView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QItemDelegate>

#include "account.h"
#include "gui/smallavatar.h"
#include "gui/contactinfodialog.h"

class ContactInfoItemDelegate : public QItemDelegate
{
public:
	ContactInfoItemDelegate(QObject* parent = 0) : QItemDelegate(parent) {}
	virtual void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex& index) const;
};

void ContactInfoItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (index.column() == 0)
	{
		QVariant value = index.data(Qt::DecorationRole);
		if (value.isValid() && value.canConvert(QVariant::Pixmap))
		{
			QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

			int dx = (option.rect.width() - pixmap.rect().width()) / 2;
			int dy = (option.rect.height() - pixmap.rect().height()) / 2;
			dx = (dx < 0) ? 0 : dx;
			dy = (dy < 0) ? 0 : dy;

			QRect rect(option.rect);
			rect.setSize(pixmap.size());
			rect.translate(dx, dy);

			drawBackground(painter, option, index);
			drawDecoration(painter, option, rect, pixmap);
		}
	}
	else
		QItemDelegate::paint(painter, option, index);
}

class ContactInfoItem : public QStandardItem
{
public:
	ContactInfoItem() : QStandardItem()
	{
		setEditable(false);
	}

	ContactInfoItem(const QString& text) : QStandardItem(text)
	{
		setEditable(false);
	}

	ContactInfoItem(const QIcon& icon, const QString& text)
		: QStandardItem(icon, text)
	{
		setEditable(false);
	}
};

ContactInfoListWidget::ContactInfoListWidget(Account* account, QWidget* parent)
	: QTreeView(parent), m_account(account)
{
	photosNotShowedBefore = true;

	infoListModel = new QStandardItemModel(this);
	//infoListModel->setSortRole(Qt::UserRole + 1);

	proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setSortLocaleAware(true);
	proxyModel->setSourceModel(infoListModel);
	setModel(proxyModel);
	setRootIsDecorated(false);
	setSortingEnabled(true);
	setUniformRowHeights(true);
	setItemDelegateForColumn(0, new ContactInfoItemDelegate(this));
	header()->setResizeMode(QHeaderView::ResizeToContents);

	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
}

void ContactInfoListWidget::setInfo(const QList<ContactInfo>& info)
{
	infoListModel->clear();
	m_info = info;

	QStringList headerLabels;

	headerLabels << "";
	headerLabels << tr("Nickname");
	headerLabels << tr("E-mail");
	headerLabels << tr("First name");
	headerLabels << tr("Last name");
	headerLabels << tr("Sex");
	headerLabels << tr("Age");

	infoListModel->setHorizontalHeaderLabels(headerLabels);

	for (int i = 0; i < info.size(); i++)
	{
		int j = 1;
		ContactInfoItem* nicknameItem = new ContactInfoItem;
		nicknameItem->setIcon(info.at(i).onlineStatus().statusIcon());
		nicknameItem->setText(info.at(i).nickname());
		infoListModel->setItem(i, j++, nicknameItem);
		infoListModel->setItem(i, j++, new ContactInfoItem(info.at(i).email()));
		infoListModel->setItem(i, j++, new ContactInfoItem(info.at(i).firstname()));
		infoListModel->setItem(i, j++, new ContactInfoItem(info.at(i).lastname()));
		infoListModel->setItem(i, j++, new ContactInfoItem(info.at(i).sex()));

		ContactInfoItem* ageItem = new ContactInfoItem;
		QString strAge = info.at(i).age();
		if (strAge.isEmpty())
			ageItem->setText("");
		else
			ageItem->setData(strAge.toUInt(0), Qt::DisplayRole);
		ageItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		infoListModel->setItem(i, j++, ageItem);
	}

	setColumnHidden(0, true);

	qDebug() << "ContactInfoListWidget: info setted";
}

QSize ContactInfoListWidget::sizeHint() const
{
	qDebug() << "ContactInfoListWidget::sizeHint()";
	int w = 0;
	int cols = header()->count();
	for (int i = 0; i < cols; i++)
		w += header()->sectionSize(i);

	int h = header()->height();
	int rows = infoListModel->rowCount();
	int rowheight = itemDelegate()->sizeHint(viewOptions(), infoListModel->index(0, 1)).height();

	h += rowheight*rows;
	h += frameWidth()*2;

	qDebug() << "ContactInfoListWidget sizeHint = " << QSize(w, h);

	return QSize(w, h);
}

void ContactInfoListWidget::updateAvatar()
{
	Avatar* avatar = qobject_cast<Avatar*>(sender());
	QList<QStandardItem*> itemList =
		infoListModel->findItems(avatar->email(), Qt::MatchExactly, 2);

	if (itemList.isEmpty())
	{
		qDebug("ContactInfoListWidget cant't find item by email");
		return;
	}

	QStandardItem* item = itemList.first();

	infoListModel->item(item->row())->setData(*avatar, Qt::DecorationRole);
	infoListModel->item(item->row())->setText(QString::number(avatar->state()));
}

void ContactInfoListWidget::showPhotos(bool b)
{
	if (photosNotShowedBefore && b)
	{
		addPhotosColumn();
		photosNotShowedBefore = false;
	}

	setColumnHidden(0, !b);
	expandAll();
}

void ContactInfoListWidget::addPhotosColumn()
{
	for (int i = 0; i < infoListModel->rowCount(); i++)
	{
		QString email = infoListModel->item(i, 2)->text();
		SmallAvatar* avatar = new SmallAvatar(this);
		connect(avatar, SIGNAL(updated()), this, SLOT(updateAvatar()));
		ContactInfoItem* item = new ContactInfoItem;
		item->setData(*avatar, Qt::DecorationRole);
		item->setData(avatar->size(), Qt::SizeHintRole);
		item->setText(QString::number(avatar->state()));
		item->setEditable(false);
		infoListModel->setItem(i, 0, item);
		avatar->load(m_account->avatarsPath(), email);
	}
}

void ContactInfoListWidget::slotDoubleClicked(const QModelIndex& index)
{
	QModelIndex realIndex = proxyModel->mapToSource(index);
	ContactInfoDialog::create(m_account, m_info.at(realIndex.row()));
}

bool ContactInfoListWidget::hasSelection() const
{
	return selectionModel()->hasSelection();
}

ContactInfo ContactInfoListWidget::selectedInfo() const
{
	QModelIndex realIndex = proxyModel->mapToSource(currentIndex());
	return m_info.at(realIndex.row());
}
