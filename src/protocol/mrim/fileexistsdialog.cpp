#include "fileexistsdialog.h"

#include <QLabel>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

FileExistsDialog::FileExistsDialog(QString fileName, QWidget* parent, Qt::WindowFlags f)
 : QDialog(parent, f), m_oldFileName(fileName)
{
	label = new QLabel(tr("File %1 is already exists. What should we do?").arg(fileName));
	quint32 i;
	QFile file;
	for (i = 1; i <= sizeof(quint32); i++)
	{
		file.setFileName(fileName + QString("-") + QString::number(i));
		if (!file.exists())
			break;
	}
	m_newFileName = file.fileName();
	lineEdit = new QLineEdit(m_newFileName);
	rewriteButton = new QPushButton(tr("Rewrite file"));
	renameButton = new QPushButton(tr("Rename file"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(rewriteButton, SIGNAL(clicked()), this, SLOT(rewriteFile()));
	connect(renameButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle(tr("File exists"));
	QVBoxLayout* layout = new QVBoxLayout;
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(rewriteButton);
	buttonsLayout->addWidget(renameButton);
	buttonsLayout->addWidget(cancelButton);
	layout->addWidget(label);
	layout->addWidget(lineEdit);
	layout->addLayout(buttonsLayout);
	setLayout(layout);
}

void FileExistsDialog::rewriteFile()
{
	m_newFileName = m_oldFileName;
	accept();
}
