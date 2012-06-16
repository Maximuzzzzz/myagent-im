/***************************************************************************
 *   Copyright (C) 2011 by Dmitry Malakhov <abr_mail@mail.ru>              *
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

#include "contactlistbroadcast.h"

#include "contactlistmodel.h"
#include "models/contactlistbroadcastmodel.h"
#include "core/contactlist.h"

ContactListBroadcast::ContactListBroadcast(QByteArray contactEmail, ContactList* cl, QWidget* parent)
 : QListView(parent)
{
	m_contactList = cl;
	broadcastModel = new ContactListBroadcastModel(contactEmail, cl, this);
	connect(cl->model(), SIGNAL(modelRebuilded()), broadcastModel, SLOT(rebuild()));
	setModel(broadcastModel);
}

ContactListBroadcast::~ContactListBroadcast()
{
}

QList<QByteArray> &ContactListBroadcast::receivers()
{
	return broadcastModel->broadcastList();
}
