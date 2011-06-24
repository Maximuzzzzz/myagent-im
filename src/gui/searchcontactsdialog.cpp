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

#include "searchcontactsdialog.h"

#include <QDebug>

#include <QDate>
#include <QIntValidator>
#include <QTextCodec>

#include "zodiac.h"
#include "resourcemanager.h"
#include "protocol/mrim/proto.h"
#include "gui/centerwindow.h"

//using namespace Proto;

int SearchContactsForm::days[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

SearchContactsForm::SearchContactsForm()
{
	qDebug() << "SearchContactsForm::SearchContactsForm()";
	
	setupUi(this);
	setFixedSize(sizeHint());
	centerWindow(this);

	infoGroupBox->setChecked(true);
	
	zodiacBox->addItem("", 0);
	for (int i = 1; i <= 12; i++)
	{
		Zodiac z(i);
		zodiacBox->addItem(z.pixmap(), z.name(), i);
	}
	
	sexBox->addItem("", 0);
	sexBox->addItem(tr("Male"), 1);
	sexBox->addItem(tr("Female"), 2);
	
	const Locations& locations = theRM.locations();
	Locations::countriesIterator it = locations.countriesBegin();
	
	countryBox->addItem("", 0);
	for (;it != locations.countriesEnd(); ++it)
		countryBox->addItem(it->name(), it->id());
	
	connect(countryBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCountryBox(int)));
	
	birthDayBox->addItem("", 0);
	for (int i = 1; i <= 31; i++)
		birthDayBox->addItem(QString::number(i), i);
	
	birthMonthBox->addItem("", 0);
	for (int i = 1; i <= 12; i++)
		birthMonthBox->addItem(QDate::longMonthName(i), i);
	
	connect(birthMonthBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkBirthMonth(int)));
	
	connect(birthDayBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkBirthDay(int)));
	
	connect(birthdateRadioButton, SIGNAL(toggled(bool)), this, SLOT(checkDate(bool)));
	connect(infoGroupBox, SIGNAL(toggled(bool)), this, SLOT(checkDate(bool)));

	connect(searchButton, SIGNAL(clicked(bool)), this, SIGNAL(searchButtonClicked()));
	
	QIntValidator* validator = new QIntValidator(0, 200, this);
	fromAgeEdit->setValidator(validator);
	toAgeEdit->setValidator(validator);
}

SearchContactsForm::~SearchContactsForm()
{
}

void SearchContactsForm::checkCountryBox(int index)
{
	int countryId = countryBox->itemData(index).toInt();
	
	if (countryId)
	{
		regionBox->clear();
		
		const Locations& locations = theRM.locations();
		Locations::countriesIterator ci = locations.country(countryId);
		Locations::Country::regionsIterator ri = ci->regionsBegin();
		
		regionBox->addItem("", 0);
		for (; ri != ci->regionsEnd(); ++ri)
			regionBox->addItem(ri->name(), ri->id());
		
		regionBox->setEnabled(true);
	}
	else
	{
		regionBox->clear();
		regionBox->setEnabled(false);
	}
}

void SearchContactsForm::checkBirthMonth(int index)
{
	int month = birthMonthBox->itemData(index).toInt();
	int day = birthDayBox->itemData(birthDayBox->currentIndex()).toInt();
	
	warnWrongDate(day > days[month-1]);
}

void SearchContactsForm::checkBirthDay(int index)
{
	int day = birthDayBox->itemData(index).toInt();
	int month = birthMonthBox->itemData(birthMonthBox->currentIndex()).toInt();
	
	warnWrongDate(day > days[month-1]);
}

void SearchContactsForm::showWarning(bool condition, const QString& msg)
{
	if (condition)
	{
		errorLabel->setText(msg);
		searchButton->setEnabled(false);
	}
	else
	{
		errorLabel->clear();
		searchButton->setEnabled(true);
	}
}

void SearchContactsForm::warnWrongDate(bool b)
{
	showWarning(b, tr("Wrong date!"));
}

void SearchContactsForm::checkDate(bool b)
{
	if (!b)
		warnWrongDate(false);
	else
	{
		int day = birthDayBox->itemData(birthDayBox->currentIndex()).toInt();
		int month = birthMonthBox->itemData(birthMonthBox->currentIndex()).toInt();
		
		warnWrongDate(day > days[month-1]);
	}
}

MRIMClient::SearchParams SearchContactsForm::info() const
{
	MRIMClient::SearchParams params;
	QTextCodec* codec = QTextCodec::codecForName("cp1251");
	
	if (!nicknameEdit->text().isEmpty())
		params[MRIM_CS_WP_REQUEST_PARAM_NICKNAME] =
			codec->fromUnicode(nicknameEdit->text());
	
	if (!nameEdit->text().isEmpty())
		params[MRIM_CS_WP_REQUEST_PARAM_FIRSTNAME] =
			codec->fromUnicode(nameEdit->text());
	
	if (!lastnameEdit->text().isEmpty())
		params[MRIM_CS_WP_REQUEST_PARAM_LASTNAME] =
			codec->fromUnicode(lastnameEdit->text());
	
	int sex = sexBox->itemData(sexBox->currentIndex()).toInt();
	if (sex)
		params[MRIM_CS_WP_REQUEST_PARAM_SEX] = QByteArray::number(sex);
	
	if (!fromAgeEdit->text().isEmpty())
		params[MRIM_CS_WP_REQUEST_PARAM_DATE1] = fromAgeEdit->text().toLatin1();
	
	if (!toAgeEdit->text().isEmpty())
		params[MRIM_CS_WP_REQUEST_PARAM_DATE2] = toAgeEdit->text().toLatin1();
	
	if (regionBox->isEnabled())
	{
		int regionId = regionBox->itemData(regionBox->currentIndex()).toInt();
		if (regionId)
			params[MRIM_CS_WP_REQUEST_PARAM_CITY_ID] = QByteArray::number(regionId);
	}
	
	if (zodiacRadioButton->isChecked())
	{
		int zodiac = zodiacBox->itemData(zodiacBox->currentIndex()).toInt();
		if (zodiac)
			params[MRIM_CS_WP_REQUEST_PARAM_ZODIAC] = QByteArray::number(zodiac);
	}
	else
	{
		int day = birthDayBox->itemData(birthDayBox->currentIndex()).toInt();
		int month = birthMonthBox->itemData(birthMonthBox->currentIndex()).toInt();
		if (day && month)
		{
			params[MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_MONTH] = QByteArray::number(month);
			params[MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_DAY] = QByteArray::number(day);
		}
	}
	
	int countryId = countryBox->itemData(countryBox->currentIndex()).toInt();
	if (countryId)
		params[MRIM_CS_WP_REQUEST_PARAM_COUNTRY_ID] = QByteArray::number(countryId);
	
	if (onlineContactsCheckBox->isChecked())
		params[MRIM_CS_WP_REQUEST_PARAM_ONLINE] = QByteArray::number(1);
	
	return params;
}
