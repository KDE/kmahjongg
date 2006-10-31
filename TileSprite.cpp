/*
    Copyright 2006 Mauricio Piacentini  <mauricio@tabuleiro.com>
    begin                : Oct 31 2006

    Kmahjongg is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TileSprite.h"
#include <QImage>
#include <QPixmap>

TileSprite::TileSprite( KGameCanvasAbstract* canvas, QPixmap& backunselected, QPixmap& backselected, QPixmap& face )
    : KGameCanvasItem(canvas)
{
    m_backselected = backselected;
    m_backunselected = backunselected;
    m_face = face;
    m_selected = false;
}

TileSprite::~TileSprite()
{
}

void TileSprite::paint(QPainter* p) {
  if (m_selected) {
	  p->drawPixmap(pos(), m_backselected);
  } else {
	  p->drawPixmap(pos(), m_backunselected);
  }
}

QRect TileSprite::rect() const {
    return QRect(pos(), m_backselected.size());
}

