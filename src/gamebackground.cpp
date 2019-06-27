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
#include "gamebackground.h"

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


GameBackground::GameBackground(QGraphicsObject * item)
    : QGraphicsObject(item)
    , m_width(100)
    , m_height(100)
{
}

GameBackground::~GameBackground()
{
}

void GameBackground::setSize(qreal width, qreal height)
{
    m_width = width;
    m_height = height;
}

void GameBackground::prepareForGeometryChange()
{
    prepareGeometryChange();
}

void GameBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *, 
    QWidget *)
{
    painter->fillRect(QRectF(0, 0, m_width, m_height), m_background);
}

void GameBackground::setBackground(const QBrush & background)
{
    m_background = background;
}

QRectF GameBackground::boundingRect() const
{
    return QRectF(QPointF(0.0, 0.0), QSizeF(m_width, m_height));
}

QRectF GameBackground::rect() const
{
    return boundingRect();
}
