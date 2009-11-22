#include "favouriteemoticonsdialog.h"

#include <cmath>

#include <QDebug>

#include <QVBoxLayout>
#include <QToolBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QStyle>
#include <QDragEnterEvent>
#include <QPainter>

#include "resourcemanager.h"
#include "emoticonwidget.h"
#include "flowlayout.h"

FavouriteEmoticonsDialog::FavouriteEmoticonsDialog()
{
	setWindowTitle(tr("Select favourite emoticons"));
	
	QVBoxLayout* layout = new QVBoxLayout;
	QHBoxLayout* topLayout = new QHBoxLayout;

	QToolBox* toolBox = new QToolBox;

	int maxSetSize = theRM.emoticons().maxSetSize();
	int emoticonsPerRow = static_cast<int>(ceil(sqrt((double)maxSetSize)));

	Emoticons::const_iterator set_it = theRM.emoticons().begin();
	Emoticons::const_iterator setEnd_it = theRM.emoticons().end();

	int nEmoticons = 0;

	for (; set_it != setEnd_it; ++set_it)
	{
		EmoticonSet::const_iterator info_it = (*set_it)->begin();
		EmoticonSet::const_iterator infoEnd_it = (*set_it)->end();

		QWidget* setWidget = new QWidget;
		QVBoxLayout* outerLayout = new QVBoxLayout;
		QGridLayout* setLayout = new QGridLayout;
		setLayout->setSpacing(2);

		int row = 0;
		int col = 0;

		for (; info_it != infoEnd_it; ++info_it)
		{
			EmoticonWidget* w = new EmoticonWidget((*info_it)->id(), this);
			w->setDragEnabled(true);
			w->setToolTip((*info_it)->tip());
			setLayout->addWidget(w, row, col);
			if (col == emoticonsPerRow)
			{
				col = 0;
				row++;
			}
			else
				col++;

			nEmoticons++;
		}
		outerLayout->addLayout(setLayout);
		outerLayout->addStretch();

		setWidget->setLayout(outerLayout);
		toolBox->addItem(setWidget, (*set_it)->title());
	}
	toolBox->setFixedWidth(toolBox->sizeHint().width());

	favouriteEmoticonsWidget = new FavoritesEmoticonsWidget(nEmoticons);

	topLayout->addWidget(toolBox);
	topLayout->addWidget(favouriteEmoticonsWidget);
	
	QLabel* helpLabel = new QLabel(tr("Drag emoticons to add and rearrange them. To remove emoticon from favourites drag it outside the box."));
	helpLabel->setWordWrap(true);
	QFrame* line = new QFrame;
	line->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
	
	layout->addLayout(topLayout);
	layout->addWidget(helpLabel);
	layout->addWidget(line);
	layout->addWidget(buttonBox);

	setLayout(layout);
	setFixedSize(sizeHint());
}

void FavouriteEmoticonsDialog::slotAccepted()
{
	qDebug() << Q_FUNC_INFO;
	theRM.emoticons().setFavouriteEmoticons(favouriteEmoticonsWidget->favouriteEmoticons());
	accept();
}

// ---------------------------------------------------------------------------------------------

FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::FavoritesEmoticonsWidget(int totalEmoticons)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	setAcceptDrops(true);

	FlowLayout* layout = new FlowLayout(6);

	foreach (QString emoticonId, theRM.emoticons().favouriteEmoticons())
	{
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(emoticonId);

		EmoticonWidget* w = new EmoticonWidget(info->id(), this);
		connect(w, SIGNAL(draggingStarted()), this, SLOT(emoticonDraggingStarted()));
		w->setDragEnabled(true);
		w->setToolTip(info->tip());
		layout->addWidget(w);
	}

	setLayout(layout);

	int emoticonsInRow = static_cast<int>(ceil(sqrt((double)totalEmoticons)));
	int emoticonsWidgetWidth = layout->itemAt(0)->widget()->width();
	int left, top, right, bottom;
	layout->getContentsMargins ( &left, &top, &right, &bottom );
	int size = emoticonsWidgetWidth*emoticonsInRow + 6*(emoticonsInRow - 1) + left + right + 1;

	setFixedSize(size, size);
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::dragEnterEvent(QDragEnterEvent * event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasFormat("Myagent-IM/emoticon"))
	{
		QString id = mimeData->data("Myagent-IM/emoticon");
		for (int i = 0; i < layout()->count(); i++)
		{
			EmoticonWidget* w = qobject_cast<EmoticonWidget*>(layout()->itemAt(i)->widget());
			if (w && w->isVisible() && w->emoticonId() == id)
				return;
		}
	}

	event->acceptProposedAction();
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::emoticonDraggingStarted()
{
	EmoticonWidget* w = qobject_cast<EmoticonWidget*>(sender());
	layout()->removeWidget(w);
	w->hide();
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::dragLeaveEvent ( QDragLeaveEvent * event )
{
	Q_UNUSED(event)
	cursorRect = QRect();
	update();
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::dropEvent(QDropEvent * event)
{
	qDebug() << Q_FUNC_INFO;
	EmoticonWidget* w = qobject_cast<EmoticonWidget*>(event->source());
	if (!w)
	{
		event->ignore();
		return;
	}

	FlowLayout* l = static_cast<FlowLayout*>(layout());
	int idx = l->indexOfItemAtPos(event->pos());

	QLayoutItem* layoutItem = l->itemAt(idx);
	if (layoutItem)
	{
		QRect itemRect = layoutItem->geometry();
		if (event->pos().x() >= itemRect.center().x())
			idx++;
	}

	if (w->parentWidget() == this)
	{
		l->insertWidget(idx, w);
		w->show();
	}
	else
	{
		w = new EmoticonWidget(w->emoticonId(), this);
		connect(w, SIGNAL(draggingStarted()), this, SLOT(emoticonDraggingStarted()));
		w->setDragEnabled(true);
		const EmoticonInfo* info = theRM.emoticons().getEmoticonInfo(w->emoticonId());
		w->setToolTip(info->tip());
		l->insertWidget(idx, w);
	}

	cursorRect = QRect();
	update();
	event->accept();
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::dragMoveEvent(QDragMoveEvent * event)
{
	FlowLayout* l = static_cast<FlowLayout*>(layout());
	int idx = l->indexOfItemAtPos(event->pos());
	qDebug() << Q_FUNC_INFO << "idx = " << idx;

	int cursorX;
	int cursorY;
	int cursorHeight = 0;

	QLayoutItem* layoutItem = l->itemAt(idx);
	if (!layoutItem)
	{
		layoutItem = l->itemAt(idx-1);
		QRect itemRect = layoutItem->geometry();
		cursorX = itemRect.x() + itemRect.width() + l->spacing()/2;
		cursorY = itemRect.y();
		cursorHeight = itemRect.height();
	}
	else
	{
		QRect itemRect = layoutItem->geometry();
		if (event->pos().x() < itemRect.center().x())
			cursorX = itemRect.x() - l->spacing()/2;
		else
			cursorX = itemRect.x() + itemRect.width() + l->spacing()/2;

		cursorY = itemRect.y();
		cursorHeight = itemRect.height();
	}

	cursorRect.setRect(cursorX, cursorY, l->spacing()/2, cursorHeight);
	update();
}

void FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::paintEvent(QPaintEvent * event)
{
	QFrame::paintEvent(event);
	QPainter p(this);
	p.fillRect(cursorRect, Qt::black);
}

QStringList FavouriteEmoticonsDialog::FavoritesEmoticonsWidget::favouriteEmoticons() const
{
	QStringList result;

	FlowLayout* l = static_cast<FlowLayout*>(layout());
	for (int i = 0; i < l->count(); i++)
	{
		QLayoutItem* layoutItem = l->itemAt(i);
		EmoticonWidget* w = qobject_cast<EmoticonWidget*>(layoutItem->widget());
		result << w->emoticonId();
	}

	return result;
}
