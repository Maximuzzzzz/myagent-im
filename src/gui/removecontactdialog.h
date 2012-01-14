#ifndef REMOVECONTACTDIALOG_H
#define REMOVECONTACTDIALOG_H

#include <QDialog>

class QCheckBox;

class RemoveContactDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RemoveContactDialog(const QString &title, const QString &text,	QWidget *parent = 0, Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	~RemoveContactDialog();

	bool removeHistory();

private:
	QCheckBox* removeHistoryCheckBox;
};

#endif // REMOVECONTACTDIALOG_H
