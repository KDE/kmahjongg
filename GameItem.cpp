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

#include <QPixmap>


GameItem::GameItem(QPixmap *pUnselPix, QPixmap *pSelPix, QPixmap *pFacePix, TileViewAngle angle,
    bool selected, QGraphicsItem *pItem)
    : QObject(0),
    QGraphicsItem(pItem),
    m_pUnselPix(pUnselPix),
    m_pSelPix(pSelPix),
    m_pFacePix(pFacePix)
{
    setSelected(selected);
}

GameItem::~GameItem()
{
}
