#ifndef REMOVECONTACTDIALOG_H
#define REMOVECONTACTDIALOG_H

#include <QObject>
#include <QDialog>
#include <QCheckBox>

class RemoveContactDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RemoveContactDialog(const QString &title, const QString &text,	QWidget *parent = 0, Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	~RemoveContactDialog();

	bool removeHistory() { return removeHistoryCheckBox->isChecked(); }

private:
	QCheckBox* removeHistoryCheckBox;
};

#endif // REMOVECONTACTDIALOG_H
