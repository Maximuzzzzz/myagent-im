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

#ifndef _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTWIDGET_H_
#define _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTWIDGET_H_

#include "swfdecqtglobal.h"

#include <QWidget>

class SwfdecQtPlayer;

class SwfdecQtWidgetPrivate;

class SWFDEC_QT4_EXPORT SwfdecQtWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY (bool interactive READ isInteractive WRITE setInteractive)
    Q_PROPERTY (SwfdecQtPlayer *player READ player WRITE setPlayer)
    friend class SwfdecQtWidgetPrivate;
public:
    SwfdecQtWidget (QWidget *parent = NULL);
    ~SwfdecQtWidget();

    SwfdecQtPlayer *player() const;
    void setPlayer (SwfdecQtPlayer *player);

    bool isInteractive() const;
    void setInteractive (bool interactive);

    virtual QSize sizeHint() const;

protected:
    virtual void resizeEvent (QResizeEvent *event);
    virtual void paintEvent (QPaintEvent *event);
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void keyReleaseEvent (QKeyEvent *event);

private:
    SwfdecQtWidgetPrivate * const d;
};

#endif

