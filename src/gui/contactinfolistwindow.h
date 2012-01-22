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

#ifndef CONTACTINFOLISTWINDOW_H
#define CONTACTINFOLISTWINDOW_H

#include <QWidget>

class QPushButton;

class ContactInfoListWidget;
class ContactInfo;
class Account;

class ContactInfoListWindow : public QWidget
{
Q_OBJECT
public:
	ContactInfoListWindow(Account* account);
	
	void setInfo(const QList<ContactInfo>& info, uint maxRows);
Q_SIGNALS:
	void addButtonClicked(const ContactInfo& info);
	void moreContactsButtonClicked();
	void newSearchButtonClicked();
private Q_SLOTS:
	void slotAddButtonClicked();
private:
	ContactInfoListWidget* infoListWidget;
	QPushButton* moreContactsButton;
};

#endif
