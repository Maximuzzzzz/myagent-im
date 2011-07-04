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

#include "swfdecqtloader.h"

#include <QDebug>
#include <QHttp>
#include <QUrl>

// Qt4 proxy class =============================================================

class SwfdecQtLoaderProxy: public QObject
{
    Q_OBJECT
public:
    SwfdecQtLoaderProxy (SwfdecLoader *loader, QObject *parent = NULL);

    void load (bool post, const QUrl &url, const QByteArray &data);
    void close();

    void ensureOpen (const QHttpResponseHeader &response);

public Q_SLOTS:
    void push (const QHttpResponseHeader &response);
    void finished (bool error);

public:
    SwfdecLoader * const loader;
    QHttp *http;
    bool opened;
    QUrl url;
};

SwfdecQtLoaderProxy::SwfdecQtLoaderProxy (SwfdecLoader *loader,
        QObject *parent) :
    QObject (parent),
    loader (loader),
    http (new QHttp (this)),
    opened (false)
{
    connect (http, SIGNAL (readyRead(QHttpResponseHeader)),
             this, SLOT (push(QHttpResponseHeader)));
    connect (http, SIGNAL (done(bool)),
             this, SLOT (finished(bool)));
}

void SwfdecQtLoaderProxy::ensureOpen (const QHttpResponseHeader &response)
{
    if (opened)
        return;

    swfdec_loader_set_url (loader, url.toEncoded());
    if (response.hasContentLength())
        swfdec_loader_set_size (loader, response.contentLength());
    swfdec_stream_open (SWFDEC_STREAM (loader));
    opened = true;
}

void SwfdecQtLoaderProxy::push (const QHttpResponseHeader &response)
{
    ensureOpen (response);

    while (const unsigned size =
            qMin (qint64 (UINT_MAX), http->bytesAvailable())) {
//        qDebug() << Q_FUNC_INFO << size;

        SwfdecBuffer * const buffer = swfdec_buffer_new (size);
        http->read (reinterpret_cast<char*> (buffer->data), size);
        swfdec_stream_push (SWFDEC_STREAM (loader), buffer);
    }
}

void SwfdecQtLoaderProxy::finished (bool error)
{
//    qDebug() << Q_FUNC_INFO << error;

    if (!error) {
        ensureOpen (http->lastResponse());
        swfdec_stream_eof (SWFDEC_STREAM (loader));
    } else {
        swfdec_stream_error (SWFDEC_STREAM (loader),
                "%s", http->errorString().toUtf8().data());
    }
}

void SwfdecQtLoaderProxy::load (bool post, const QUrl &url,
        const QByteArray &data)
{
//    qDebug() << Q_FUNC_INFO << url.toString();

    this->url = url;
    QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ?
        QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost (url.host(), mode, url.port() == -1 ? 0 : url.port());
    if (!url.userName().isEmpty())
        http->setUser (url.userName(), url.password());
    if (!post)
        http->get (url.path());
    else
        http->post (url.path(), data);
}

void SwfdecQtLoaderProxy::close()
{
//    qDebug() << Q_FUNC_INFO;

    http->abort();
}

// GObject stuff ===============================================================

struct _SwfdecQtLoader
{
    SwfdecLoader loader;

    SwfdecQtLoaderProxy *proxy;
};

struct _SwfdecQtLoaderClass
{
    SwfdecLoaderClass loaderClass;
};

G_DEFINE_TYPE (SwfdecQtLoader, swfdec_qt_loader, SWFDEC_TYPE_FILE_LOADER)

static void swfdec_qt_loader_load (SwfdecLoader *loader, SwfdecPlayer *player,
    const char *url_string, SwfdecLoaderRequest request, SwfdecBuffer *buffer)
{
    SwfdecURL *url;

    if (swfdec_url_path_is_relative (url_string)) {
        url = swfdec_url_new_relative (swfdec_player_get_base_url (player), url_string);
    } else {
        url = swfdec_url_new (url_string);
    }

    if (url == NULL) {
        swfdec_stream_error (SWFDEC_STREAM (loader), "invalid URL %s", url_string);
        return;
    };

    if (!swfdec_url_has_protocol (url, "http") &&
        !swfdec_url_has_protocol (url, "https")) {
        SWFDEC_LOADER_CLASS (swfdec_qt_loader_parent_class)->load (loader,
                player, url_string, request, buffer);
        swfdec_url_free (url);
        return;
    }

    SwfdecQtLoader *qtLoader = SWFDEC_QT_LOADER (loader);

    QByteArray data = buffer == NULL ?
        QByteArray() : QByteArray ((char *) buffer->data, buffer->length);

    qtLoader->proxy->load (request == SWFDEC_LOADER_REQUEST_POST,
            QUrl (QString::fromUtf8 (swfdec_url_get_url (url))), data);

    swfdec_url_free (url);
}

static void swfdec_qt_loader_close (SwfdecStream *stream)
{
    SwfdecQtLoader *qtLoader = SWFDEC_QT_LOADER (stream);

    qtLoader->proxy->close();
}

static void swfdec_qt_loader_init (SwfdecQtLoader *qtLoader)
{
    qtLoader->proxy = new SwfdecQtLoaderProxy (SWFDEC_LOADER (qtLoader));
}

static void swfdec_qt_loader_dispose (GObject *object)
{
    SwfdecQtLoader *qtLoader = SWFDEC_QT_LOADER (object);

    delete qtLoader->proxy;

    G_OBJECT_CLASS (swfdec_qt_loader_parent_class)->dispose (object);
}

static void swfdec_qt_loader_class_init (SwfdecQtLoaderClass *klass)
{
    GObjectClass *objectClass = G_OBJECT_CLASS (klass);
    SwfdecStreamClass *streamClass = SWFDEC_STREAM_CLASS (klass);
    SwfdecLoaderClass *loaderClass = SWFDEC_LOADER_CLASS (klass);

    objectClass->dispose = swfdec_qt_loader_dispose;
    streamClass->close = swfdec_qt_loader_close;
    loaderClass->load = swfdec_qt_loader_load;
}

//SwfdecLoader *swfdec_qt_loader_new (const char *uri)
//{
//    // Most of this stuff should be in init TODO
//    // http://blogs.gnome.org/johannes/2007/02/02/use-construct-properties/
//    SwfdecLoader *loader;
//    SwfdecURL *url;
//
//    g_return_val_if_fail (uri != NULL, NULL);
//
//    url = swfdec_url_new (uri);
//    loader = SWFDEC_LOADER (g_object_new (SWFDEC_TYPE_QT_LOADER,
//                "url", url, NULL));
//    swfdec_url_free (url);
//    swfdec_qt_loader_load (loader, NULL, SWFDEC_LOADER_REQUEST_DEFAULT, NULL, 0);
//    return loader;
//}

#include "swfdecqtloader.moc"
