/*
    SPDX-FileCopyrightText: 2019 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

#include "moc_gamebackground.cpp"
