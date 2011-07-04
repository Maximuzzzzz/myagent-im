/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This library is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or     *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with this program; if not, write to the Free Software Foundation,    *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.          *
 ******************************************************************************/

#include "swfdecqtwidget.h"
#include "swfdecqtplayer.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

class SwfdecQtWidgetPrivate : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void invalidate (const QRegion &region);
    void initialized();
    void updateMouseCursor();

public:
    SwfdecQtWidget *p;
    SwfdecQtPlayer *player;
    bool interactive;
};

// SwfdecQtWidgetPrivate =======================================================

void SwfdecQtWidgetPrivate::invalidate (const QRegion &region)
{
    p->update (region);
}

void SwfdecQtWidgetPrivate::initialized()
{
//    qDebug() << Q_FUNC_INFO;

    p->updateGeometry();

    player->setSize (p->size());
}

void SwfdecQtWidgetPrivate::updateMouseCursor()
{
    SwfdecQtPlayer::SwfdecMouseCursor cursor = player && interactive ?
        player->mouseCursor() : SwfdecQtPlayer::MouseCursorNormal;

//    qDebug() << Q_FUNC_INFO << cursor;

    switch (cursor) {
    case SwfdecQtPlayer::MouseCursorNone:
        p->setCursor (Qt::BlankCursor);
        break;
    case SwfdecQtPlayer::MouseCursorText:
        p->setCursor (Qt::IBeamCursor);
        break;
    case SwfdecQtPlayer::MouseCursorClick:
        p->setCursor (Qt::PointingHandCursor);
        break;
    case SwfdecQtPlayer::MouseCursorNormal:
    default:
        p->setCursor (Qt::ArrowCursor);
        break;
    }
}

// SwfdecQtWidget ================================================================

SwfdecQtWidget::SwfdecQtWidget (QWidget *parent) :
    QWidget (parent),
    d (new SwfdecQtWidgetPrivate)
{
    d->p = this;
    d->player = NULL;
    d->interactive = false;

    setMouseTracking (true);
}

SwfdecQtWidget::~SwfdecQtWidget()
{
    delete d;
}

SwfdecQtPlayer *SwfdecQtWidget::player() const
{
    return d->player;
}

void SwfdecQtWidget::setPlayer (SwfdecQtPlayer *player)
{
    if (d->player)
        d->player->disconnect (this);

    d->player = player;

    if (player) {
        connect (player, SIGNAL (invalidate(QRegion)),
                 d, SLOT (invalidate(QRegion)));
        connect (player, SIGNAL (initialized()),
                 d, SLOT (initialized()));
        connect (player, SIGNAL (mouseCursorChanged()),
                 d, SLOT (updateMouseCursor()));
    }
    d->updateMouseCursor();
    updateGeometry();
    update();
}

bool SwfdecQtWidget::isInteractive() const
{
    return d->interactive;
}

void SwfdecQtWidget::setInteractive (bool interactive)
{
    d->interactive = interactive;
    setFocusPolicy (interactive ? Qt::StrongFocus : Qt::NoFocus);
    d->updateMouseCursor();
}

QSize SwfdecQtWidget::sizeHint() const
{
    if (!d->player)
        return QSize();

//    qDebug() << Q_FUNC_INFO << d->player->imageSize();
//
//    TODO
//    return d->player->imageSize();
    return d->player->size();
}

void SwfdecQtWidget::paintEvent (QPaintEvent *event)
{
    const QRect rect = event->rect();

//    qDebug() << Q_FUNC_INFO << rect;

//    TODO: initial expose

    QPainter painter (this);

    if (!d->player || !d->player->isInitialized()) {
        painter.fillRect (rect, Qt::white);
    } else {
        painter.drawImage (rect.x(), rect.y(), d->player->render (rect));
//        painter.setPen (QColor (0, 0, 255, 30));
//        painter.drawRect (rect.adjusted (0, 0, -1, -1));
    }
}

void SwfdecQtWidget::resizeEvent (QResizeEvent *event)
{
//    qDebug() << Q_FUNC_INFO << event->size();

    if (d->player && d->player->isInitialized())
        d->player->setSize (event->size());

    QWidget::resizeEvent (event);
}

void SwfdecQtWidget::keyPressEvent (QKeyEvent *event)
{
    if (d->player && d->interactive && !event->isAutoRepeat() &&
        d->player->keyPress (event->key(), event->text())) {
        event->setAccepted (true);
        return;
    }

    QWidget::keyPressEvent (event);
}

void SwfdecQtWidget::keyReleaseEvent (QKeyEvent *event)
{
    if (d->player && d->interactive && !event->isAutoRepeat() &&
        d->player->keyRelease (event->key(), event->text())) {
        event->setAccepted (true);
        return;
    }

    QWidget::keyReleaseEvent (event);
}

void SwfdecQtWidget::mousePressEvent (QMouseEvent *event)
{
    // TODO: different buttons
    if (event->button() == Qt::LeftButton &&
            d->player && d->interactive)
        d->player->mousePress (event->x(), event->y(), 1);

    QWidget::mousePressEvent (event);
}

void SwfdecQtWidget::mouseReleaseEvent (QMouseEvent *event)
{
    // TODO: different buttons
    if (event->button() == Qt::LeftButton &&
            d->player && d->interactive)
        d->player->mouseRelease (event->x(), event->y(), 1);

    QWidget::mouseReleaseEvent (event);
}

void SwfdecQtWidget::mouseMoveEvent (QMouseEvent *event)
{
    if (d->player && d->interactive)
        d->player->mouseMove (event->x(), event->y());

    QWidget::mouseMoveEvent (event);
}

#include "swfdecqtwidget.moc"
