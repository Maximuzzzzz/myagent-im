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

#ifndef CONTACTINFOLISTWIDGET_H
#define CONTACTINFOLISTWIDGET_H

#include <QTreeView>
#include <QStyleOptionViewItem>

#include "contactinfo.h"

class Account;
class QStandardItemModel;
class QSortFilterProxyModel;

class ContactInfoListWidget : public QTreeView
{
Q_OBJECT
public:
	ContactInfoListWidget(Account* account, QWidget* parent = 0);

	void setInfo(const QList<ContactInfo>& info);
	virtual QSize sizeHint() const;
	
	bool hasSelection() const;
	ContactInfo selectedInfo() const;
	
public slots:
	void showPhotos(bool b);
	
private slots:
	void updateAvatar();
	void slotDoubleClicked(const QModelIndex& index);
private:
	void addPhotosColumn();
	bool photosNotShowedBefore;
	QStandardItemModel* infoListModel;
	QSortFilterProxyModel* proxyModel;
	
	QList<ContactInfo> m_info;
	Account* m_account;
};

#endif
