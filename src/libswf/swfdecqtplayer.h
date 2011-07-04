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

#ifndef _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTPLAYER_H_
#define _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTPLAYER_H_

#include "swfdecqtglobal.h"

#include <QColor>
#include <QImage>
#include <QObject>

class QUrl;

class SwfdecQtPlayerPrivate;

class SWFDEC_QT4_EXPORT SwfdecQtPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool playing READ isPlaying WRITE setPlaying)
    Q_PROPERTY (bool initialized READ isInitialized)
    Q_PROPERTY (SwfdecMouseCursor mouseCursor READ mouseCursor)
//    Q_PROPERTY (QSize imageSize READ imageSize)
    Q_PROPERTY (QSize size READ size WRITE setSize)
    Q_PROPERTY (QColor backgroundColor
            READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY (QSize size READ size WRITE setSize)

    friend class SwfdecQtPlayerPrivate;

public:
    SwfdecQtPlayer (const QString &uri, const QByteArray &variables,
            QObject *parent = NULL);
    ~SwfdecQtPlayer();

    enum SwfdecMouseCursor {
          MouseCursorNormal,
          MouseCursorNone,
          MouseCursorText,
          MouseCursorClick
    };

public Q_SLOTS:
    void setPlaying (bool playing);
    void nextFrame();

public:
    bool isPlaying() const;

    bool isInitialized() const;
    SwfdecMouseCursor mouseCursor() const;
//    QSize imageSize() const;

    void setSize (const QSize &size);
    QSize size() const;

    void setBackgroundColor (const QColor &color);
    QColor backgroundColor() const;

    QImage render (const QRect &rect) const;
    bool mouseMove (int x, int y);
    bool mousePress (int x, int y, int button);
    bool mouseRelease (int x, int y, int button);
    bool keyPress (int keyCode, const QString &text);
    bool keyRelease (int keyCode, const QString &text);

Q_SIGNALS:
    void invalidate (const QRect &rect);
    void invalidate (const QRegion &region);
    void initialized();
    void mouseCursorChanged();
    void playingChanged (bool playing);
	void unknownSignal(QString name);

private:
    SwfdecQtPlayerPrivate * const d;
};

#endif
