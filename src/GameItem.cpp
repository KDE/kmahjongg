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


GameItem::GameItem(bool selected, QGraphicsObject *item)
    : QGraphicsObject(item),
    m_dying(false),
    m_shadowWidth(0),
    m_shadowHeight(0),
    m_angle(),
    m_selPix(new QPixmap()),
    m_unselPix(new QPixmap()),
    m_facePix(new QPixmap())
{
    // Init POSITION
    m_stPos.y = 0;
    m_stPos.x = 0;
    m_stPos.z = 0;
    m_stPos.f = 0;

    setSelected(selected);
    setOpacity(1.0);
}

GameItem::~GameItem()
{
    delete m_unselPix;
    delete m_selPix;
    delete m_facePix;
}

TileViewAngle GameItem::getAngle() const
{
    return m_angle;
}

void GameItem::setAngle(TileViewAngle angle, QPixmap * selPix, QPixmap * unselPix, int shadowWidth, int shadowHeight)
{
    m_angle = angle;

    // Set the new pictures realted to the new angle.
    *m_selPix = *selPix;
    *m_unselPix = *unselPix;

    // Set the new shadow width and height.
    m_shadowWidth = shadowWidth;
    m_shadowHeight = shadowHeight;

    // Update the face offset.
    updateFaceOffset();
}

bool GameItem::isShadow(QPointF const position) const
{
    // Get the realated point.
    QPointF mappedPosition = mapFromParent(position);

    int newPosX = mappedPosition.x() + getShadowDeltaX();
    int newPosY = mappedPosition.y() + getShadowDeltaY();

    if ((newPosX < 0 || newPosX > m_selPix->width()) || (newPosY < 0 || newPosY > m_selPix->height())) {
        return true;
    }

    return false;
}

int GameItem::getShadowDeltaX() const
{
    return (m_angle == NE || m_angle == SE) ? 1 * m_shadowWidth: -1 * m_shadowWidth;
}

int GameItem::getShadowDeltaY() const
{
    return (m_angle == NW || m_angle == NE) ? 1 * m_shadowHeight: -1 * m_shadowHeight;
}

void GameItem::prepareForGeometryChange()
{
    prepareGeometryChange();
}

void GameItem::updateFaceOffset()
{
    int horizontalOffset = m_selPix->width() - m_facePix->width();
    int verticalOffset = m_selPix->height() - m_facePix->height();

    switch (m_angle) {
    case NW:
        m_faceOffset = QPointF(horizontalOffset, 0);
        break;
    case NE:
        m_faceOffset = QPointF(0, 0);
        break;
    case SE:
        m_faceOffset = QPointF(0, verticalOffset);
        break;
    case SW:
        m_faceOffset = QPointF(horizontalOffset, verticalOffset);
        break;
    }
}

void GameItem::paint(QPainter * pPainter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (isSelected()) {
        pPainter->drawPixmap(pos(), *m_selPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_facePix);
    } else {
        pPainter->drawPixmap(pos(), *m_unselPix);
        pPainter->drawPixmap(pos() + m_faceOffset, *m_facePix);
    }
}

void GameItem::setFace(QPixmap * facePix)
{
    *m_facePix = *facePix;
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
    return QRectF(pos(), m_selPix->size());
}

QRectF GameItem::rect() const
{
    return boundingRect();
}

void GameItem::setGridPos(USHORT x, USHORT y, USHORT z)
{
    m_stPos.z = z;
    m_stPos.y = y;
    m_stPos.x = x;
}

void GameItem::setGridPos(POSITION & stPos)
{
    m_stPos = stPos;
}

POSITION GameItem::getGridPos() const
{
    return m_stPos;
}

void GameItem::setFaceId(USHORT faceId)
{
    m_stPos.f = faceId;
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
    return m_stPos.z;
}
