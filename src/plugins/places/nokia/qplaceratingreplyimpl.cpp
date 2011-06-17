/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplaceratingreplyimpl.h"

#include <QHash>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceRatingReplyImpl::QPlaceRatingReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceReply(parent),
    restReply(reply)
{
    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &)),
                this, SLOT(restFinished(const QString &)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
    }
}

/*!
    Destructor.
*/
QPlaceRatingReplyImpl::~QPlaceRatingReplyImpl()
{
}

void QPlaceRatingReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceRatingReplyImpl::restError(QPlaceRestReply::Error errorId)
{
    if (errorId == QPlaceRestReply::Canceled) {
        this->setError(CancelError, "ReauestCanceled");
    } else if (errorId == QPlaceRestReply::NetworkError) {
        this->setError(CommunicationError, "Network error");
    }
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    setFinished(true);
    emit finished();
    emit processingFinished(this);
}

void QPlaceRatingReplyImpl::restFinished(const QString &data)
{
    Q_UNUSED(data);
    setFinished(true);
    emit finished();
    emit processingFinished(this);
    restReply->deleteLater();
    restReply = NULL;
}
