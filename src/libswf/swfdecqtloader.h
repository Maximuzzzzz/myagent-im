/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 * Copyright (C) 2006  Benjamin Otte <otte@gnome.org>                         *
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

#ifndef _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTLOADER_H_
#define _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <swfdec/swfdec.h>

#ifdef __cplusplus
}
#endif

G_BEGIN_DECLS

typedef struct _SwfdecQtLoader SwfdecQtLoader;
typedef struct _SwfdecQtLoaderClass SwfdecQtLoaderClass;

#define SWFDEC_TYPE_QT_LOADER (swfdec_qt_loader_get_type())
#define SWFDEC_IS_QT_LOADER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                SWFDEC_TYPE_QT_LOADER))
#define SWFDEC_IS_QT_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                SWFDEC_TYPE_QT_LOADER))
#define SWFDEC_QT_LOADER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                SWFDEC_TYPE_QT_LOADER, SwfdecQtLoader))
#define SWFDEC_QT_LOADER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
                SWFDEC_TYPE_QT_LOADER, SwfdecQtLoaderClass))
#define SWFDEC_QT_LOADER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                SWFDEC_TYPE_QT_LOADER, SwfdecQtLoaderClass))

GType swfdec_qt_loader_get_type();
//SwfdecLoader *swfdec_qt_loader_new (const char *uri);

G_END_DECLS

#endif
