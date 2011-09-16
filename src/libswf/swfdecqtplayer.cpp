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

#include "swfdecqtexception.h"
#include "swfdecqtkeys.h"
#include "swfdecqtloader.h"
#include "swfdecqtplayer.h"

#include <swfdec/swfdec.h>

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTimer>

class SwfdecQtPlayerPrivate : public QObject
{
    Q_OBJECT
public:
    SwfdecQtPlayerPrivate (SwfdecQtPlayer *p);

    qint64 msecsToNextEvent() const;
    void setTimer (qint64 msecs);

    static void notifyCallback (SwfdecPlayer *swfdec, GParamSpec *pspec,
            SwfdecQtPlayer *player);
	static void invalidateCallback (SwfdecPlayer *swfdec,
            const SwfdecRectangle *extents, const SwfdecRectangle *rectangles,
            guint n_rectangles, SwfdecQtPlayer *player);
    static void advanceCallback (SwfdecPlayer *swfdec, unsigned milliSeconds,
            unsigned audioFrames, SwfdecQtPlayer *player);

    static qint64 diff (const QDateTime &first, const QDateTime &last);
    static QRect rect (const SwfdecRectangle *rect);

public Q_SLOTS:
    void timer();

public:
    SwfdecQtPlayer * const p;
    bool playing;
    SwfdecPlayer *player;
    QDateTime lastEvent;
};

// SwfdecQtPlayerPrivate =======================================================

SwfdecQtPlayerPrivate::SwfdecQtPlayerPrivate (SwfdecQtPlayer *p) :
    p (p)
{
}

qint64 SwfdecQtPlayerPrivate::diff (const QDateTime &first,
        const QDateTime &last)
{
    const QDateTime utcFirst = first.toUTC();
    const QDateTime utcLast = last.toUTC();
    const qint64 days = utcFirst.date().daysTo (utcLast.date());
    return days * 24 * 60 * 60 * 1000 +
        utcFirst.time().msecsTo (utcLast.time());
}

qint64 SwfdecQtPlayerPrivate::msecsToNextEvent() const
{
    const glong swfDiff = swfdec_player_get_next_event (player);
    // stop playing
    if (swfDiff < 0)
        return LLONG_MIN;
    const qint64 elapsed = diff (lastEvent, QDateTime::currentDateTime());
    return swfDiff - elapsed;
}

QRect SwfdecQtPlayerPrivate::rect (const SwfdecRectangle *rect)
{
    return QRectF (rect->x, rect->y, rect->width, rect->height).toAlignedRect();
}

void SwfdecQtPlayerPrivate::advanceCallback (SwfdecPlayer *swfdec,
        unsigned milliSeconds, unsigned audioFrames, SwfdecQtPlayer *player)
{
    Q_UNUSED (swfdec);
    Q_UNUSED (audioFrames);

//    qDebug() << Q_FUNC_INFO << milliSeconds;

    player->d->lastEvent = player->d->lastEvent.addMSecs (milliSeconds);
}

void SwfdecQtPlayerPrivate::notifyCallback (SwfdecPlayer *swfdec,
        GParamSpec *pspec, SwfdecQtPlayer *player)
{
    Q_UNUSED (swfdec);

//	qDebug() << Q_FUNC_INFO << pspec->name;

    if (g_str_equal (pspec->name, "mouse-cursor"))
        Q_EMIT player->mouseCursorChanged();
	else if (g_str_equal (pspec->name, "initialized"))
        Q_EMIT player->initialized();
	else
		Q_EMIT player->unknownSignal(QString(pspec->name));
}

void SwfdecQtPlayerPrivate::invalidateCallback (SwfdecPlayer *swfdec,
        const SwfdecRectangle *extents, const SwfdecRectangle *rectangles,
        guint n_rectangles, SwfdecQtPlayer *player)
{
    Q_UNUSED (swfdec);

//    qDebug() << Q_FUNC_INFO << rect (extents);

    QRegion region;
    for (unsigned i = 0; i < n_rectangles; ++i)
        region += rect ((SwfdecRectangle *) &rectangles[i]);
    Q_EMIT player->invalidate (region);

    Q_EMIT player->invalidate (rect (extents));
}

void SwfdecQtPlayerPrivate::timer()
{
    if (!playing)
        return;

    Q_FOREVER {
        const qint64 msecs = msecsToNextEvent();
        if (msecs == LLONG_MIN) {
//            qDebug() << Q_FUNC_INFO << "stop";
            p->setPlaying (false);
            return;
        }
        if (msecs > 0) {
//            qDebug() << Q_FUNC_INFO << "wait" << msecs;
            setTimer (msecs);
            return;
        }
//        qDebug() << Q_FUNC_INFO << "advance";
        swfdec_player_advance (player,
                swfdec_player_get_next_event (player) - msecs);
    }
}

void SwfdecQtPlayerPrivate::setTimer (qint64 msecs)
{
//    qDebug() << Q_FUNC_INFO << msecs;
    QTimer::singleShot (msecs, this, SLOT (timer()));
}

// SwfdecQtPlayer ================================================================

SwfdecQtPlayer::SwfdecQtPlayer (const QString &uri, const QByteArray &variables,
        QObject *parent) :
    QObject (parent),
    d (new SwfdecQtPlayerPrivate (this))
{
    d->playing = false;

    // TODO uri

    swfdec_init();

    SwfdecURL *url = swfdec_url_new (uri.toUtf8());
    if (url == NULL)
        throw SwfdecQtException (tr ("Unable to convert URI %1").arg (uri));

    d->player = SWFDEC_PLAYER (g_object_new (SWFDEC_TYPE_PLAYER,
        "loader-type", SWFDEC_TYPE_QT_LOADER,
        NULL));
    swfdec_player_set_variables (d->player, variables);
    swfdec_player_set_url (d->player, url);
    swfdec_url_free (url);

    g_signal_connect (d->player, "invalidate",
            G_CALLBACK (SwfdecQtPlayerPrivate::invalidateCallback), this);
    g_signal_connect (d->player, "notify",
            G_CALLBACK (SwfdecQtPlayerPrivate::notifyCallback), this);
    g_signal_connect (d->player, "advance",
            G_CALLBACK (SwfdecQtPlayerPrivate::advanceCallback), this);
}

SwfdecQtPlayer::~SwfdecQtPlayer()
{
    g_object_unref (d->player);
    delete d;
}

//QSize SwfdecQtPlayer::imageSize() const
//{
//    int playerWidth, playerHeight;
//    swfdec_player_get_image_size (d->player, &playerWidth, &playerHeight);
//    return QSize (playerWidth, playerHeight);
//}

void SwfdecQtPlayer::setSize (const QSize &size)
{
    swfdec_player_set_size (d->player, size.width(), size.height());
}

QSize SwfdecQtPlayer::size() const
{
    int playerWidth, playerHeight;
    swfdec_player_get_size (d->player, &playerWidth, &playerHeight);
    return QSize (playerWidth, playerHeight);
}

bool SwfdecQtPlayer::isPlaying() const
{
    return d->playing;
}

bool SwfdecQtPlayer::isInitialized() const
{
    return swfdec_player_is_initialized (d->player);
}

void SwfdecQtPlayer::setPlaying (bool playing)
{
    if (!d->playing && playing) {
        if (!isInitialized())
            nextFrame();
        d->playing = true;
        d->lastEvent = QDateTime::currentDateTime();
        d->setTimer (0);
    } else if (d->playing && !playing) {
        d->playing = false;
    } else {
        return;
    }
    Q_EMIT playingChanged (d->playing);
}

void SwfdecQtPlayer::setBackgroundColor (const QColor &color)
{
    swfdec_player_set_background_color (d->player, color.rgba());
}

QColor SwfdecQtPlayer::backgroundColor() const
{
    return QColor (swfdec_player_get_background_color (d->player));
}

void SwfdecQtPlayer::nextFrame()
{
    if (d->playing)
        return;

    swfdec_player_advance (d->player, swfdec_player_get_next_event (d->player));
}

QImage SwfdecQtPlayer::render (const QRect &rect) const
{
    QImage image (rect.width(), rect.height(),
            QImage::Format_ARGB32_Premultiplied);

    cairo_surface_t *surface = cairo_image_surface_create_for_data
        (image.bits(), CAIRO_FORMAT_ARGB32, image.width(), image.height(),
         image.bytesPerLine());
    cairo_t *cr = cairo_create (surface);
    cairo_translate (cr, -rect.x(), -rect.y());
    swfdec_player_render (d->player, cr, rect.x(), rect.y(), rect.width(),
            rect.height());
    cairo_show_page (cr);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return image;
}

bool SwfdecQtPlayer::mouseMove (int x, int y)
{
    return swfdec_player_mouse_move (d->player, x, y);
}

bool SwfdecQtPlayer::mousePress (int x, int y, int button)
{
    return swfdec_player_mouse_press (d->player, x, y, button);
}

bool SwfdecQtPlayer::mouseRelease (int x, int y, int button)
{
    return swfdec_player_mouse_release (d->player, x, y, button);
}

bool SwfdecQtPlayer::keyPress (int keyCode, const QString &text)
{
    const QVector<unsigned> characters = text.toUcs4();
    const unsigned character = characters.isEmpty() ? 0 : characters[0];
    const SwfdecKey key = swfdecQtKeyMapping (keyCode);
    if (!key)
        qDebug() << Q_FUNC_INFO << "Key not found:" << keyCode;
    return swfdec_player_key_press (d->player, key, character);
}

bool SwfdecQtPlayer::keyRelease (int keyCode, const QString &text)
{
    const QVector<unsigned> characters = text.toUcs4();
    const unsigned character = characters.isEmpty() ? 0 : characters[0];
    const SwfdecKey key = swfdecQtKeyMapping (keyCode);
    if (!key)
        qDebug() << Q_FUNC_INFO << "Key not found:" << keyCode;
    return swfdec_player_key_release (d->player, key, character);
}

SwfdecQtPlayer::SwfdecMouseCursor SwfdecQtPlayer::mouseCursor() const
{
    SwfdecQtPlayer::SwfdecMouseCursor result;
    g_object_get (d->player, "mouse-cursor", &result, NULL);
    return result;
}

#include "swfdecqtplayer.moc"
