#ifndef FILEEXISTSDIALOG_H
#define FILEEXISTSDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;

class FileExistsDialog : public QDialog
{
Q_OBJECT
public:
	explicit FileExistsDialog(QString fileName, QWidget* parent = 0, Qt::WindowFlags f = 0);
	QString fileName() const { return m_newFileName; }

private Q_SLOTS:
	void rewriteFile();

private:
	QLabel* label;
	QLineEdit* lineEdit;
	QPushButton* rewriteButton;
	QPushButton* renameButton;
	QPushButton* cancelButton;

	QString m_newFileName;
	QString m_oldFileName;
};

#endif // FILEEXISTSDIALOG_H
