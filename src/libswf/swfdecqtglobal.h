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

#ifndef _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTGLOBAL_H_
#define _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTGLOBAL_H_

#include <QtGlobal>

#define SWFDEC_QT4_GCC_VERSION (__GNUC__ * 100                              \
                              + __GNUC_MINOR__ * 10                         \
                              + __GNUC_PATCHLEVEL)

#if defined (Q_CC_MSVC) || defined (Q_OS_WIN32)
    #define SWFDEC_QT4_EXPORT_DECL __declspec (dllexport)
    #define SWFDEC_QT4_IMPORT_DECL __declspec (dllimport)
#else
    // Test for GCC >= 4.0.0
    #if SWFDEC_QT4_GCC_VERSION >= 400
        #define SWFDEC_QT4_EXPORT_DECL __attribute__ ((visibility ("default")))
        #define SWFDEC_QT4_IMPORT_DECL
    #else
        #define SWFDEC_QT4_EXPORT_DECL
        #define SWFDEC_QT4_IMPORT_DECL
    #endif
#endif

#if defined (Q_CC_MSVC)
    #define SWFDEC_QT4_DEPRECATED __declspec (deprecated)
#else
    #define SWFDEC_QT4_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifdef SWFDEC_QT4_MAKEDLL
    #define SWFDEC_QT4_EXPORT SWFDEC_QT4_EXPORT_DECL
#else
    #define SWFDEC_QT4_EXPORT SWFDEC_QT4_IMPORT_DECL
#endif

#endif

