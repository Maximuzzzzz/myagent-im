#include "centeredmessagebox.h"

#include "centerwindow.h"

QMessageBox::StandardButton CenteredMessageBox::messageBox(QMessageBox::Icon icon, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	QMessageBox mb(icon, title, text, buttons);
	mb.setDefaultButton(defaultButton);
	centerWindow(&mb);
	return (QMessageBox::StandardButton)mb.exec();
}

QMessageBox::StandardButton CenteredMessageBox::critical(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Critical, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::information(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Information, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::question(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton CenteredMessageBox::warning(const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	return messageBox(QMessageBox::Warning, title, text, buttons, defaultButton);
}
