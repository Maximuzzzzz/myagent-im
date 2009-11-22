#ifndef CENTEREDMESSAGEBOX_H
#define CENTEREDMESSAGEBOX_H

#include <QMessageBox>

class CenteredMessageBox
{
public:
	static QMessageBox::StandardButton critical(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton information(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton question(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
	static QMessageBox::StandardButton warning(const QString & title, const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
private:
	static inline QMessageBox::StandardButton messageBox(QMessageBox::Icon icon, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);
};

#endif
