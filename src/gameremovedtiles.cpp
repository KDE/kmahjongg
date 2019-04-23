/* Copyright (C) 2019 Christian Krippendorf <Coding@Christian-Krippendorf.de>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

// own
#include "gameremovedtiles.h"

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>


GameRemovedTiles::GameRemovedTiles(QGraphicsObject * object)
    : QGraphicsObject(object)
    , m_width(100)
    , m_height(100)
{
}

GameRemovedTiles::~GameRemovedTiles()
{
}

void GameRemovedTiles::setSize(qreal width, qreal height)
{
    m_width = width;
    m_height = height;
}

void GameRemovedTiles::prepareForGeometryChange()
{
    prepareGeometryChange();
}

void GameRemovedTiles::paint(QPainter *painter, const QStyleOptionGraphicsItem *, 
    QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(0.5);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, m_width, m_height), 10, 10);
    QPen pen(Qt::black, 10);
    painter->setPen(pen);
    painter->fillPath(path, Qt::black);
}

QRectF GameRemovedTiles::boundingRect() const
{
    return QRectF(QPointF(0.0, 0.0), QSizeF(m_width, m_height));
}

QRectF GameRemovedTiles::rect() const
{
    return boundingRect();
}
