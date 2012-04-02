/* Copyright (C) 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA. */

#include "GameItem.h"

#include <kdebug.h>

#include <QPixmap>
#include <QPainter>
#include <QTimer>


GameItem::GameItem(QPixmap *pUnselPix, QPixmap *pSelPix, QPixmap *pFacePix, TileViewAngle angle,
    bool selected, QGraphicsItem *pItem)
    : QObject(0),
    QGraphicsItem(pItem),
    m_pUnselPix(pUnselPix),
    m_pFacePix(pFacePix),
    m_dying(false)
{
    setAngle(angle, pSelPix, pUnselPix);
    setSelected(selected);
    setOpacity(1.0);
}

GameItem::~GameItem()
{
}

TileViewAngle GameItem::getAngle() const
{
    return m_angle;
}

void GameItem::setAngle(TileViewAngle angle, QPixmap *pSelPix, QPixmap *pUnselPix)
{
    m_angle = angle;
    m_pSelPix = pSelPix;
    m_pUnselPix = pUnselPix;

    updateFaceOffset();
}

void GameItem::updateFaceOffset()
{
    int iHorizontalOffset = m_pSelPix->width() - m_pFacePix->width();
    int iVerticalOffset = m_pSelPix->height() - m_pFacePix->height();

    switch (m_angle) {
    case NW:
        m_faceOffset = QPointF(iHorizontalOffset, 0);
        break;
    case NE:
        m_faceOffset = QPointF(0, 0);
        break;
    case SE:
        m_faceOffset = QPointF(0, iVerticalOffset);
        break;
    case SW:
        m_faceOffset = QPointF(iHorizontalOffset, iVerticalOffset);
        break;
    default:
        kDebug() << "Cannot detect the angle!";
    }
}

void GameItem::paint(QPainter *pPainter, const QStyleOptionGraphicsItem * pOption, QWidget *pWidget)
{
    if (isSelected()) {
        pPainter->drawPixmap(pos(), *m_pSelPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_pFacePix);
    } else {
        pPainter->drawPixmap(pos(), *m_pUnselPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_pFacePix);
    }
}

void GameItem::fadeOut()
{
    m_dying = true;
    setOpacity(opacity() - 0.05);

    if (opacity() <= 0) {
        // cancel fade and schedule our destruction
        deleteLater();

        return;
    }

    // keep fading
    QTimer::singleShot(40, this, SLOT(fadeOut()));
}

void GameItem::fadeIn()
{
    if (m_dying) {
        return;
    }

    setOpacity(opacity() + 0.05);

    if (opacity() == 1.00 || m_dying) {
        //cancel fade in
        return;
    }

    //keep fading
    QTimer::singleShot(40, this, SLOT(fadeIn()));
}
