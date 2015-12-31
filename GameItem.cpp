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

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>


GameItem::GameItem(bool selected, QGraphicsObject *pItem)
    : QGraphicsObject(pItem),
    m_dying(false),
    m_iShadowWidth(0),
    m_iShadowHeight(0),
    m_pSelPix(new QPixmap()),
    m_pUnselPix(new QPixmap()),
    m_pFacePix(new QPixmap())
{
    // Init POSITION
    m_stPos.y = 0;
    m_stPos.x = 0;
    m_stPos.e = 0;
    m_stPos.f = 0;

    setSelected(selected);
    setOpacity(1.0);
}

GameItem::~GameItem()
{
    delete m_pUnselPix;
    delete m_pSelPix;
    delete m_pFacePix;
}

TileViewAngle GameItem::getAngle() const
{
    return m_angle;
}

void GameItem::setAngle(TileViewAngle angle, QPixmap * pSelPix, QPixmap * pUnselPix, int iShadowWidth, int iShadowHeight)
{
    m_angle = angle;

    // Set the new pictures realted to the new angle.
    *m_pSelPix = *pSelPix;
    *m_pUnselPix = *pUnselPix;

    // Set the new shadow width and height.
    m_iShadowWidth = iShadowWidth;
    m_iShadowHeight = iShadowHeight;

    // Update the face offset.
    updateFaceOffset();
}

bool GameItem::isShadow(QPointF const position) const
{
    // Get the realated point.
    QPointF mappedPosition = mapFromParent(position);

    int iNewPosX = mappedPosition.x() + getShadowDeltaX();
    int iNewPosY = mappedPosition.y() + getShadowDeltaY();

    if ((iNewPosX < 0 || iNewPosX > m_pSelPix->width()) || (iNewPosY < 0 || iNewPosY > m_pSelPix->height())) {
        return true;
    }

    return false;
}

int GameItem::getShadowDeltaX() const
{
    return (m_angle == NE || m_angle == SE) ? 1 * m_iShadowWidth: -1 * m_iShadowWidth;
}

int GameItem::getShadowDeltaY() const
{
    return (m_angle == NW || m_angle == NE) ? 1 * m_iShadowHeight: -1 * m_iShadowHeight;
}

void GameItem::prepareForGeometryChange()
{
    prepareGeometryChange();
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
    }
}

void GameItem::paint(QPainter * pPainter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (isSelected()) {
        pPainter->drawPixmap(pos(), *m_pSelPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_pFacePix);
    } else {
        pPainter->drawPixmap(pos(), *m_pUnselPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_pFacePix);
    }
}

void GameItem::setFace(QPixmap * pFacePix)
{
    *m_pFacePix = *pFacePix;
    updateFaceOffset();
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
    QTimer::singleShot(40, this, &GameItem::fadeOut);
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
    QTimer::singleShot(40, this, &GameItem::fadeIn);
}

QRectF GameItem::boundingRect() const
{
    return QRectF(pos(), m_pSelPix->size());
}

QRectF GameItem::rect() const
{
    return boundingRect();
}

void GameItem::setGridPos(USHORT usX, USHORT usY, USHORT usZ)
{
    m_stPos.e = usZ;
    m_stPos.y = usY;
    m_stPos.x = usX;
}

void GameItem::setGridPos(POSITION & stPos)
{
    m_stPos = stPos;
}

POSITION GameItem::getGridPos() const
{
    return m_stPos;
}

void GameItem::setFaceId(USHORT usFaceId)
{
    m_stPos.f = usFaceId;
}

USHORT GameItem::getFaceId() const
{
    return m_stPos.f;
}

USHORT GameItem::getGridPosX() const
{
    return m_stPos.x;
}

USHORT GameItem::getGridPosY() const
{
    return m_stPos.y;
}

USHORT GameItem::getGridPosZ() const
{
    return m_stPos.e;
}
