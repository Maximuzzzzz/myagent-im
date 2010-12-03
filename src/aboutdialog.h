#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>

#include "ui_aboutdialog.h"

class AboutDialog : public QWidget, private Ui::AboutDialog
{
Q_OBJECT
public:
	AboutDialog();
	~AboutDialog();

private slots:
	void openHomeURL();
	void openLicenseURL();
};

#endif // ABOUTDIALOG_H
