/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTEXCEPTION_H_
#define _LIBSWFDEC_QT4_SRC_LIB_SWFDECQTEXCEPTION_H_

#include "swfdecqtglobal.h"

#include <QByteArray>
#include <QString>

/** Exception that provides a simple error message. All derived classes must be
 * marked with SWFDEC_QT4_EXPORT, otherwise exceptions will just terminate()!
 */
class SWFDEC_QT4_EXPORT SwfdecQtException: public std::exception
{
public:
    /** Creates a new instance with the given error message. Please use a
     * meaningful description that makes it possible to isolate the error
     * position. Converts the QString to the local encoding with
     * QString#toLocal8Bit() before saving it as a char array that will be
     * returned by #what().
     *
     * @param message error message
     *
     * @see what()
     */
    SwfdecQtException (const QString &message) throw() :
        message (message.toLocal8Bit())
    {
    }

    /** Virtual destructor to make the compiler happy. */
    virtual ~SwfdecQtException() throw()
    {
    }

    /** Returns the error message.
     *
     * @return error message
     */
    virtual const char* what() const throw()
    {
        return message;
    }

private:
    const QByteArray message;
};

#endif
