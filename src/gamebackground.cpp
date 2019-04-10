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
#include <QPixmap>
#include <QTimer>


GameBackground::GameBackground(QGraphicsObject * item)
    : QGraphicsObject(item)
    , m_background(new QPixmap())
{
}

GameBackground::~GameBackground()
{
    delete m_background;
}

void GameBackground::prepareForGeometryChange()
{
    prepareGeometryChange();
}

void GameBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *, 
    QWidget *)
{
    painter->drawPixmap(pos(), *m_background);
}

void GameBackground::setBackground(QPixmap *background)
{
    *m_background = *background;
}

QRectF GameBackground::boundingRect() const
{
    return QRectF(pos(), m_background->size());
}

QRectF GameBackground::rect() const
{
    return boundingRect();
}
