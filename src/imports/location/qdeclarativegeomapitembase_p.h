/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPITEMBASE_H
#define QDECLARATIVEGEOMAPITEMBASE_H

#include <QtQuick/QQuickItem>

#include "qdeclarativegeomap_p.h"
#include "qdeclarativecoordinate_p.h"

QT_BEGIN_NAMESPACE

class QGeoMapViewportChangeEvent
{
public:
    QGeoMapViewportChangeEvent();
    QGeoMapViewportChangeEvent(const QGeoMapViewportChangeEvent &other);

    QGeoCameraData cameraData;
    QSizeF mapSize;

    bool zoomLevelChanged;
    bool centerChanged;
    bool mapSizeChanged;
    bool tiltChanged;
    bool bearingChanged;
    bool rollChanged;
};

class QDeclarativeGeoMapItemBase : public QQuickItem
{
    Q_OBJECT
public:
    QDeclarativeGeoMapItemBase(QQuickItem *parent = 0);
    virtual ~QDeclarativeGeoMapItemBase();

    virtual void setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map);
    virtual void setPositionOnMap(const QGeoCoordinate& coordinate, const QPointF& offset);
    virtual bool contains(QPointF point);
    virtual void dragStarted();
    virtual void dragEnded();

    QDeclarativeGeoMap* quickMap() {return quickMap_;}
    QGeoMap* map() {return map_;}

protected Q_SLOTS:
    virtual void updateMapItem() = 0;
    virtual void afterChildrenChanged();
    virtual void afterViewportChanged(const QGeoMapViewportChangeEvent &event) = 0;

private Q_SLOTS:
    void baseCameraDataChanged(const QGeoCameraData &camera);

private:
    QGeoMap* map_;
    QDeclarativeGeoMap* quickMap_;

    QSizeF lastSize_;
    QGeoCameraData lastCameraData_;
};

QT_END_NAMESPACE

#endif
