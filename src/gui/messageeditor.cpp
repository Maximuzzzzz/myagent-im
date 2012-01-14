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

#include "messageeditor.h"

#include <qdesktopwidget.h>
#include <QToolBar>
#include <QAction>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QKeyEvent>
#include <QSettings>
#include <QPushButton>
#include <QProgressBar>
#include <QTextBlock>

#include "messageedit.h"
#include "toolbutton.h"
#include "account.h"
#include "contact.h"
#include "gui/emoticonselector.h"
#include "gui/multselector.h"

MessageEditor::MessageEditor(Account* account, Contact* contact, EmoticonSelector* emSel, MultSelector* multSel, QWidget* parent)
	: QWidget(parent), m_account(account), m_contact(contact), emoticonSelector(emSel), multSelector(multSel)
{
	connect(m_contact, SIGNAL(destroyed(Contact*)), this, SLOT(writeSettings()));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	
	createToolBar();
	createFileTransferBar();
	messageEdit = new MessageEdit;
	messageEdit->installEventFilter(this);
	
	connect(messageEdit, SIGNAL(textChanged()), SIGNAL(textChanged()));
	connect(messageEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateFormatActions()));

	layout->addWidget(toolBar);
	layout->addWidget(fileTransferBar);
	layout->addWidget(fileProcessBar);
	
	layout->addWidget(messageEdit);
	setLayout(layout);

	connect(emoticonSelector, SIGNAL(selected(MessageEditor*, QString)), SLOT(insertEmoticon(MessageEditor*, QString)));
	connect(emoticonSelector, SIGNAL(closed()), smilesAction, SLOT(toggle()));

	connect(multSelector, SIGNAL(selected(MessageEditor*, QString)), SLOT(insertFlash(MessageEditor*, QString)));
	connect(multSelector, SIGNAL(closed()), multsAction, SLOT(toggle()));

	readSettings();

	connect(messageEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat&)), this, SLOT(slotCurrentCharFormatChanged(const QTextCharFormat &)));

	if (!m_contact->isConference())
	{
		connect(m_contact, SIGNAL(statusChanged(OnlineStatus)), this, SLOT(checkContactStatus(OnlineStatus)));
		checkContactStatus(m_contact->status());
	}
}

MessageEditor::~MessageEditor()
{
	writeSettings();
}

bool MessageEditor::eventFilter(QObject * obj, QEvent * ev)
{
	if (obj == messageEdit && ev->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
		//qDebug() << "MessageEdit::eventFilter, key =" << QString::number(keyEvent->key(), 16) << ", mods =" << QString::number(keyEvent->modifiers(), 16);
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
		{
			QString enterVariant = m_account->settings()->value("Messages/sendOnEnter", "Ctrl+Enter").toString();
			if (enterVariant == "Enter")
			{
				if (!keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::NoModifier)
				{
					Q_EMIT sendPressed();
					return true;
				}
			}
			else if (enterVariant == "Enter+Enter")
			{
				if (keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::NoModifier)
				{
					Q_EMIT sendPressed();
					return true;
				}
			}
			else if (enterVariant == "Ctrl+Enter")
			{
				if (!keyEvent->isAutoRepeat() && keyEvent->modifiers() == Qt::ControlModifier)
				{
					Q_EMIT sendPressed();
					return true;
				}
			}
		}
		
		if ((keyEvent->key() == Qt::Key_S || keyEvent->key() == 0x42b) && keyEvent->modifiers() == Qt::AltModifier)
		{
			Q_EMIT sendPressed();
			return true;
		}
	}
	
	return QWidget::eventFilter(obj, ev);
}

QAction* MessageEditor::addToolAction(const QIcon& icon, const QObject * receiver, const char * method)
{
	QAction* action = new QAction(icon, "", this);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), receiver, method);
	toolBar->addAction(action);
	toolBar->widgetForAction(action)->setStyleSheet("QToolButton { border: 0px; padding: 0px }");
	
	return action;
}

QIcon MessageEditor::toolIcon(const QString& toolName) const
{
	QIcon icon;
	icon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_" + toolName + "_d.png"), QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_" + toolName + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_" + toolName + "_p.png"), QIcon::Normal, QIcon::On);
	icon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_" + toolName + "_ph.png"), QIcon::Active, QIcon::On);
	
	return icon;
}

void MessageEditor::createToolBar()
{
	toolBar = new QToolBar;
	
	spellAction = addToolAction(toolIcon("orfo"), this, SIGNAL(setSignalCheckSpelling(bool)));

	toolBar->addSeparator();

	fontAction = addToolAction(toolIcon("font"), this, SLOT(chooseFont()));
	colorAction = addToolAction(toolIcon("font_color"), this, SLOT(chooseFontColor()));
	bkColorAction = addToolAction(toolIcon("bkcolor"), this, SLOT(chooseBackgroundColor()));
	
	toolBar->addSeparator();
	
	boldFontAction = addToolAction(toolIcon("bold"), this, SLOT(setFontBold(bool)));
	italicFontAction = addToolAction(toolIcon("italic"), this, SLOT(setFontItalic(bool)));
	underlinedFontAction = addToolAction(toolIcon("underlined"), this, SLOT(setFontUnderline(bool)));
	
	toolBar->addSeparator();
	
	smilesAction = addToolAction(toolIcon("smiles"), this, SLOT(triggerEmoticonSelector()));

	QIcon multIcon;
	multIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_mults_dis.png"), QIcon::Disabled, QIcon::Off);
	multIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_mults_d.png"), QIcon::Normal, QIcon::Off);
	multIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_mults_p.png"), QIcon::Normal, QIcon::On);
	multIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_s_mults_h.png"), QIcon::Active, QIcon::Off);
	multsAction = addToolAction(multIcon, this, SLOT(triggerMultSelector()));
	
	toolBar->addSeparator();
	
	QIcon wakeupIcon;
	wakeupIcon.addPixmap(QPixmap(":/icons/editor/wakeup_dis.png"), QIcon::Disabled, QIcon::Off);
	wakeupIcon.addPixmap(QPixmap(":/icons/editor/wakeup_d.png"), QIcon::Normal, QIcon::Off);
	wakeupIcon.addPixmap(QPixmap(":/icons/editor/wakeup_p.png"), QIcon::Normal, QIcon::On);
	wakeupIcon.addPixmap(QPixmap(":/icons/editor/wakeup_h.png"), QIcon::Active, QIcon::Off);

	wakeupButton = new ToolButton(wakeupIcon, toolBar);
	connect(wakeupButton, SIGNAL(clicked()), SIGNAL(wakeupPressed()));
	toolBar->addWidget(wakeupButton);

	toolBar->addSeparator();

	fileTransferAction = new QAction(QIcon(":/icons/ft.png"), "", this);
	fileTransferAction->setCheckable(true);
	connect(fileTransferAction, SIGNAL(triggered(bool)), this, SLOT(fileTransfer(bool)));
	toolBar->addAction(fileTransferAction);

	toolBar->addSeparator();
	ignoreAction = addToolAction(toolIcon("ignore"), this, SIGNAL(setIgnore(bool)));

	ignoreAction->setToolTip(tr("Ignore interlocutor style"));

	toolBar->addSeparator();

	QIcon broadcastIcon;
	broadcastIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_f_multi_dis.png"), QIcon::Disabled, QIcon::Off);
	broadcastIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_f_multip_d.png"), QIcon::Normal, QIcon::Off);
	broadcastIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_f_multi_p.png"), QIcon::Normal, QIcon::On);
	broadcastIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_f_multi_h.png"), QIcon::Active, QIcon::Off);
	broadcastIcon.addPixmap(QPixmap(":/icons/editor/msg_bar_f_multi_ph.png"), QIcon::Active, QIcon::On);

	broadcastAction = new QAction(broadcastIcon, "", toolBar);
	broadcastAction->setCheckable(true);
	connect(broadcastAction, SIGNAL(triggered(bool)), SIGNAL(showBroadcastPanel(bool)));
	toolBar->addAction(broadcastAction);
	toolBar->widgetForAction(broadcastAction)->setStyleSheet("QToolButton { border: 0px; padding: 0px }");
	broadcastAction->setToolTip(tr("Broadcast"));

	if (m_contact->isConference())
		broadcastAction->setEnabled(false);
}

void MessageEditor::chooseFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, messageEdit->currentFont(), this);
	if (ok)
		setCurrentFont(font);
	
	fontAction->setChecked(false);
	messageEdit->setFocus(Qt::OtherFocusReason);
}

void MessageEditor::chooseFontColor()
{
	QColor color = QColorDialog::getColor(messageEdit->textColor(), this);
	if (color.isValid())
		setTextColor(color);
	
	colorAction->setChecked(false);
	messageEdit->setFocus(Qt::OtherFocusReason);
}

void MessageEditor::chooseBackgroundColor()
{
	QColor color = QColorDialog::getColor(messageEdit->textBackgroundColor(), this);
	if (color.isValid())
		setTextBackgroundColor(color);
	
	bkColorAction->setChecked(false);
	messageEdit->setFocus(Qt::OtherFocusReason);
}

void MessageEditor::setCurrentFont(const QFont & f)
{
	QTextCharFormat fmt;
	fmt.setFont(f);

	mergeMyCharFormat(fmt);
	updateFormatActions();

	defaultFont = f;
}

void MessageEditor::setTextColor(const QColor & c)
{
	QTextCharFormat fmt;
	fmt.setForeground(c);

	mergeMyCharFormat(fmt);

	defaultFontColor = c;
}

void MessageEditor::setTextBackgroundColor(const QColor & c)
{
	QTextCharFormat fmt;
	fmt.setBackground(c);

	mergeMyCharFormat(fmt);

	defaultBkColor = c;
}

void MessageEditor::mergeMyCharFormat(const QTextCharFormat & fmt)
{
	qDebug() << "MessageEditor::mergeMyCharFormat";
	QTextCursor cursor = messageEdit->textCursor();

	cursor.mergeBlockCharFormat(fmt);
	messageEdit->document()->markContentsDirty(cursor.block().position(), cursor.block().length());

	cursor.mergeCharFormat(fmt);

	lastUserFormat = cursor.charFormat();

	messageEdit->setTextCursor(cursor);
}

void MessageEditor::setFontBold(bool b)
{
	QTextCharFormat fmt;
	fmt.setFontWeight(b ? QFont::Bold : QFont::Normal);

	mergeMyCharFormat(fmt);
}

void MessageEditor::setFontItalic(bool b)
{
	QTextCharFormat fmt;
	fmt.setFontItalic(b);

	mergeMyCharFormat(fmt);
}

void MessageEditor::setFontUnderline(bool b)
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(b);

	mergeMyCharFormat(fmt);
}

void MessageEditor::insertEmoticon(MessageEditor* editor, const QString & id)
{
	if (editor != this)
		return;

	QTextCursor cursor = messageEdit->textCursor();
	QTextCharFormat currentFormat = cursor.charFormat();
	EmoticonFormat fmt(currentFormat, id);
	cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
	messageEdit->setTextCursor(cursor);
	messageEdit->setFocus(Qt::OtherFocusReason);
	messageEdit->setCurrentCharFormat(currentFormat);

	smilesAction->setChecked(false);
	emoticonSelector->hide();
}

void MessageEditor::insertFlash(MessageEditor* editor, const QString & id)
{
	qDebug() << Q_FUNC_INFO << id;
	if (editor != this)
		return;

	QTextCursor cursor = messageEdit->textCursor();
	QTextCharFormat currentFormat = cursor.charFormat();
	EmoticonFormat fmt(currentFormat, id);
/*	cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
	messageEdit->setTextCursor(cursor);
	messageEdit->setFocus(Qt::OtherFocusReason);
	messageEdit->setCurrentCharFormat(currentFormat);*/

	multsAction->setChecked(false);
	multSelector->hide();

	Q_EMIT showMult(id);
}

void MessageEditor::readSettings()
{
	if (m_account.isNull() || m_contact.isNull())
	{
		qDebug() << "MessageEditor::readSettings() error: m_account = " << (void*)m_account << ", m_contact = " << (void*)m_contact;
		return;
	}
	
	bool checkSpelling = m_account->settings()->value("MessageEditor/checkSpelling", false).toBool();
	setCheckSpelling(checkSpelling);
	
	QString settingsPrefix = "MessageEditor_" + m_contact->email() + '/';
	defaultFont.fromString(m_account->settings()->value(settingsPrefix + "font").toString());
	defaultFontColor = QColor(m_account->settings()->value(settingsPrefix + "textColor", "#000000").toString());
	defaultBkColor = QColor(m_account->settings()->value(settingsPrefix + "backgroundColor", "#ffffff").toString());

	ignoreAction->setChecked(m_account->settings()->value("ChatWindow/ignoreSettings", false).toBool());

	QTextCharFormat fmt;
	fmt.setFont(defaultFont);
	fmt.setForeground(defaultFontColor);
	fmt.setBackground(defaultBkColor);

	mergeMyCharFormat(fmt);
}

void MessageEditor::writeSettings()
{
	if (m_account.isNull() || m_contact.isNull())
	{
		qDebug() << "MessageEditor::writeSettings() error: m_account = " << (void*)m_account << ", m_contact = " << (void*)m_contact;
		return;
	}

	QString settingsPrefix = "MessageEditor_" + m_contact->email() + '/';
	
	m_account->settings()->setValue(settingsPrefix + "font", lastUserFormat.font().toString());
	m_account->settings()->setValue(settingsPrefix + "textColor", lastUserFormat.foreground().color().name());
	if (lastUserFormat.background().style() == Qt::NoBrush)
		m_account->settings()->remove(settingsPrefix + "backgroundColor");
	else
		m_account->settings()->setValue(settingsPrefix + "backgroundColor", lastUserFormat.background().color().name());
	m_account->settings()->setValue("ChatWindow/ignoreSettings", ignoreAction->isChecked());
}

bool MessageEditor::isBlocked()
{
	return messageEdit->isReadOnly();
}

void MessageEditor::clear()
{
	messageEdit->blockSignals(true);
	messageEdit->clear();
	mergeMyCharFormat(lastUserFormat);
	updateFormatActions();
	messageEdit->blockSignals(false);
}

void MessageEditor::triggerEmoticonSelector()
{
	QDesktopWidget Screen;
	int screenWidth = Screen.screenGeometry().width();
	int screenHeight = Screen.screenGeometry().height();

	int XPos, YPos;

	if (emoticonSelector->geometry().width() + mapToGlobal(this->pos()).x() + 180 > screenWidth)
		XPos = mapToGlobal(this->pos()).x() + 30;
	else
		XPos = mapToGlobal(this->pos()).x() + 180;
	if (emoticonSelector->geometry().height() + mapToGlobal(this->pos()).y() + 30 > screenHeight)
		YPos = mapToGlobal(this->pos()).y() - 275;
	else
		YPos = mapToGlobal(this->pos()).y() + 30;
	emoticonSelector->setGeometry(XPos, YPos, 0, 0);
	emoticonSelector->appear(this, !emoticonSelector->isVisible());
}

void MessageEditor::triggerMultSelector()
{
	QDesktopWidget Screen;
	int screenWidth = Screen.screenGeometry().width();
	int screenHeight = Screen.screenGeometry().height();

	int XPos, YPos;

	if (multSelector->geometry().width() + mapToGlobal(this->pos()).x() + 180 > screenWidth)
		XPos = mapToGlobal(this->pos()).x() + 30;
	else
		XPos = mapToGlobal(this->pos()).x() + 180;
	if (multSelector->geometry().height() + mapToGlobal(this->pos()).y() + 30 > screenHeight)
		YPos = mapToGlobal(this->pos()).y() - 275;
	else
		YPos = mapToGlobal(this->pos()).y() + 30;
	multSelector->setGeometry(XPos, YPos, 0, 0);
	multSelector->appear(this, !multSelector->isVisible());
}

void MessageEditor::hideEvent(QHideEvent*)
{
	emoticonSelector->hide();
	multSelector->hide();
	smilesAction->setChecked(false);
	multsAction->setChecked(false);
}

void MessageEditor::updateFormatActions()
{
	if (messageEdit->fontWeight() == QFont::Bold)
		boldFontAction->setChecked(true);
	else
		boldFontAction->setChecked(false);
	
	italicFontAction->setChecked(messageEdit->fontItalic());
	underlinedFontAction->setChecked(messageEdit->fontUnderline());
}

QTextDocument * MessageEditor::document() const
{
	return messageEdit->document();
}

void MessageEditor::createFileTransferBar()
{
	fileTransferBar = new QToolBar;

	QLabel* ftLabel = new QLabel(this);

	ftLabel->setPixmap(QPixmap(":/icons/editor/msg_p_f_title.png"));
	filesBox = new QComboBox(this);
	filesBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	QWidget* helperWidget = new QWidget(this);
	QVBoxLayout* helperLayout = new QVBoxLayout;
	helperLayout->setSpacing(0);
	helperLayout->setContentsMargins(0,0,0,0);

	QHBoxLayout* labelsLayout = new QHBoxLayout;

	QLabel* label1 = new QLabel(tr("<small>Total size: </small>"));
	label1->setStyleSheet("QLabel { margin : 0px; border : 0px; padding : 0px }");
	bytesLabel = new QLabel;
	bytesLabel->setStyleSheet("QLabel { margin : 0px; border : 0px; padding : 0px }");
	//QLabel* label2 = new QLabel(tr("<small>&nbsp; bytes</small>"));
	//label2->setStyleSheet("QLabel { margin : 0px; border : 0px; padding : 0px }");
	
	totalSize = 0;
	bytesLabel->setText("<small>" + FileMessage::getSizeInString(totalSize) + "</small>");

	labelsLayout->addWidget(label1);
	labelsLayout->addWidget(bytesLabel);
	//labelsLayout->addWidget(label2);
	labelsLayout->addStretch();
	
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	
	plus = createFileTransferToolButton(fileTransferIcon("add"), this, SLOT(addFile()));
	minus = createFileTransferToolButton(fileTransferIcon("delete"), this, SLOT(deleteFile()));
	send = createFileTransferToolButton(fileTransferIcon("send"), this, SLOT(sendFiles()));

	minus->setEnabled(false);
	send->setEnabled(false);

	buttonsLayout->addWidget(plus);
	buttonsLayout->addWidget(minus);
	buttonsLayout->addSpacing(4);
	buttonsLayout->addWidget(send);
	buttonsLayout->addStretch();

	helperLayout->addLayout(labelsLayout);
	helperLayout->addLayout(buttonsLayout);

	helperWidget->setLayout(helperLayout);

	fileTransferBar->addWidget(ftLabel);
	fileTransferBar->addWidget(filesBox);
	fileTransferBar->addWidget(helperWidget);

	fileTransferBar->setVisible(false);

	fileProcessBar = new QToolBar;

	QLabel* ftLabel2 = new QLabel(this);
	
	ftLabel2->setPixmap(QPixmap(":/icons/editor/msg_p_f_title.png"));
	QWidget* indicatorWidget = new QWidget(this);
	QVBoxLayout* indicatorLayout = new QVBoxLayout;
	indicatorLayout->setSpacing(0);
	indicatorLayout->setContentsMargins(0,0,0,0);

	/*QWidget* cancelHelperWidget = new QWidget;
	QHBoxLayout* cancelHelperLayout = new QHBoxLayout;
	cancelHelperLayout->setContentsMargins(0, 0, 0, 0);
	cancel = createFileTransferToolButton(fileTransferIcon("cancel"), this, SLOT(cancelTransferring()));
	cancelHelperLayout->addWidget(cancel);
	cancelHelperWidget->setLayout(cancelHelperLayout);*/
	cancel = new QPushButton(tr("Cancel"));
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(cancelTransferring()));

	fileStatusLabel = new QLabel(tr("<small>Status...</small>"));
	fileProgress = new QProgressBar();

	indicatorLayout->addWidget(fileStatusLabel);
	indicatorLayout->addWidget(fileProgress);
	indicatorWidget->setLayout(indicatorLayout);

	fileProcessBar->addWidget(ftLabel2);
	fileProcessBar->addWidget(indicatorWidget);
	fileProcessBar->addWidget(cancel);

	fileProcessBar->setVisible(false);
}

QIcon MessageEditor::fileTransferIcon(const QString & toolName) const
{
	QIcon icon;
	
	icon.addPixmap(QPixmap(":/icons/editor/msg_p_f_" + toolName + "_dis.png"), QIcon::Disabled, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/editor/msg_p_f_" + toolName + "_d.png"), QIcon::Normal, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/editor/msg_p_f_" + toolName + "_h.png"), QIcon::Active, QIcon::Off);
	icon.addPixmap(QPixmap(":/icons/editor/msg_p_f_" + toolName + "_p.png"), QIcon::Active, QIcon::On);
	
	return icon;
}

QToolButton* MessageEditor::createFileTransferToolButton(const QIcon & icon, const QObject * receiver, const char * method)
{
	QSize size = icon.actualSize(QSize(256, 32));
	QAction* action = new QAction(icon, "", this);
	ToolButton* toolButton = new ToolButton(action, this);
	toolButton->setIconSize(size);
	connect(action, SIGNAL(triggered(bool)), receiver, method);

	return toolButton;
}

void MessageEditor::addFile()
{
	QStringList files =	QFileDialog::getOpenFileNames(this, tr("Select files to transfer"));

	QStringList::const_iterator it = files.begin();
	for (; it != files.end(); ++it)
	{
		int index = filesBox->findData(*it);
		if (index != -1)
			continue;

		QFileInfo finfo(*it);
		filesBox->addItem(finfo.fileName(), *it);
		fileList.append(finfo);
		totalSize += finfo.size();
	}

	qDebug() << "filesBox.sizeHint = " << filesBox->sizeHint();
	filesBox->resize(filesBox->sizeHint());

	if (filesBox->count() > 0)
	{
		minus->setEnabled(true);
		send->setEnabled(true);
	}

	bytesLabel->setText("<small>" + FileMessage::getSizeInString(totalSize) + "</small>");
}

void MessageEditor::deleteFile()
{
	totalSize -= fileList.takeAt(filesBox->currentIndex()).size();
	filesBox->removeItem(filesBox->currentIndex());
	if (filesBox->count() == 0)
	{
		minus->setEnabled(false);
		send->setEnabled(false);
	}

	bytesLabel->setText("<small>" + FileMessage::getSizeInString(totalSize) + "</small>");
}

void MessageEditor::fileTransfer(bool checked)
{
	if (!checked)
		cancelTransferring();
	fileTransferBar->setVisible(checked);
	fileTransferAction->setChecked(checked);
}

void MessageEditor::slotCurrentCharFormatChanged(const QTextCharFormat & f)
{
	qDebug() << "MessageEditor::slotCurrentCharFormatChanged type = " << f.objectType();
}

void MessageEditor::checkContactStatus(OnlineStatus status)
{
	wakeupButton->setEnabled(status.connected());
	multsAction->setEnabled(status.connected());
	fileTransferAction->setEnabled(status.connected());
	if (fileTransferAction->isChecked())
	{
		fileTransferBar->setVisible(status.connected());
		fileTransferAction->setChecked(false);
	}
}

void MessageEditor::messageEditorActivate()
{
	qDebug() << "MessageEditor::messagEditorActivate";
	messageEdit->setFocus();
}

void MessageEditor::setCheckSpelling(bool on)
{
	qDebug() << "setCheckSpelling" << on;
	messageEdit->setCheckSpelling(on);
	if (spellAction->isChecked() != on)
		spellAction->setChecked(on);
	
	m_account->settings()->setValue("MessageEditor/checkSpelling", spellAction->isChecked());
}

bool MessageEditor::event(QEvent* event)
{
	if (/*event->type() == 17 || */event->type() == 24) //If event type == QEvent::Show or QEvent::WindowActivate
		messageEdit->setFocus();
	return true;
}

void MessageEditor::cancelTransferring(quint32 sessId)
{
	qDebug() << "MessageEditor::cancelTransferring()";
	Q_EMIT transferringCancel();
}

void MessageEditor::sendFiles()
{
	qDebug() << "MessageEditor::sendFiles()";
	fileProgress->setValue(0);
	fileProgress->setMaximum(MAX_INT);
	fileProcessBar->setVisible(true);
	fileTransferBar->setVisible(false);

	Q_EMIT filesTransfer(fileList);
}

void MessageEditor::receiveFiles()
{
	qDebug() << Q_FUNC_INFO;

	fileTransferAction->setChecked(true);
	fileProcessBar->setVisible(true);
	fileTransferBar->setVisible(false);

	fileProgress->setMaximum(MAX_INT);
	fileProgress->setValue(0);

}

/*void MessageEditor::fileReceived(FileMessage* fmsg)
{
	qDebug() << "MessageEditor::fileReceived";
}
*/

void MessageEditor::slotProgress(FileMessage::Status action, int percentage)
{
	qDebug() << Q_FUNC_INFO;
	switch (action)
	{
		case FileMessage::TRANSFERRING_READY:
		case FileMessage::TRANSFERRING_FILE:
		case FileMessage::RECEIVING_FILE:
			qDebug() << "MessageEditor::file progress";
			fileProgress->setValue(percentage);
			break;

		case FileMessage::TRANSFERRING_COMPLETE:
			qDebug() << "MessageEditor::Transferring complete";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		case FileMessage::RECEIVING_COMPLETE:
			qDebug() << "MessageEditor::Receiving complete";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		case FileMessage::TRANSFER_ERROR:
			qDebug() << "MessageEditor::Transfering error";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		case FileMessage::RECEIVE_ERROR:
			qDebug() << "MessageEditor::Receiving error";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		case FileMessage::TRANSFER_CANCEL:
			qDebug() << "MessageEditor::Transfering canceled";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		case FileMessage::RECEIVE_CANCEL:
			qDebug() << "MessageEditor::Receiving cancelled";
			fileProcessBar->setVisible(false);
			fileTransferBar->setVisible(false);
			fileTransferAction->setChecked(false);
			break;

		default:
			break;

	}
}

bool MessageEditor::isIgnoreFont()
{
	return ignoreAction->isChecked();
}

void MessageEditor::slotIgnoreSet(bool ignore)
{
	ignoreAction->setChecked(ignore);
	writeSettings();
}
