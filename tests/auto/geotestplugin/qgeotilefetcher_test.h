/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOTILEFETCHER_TEST_H
#define QGEOTILEFETCHER_TEST_H

#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QtLocation/private/qgeotilespec_p.h>

#include <QLocale>
#include <QPainter>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include <QDebug>
#include <QTimerEvent>
#include <QVariant>

QT_USE_NAMESPACE

class TiledMapReplyTest :public QGeoTiledMapReply
{
    Q_OBJECT
public:
    TiledMapReplyTest(const QGeoTileSpec &spec, QObject *parent=0): QGeoTiledMapReply (spec, parent) {}
    void callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void callSetFinished ( bool finished ) { setFinished(finished);}
    void callSetCached(bool cached) { setFinished(cached);}
    void callSetMapImageData(const QByteArray &data) { setMapImageData(data); }
    void callSetMapImageFormat(const QString &format) { setMapImageFormat(format); }
    void abort() { emit aborted(); }

Q_SIGNALS:
    void aborted();
};

class QGeoTileFetcherTest: public QGeoTileFetcher
{
    Q_OBJECT
public:
    QGeoTileFetcherTest(QObject *parent = 0)
    :    QGeoTileFetcher(parent), finishRequestImmediately_(false), errorCode_(QGeoTiledMapReply::NoError)
    {
    }

    bool init()
    {
        return true;
    }

    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec)
    {
        TiledMapReplyTest* mappingReply =  new TiledMapReplyTest(spec, this);

        QImage im(256, 256, QImage::Format_RGB888);
        im.fill(QColor("lightgray"));
        QRectF rect;
        QString text("X: " + QString::number(spec.x()) + "\nY: " + QString::number(spec.y()) + "\nZ: " + QString::number(spec.zoom()));
        rect.setWidth(250);
        rect.setHeight(250);
        rect.setLeft(3);
        rect.setTop(3);
        QPainter painter;
        QPen pen(QColor("firebrick"));
        painter.begin(&im);
        painter.setPen(pen);
        painter.setFont( QFont("Times", 35, 10, false));
        painter.drawText(rect, text);
        // different border color for vertically and horizontally adjacent frames
        if ((spec.x() + spec.y()) % 2 == 0)
            pen.setColor(QColor("yellow"));
        pen.setWidth(5);
        painter.setPen(pen);
        painter.drawRect(0,0,255,255);
        painter.end();
        QPixmap pm = QPixmap::fromImage(im);
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pm.save(&buffer, "PNG");

        mappingReply->callSetMapImageData(bytes);
        mappingReply->callSetMapImageFormat("png");

        if (finishRequestImmediately_) {
            updateRequest(mappingReply);
            return mappingReply;
        } else {
            if (m_queue.isEmpty())
                timer_.start(500, this);
            m_queue.append(mappingReply);
        }

        return mappingReply;
    }

    void setParams(const QVariantMap &parameters)
    {
        if (parameters.contains(QStringLiteral("finishRequestImmediately")))
            finishRequestImmediately_ = parameters.value(QStringLiteral("finishRequestImmediately")).toBool();
    }

    void setTileSize(QSize tileSize)
    {
        tileSize_ = tileSize;
    }

public Q_SLOTS:
    void requestAborted()
    {
        timer_.stop();
        errorString_.clear();
        errorCode_ = QGeoTiledMapReply::NoError;
    }

protected:
    void updateRequest(TiledMapReplyTest* mappingReply)
    {
        if (errorCode_) {
            mappingReply->callSetError(errorCode_, errorString_);
            emit tileError(mappingReply->tileSpec(), errorString_);
        } else {
            mappingReply->callSetError(QGeoTiledMapReply::NoError, "no error");
            mappingReply->callSetFinished(true);
        }
    }

    void timerEvent(QTimerEvent *event)
    {
        if (event->timerId() != timer_.timerId()) {
            QGeoTileFetcher::timerEvent(event);
            return;
        }
        updateRequest(m_queue.takeFirst());
        if (m_queue.isEmpty())
            timer_.stop();
    }

private:
    bool finishRequestImmediately_;
    QBasicTimer timer_;
    QGeoTiledMapReply::Error errorCode_;
    QString errorString_;
    QSize tileSize_;
    QList<TiledMapReplyTest*> m_queue;
};

#endif
