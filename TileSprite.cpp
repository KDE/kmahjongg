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
#include <QPixmap>
#include <QTimer>

TileSprite::TileSprite( KGameCanvasAbstract* canvas, QPixmap & backunselected, QPixmap & backselected, QPixmap & face, TileViewAngle angle, bool selected )
    : QObject(), KGameCanvasItem(canvas)
{
    m_dying = false;
    setOpacity(255);
    m_backselected = backselected;
    m_backunselected = backunselected;
    m_face = face;
    m_selected = selected;
    m_angle = angle;
    m_woffset = m_backselected.width() - m_face.width();
    m_hoffset = m_backselected.height() - m_face.height();
    updateOffset();
}

TileSprite::~TileSprite()
{
}

void TileSprite::setAngle(TileViewAngle angle, QPixmap & backunselected, QPixmap & backselected) {
    m_angle = angle;
    m_backselected = backselected;
    m_backunselected = backunselected;
    updateOffset();
    //changed(); We need to call updateSpriteMap to relayer anyway
}

void TileSprite::updateOffset() {
    switch( m_angle )
        {
            case NW:
		m_faceoffset = QPoint(m_woffset,0);
		break;
	    case NE:
		m_faceoffset = QPoint(0,0);
		break;
	    case SE:
		m_faceoffset = QPoint(0,m_hoffset);
		break;
	    case SW:
		m_faceoffset = QPoint(m_woffset,m_hoffset);
		break;
	}
}

void TileSprite::paintInternal(QPainter* p, const QRect& /*prect*/,
                  const QRegion& /*preg*/, const QPoint& /*delta*/, double cumulative_opacity) {
  int op = int(cumulative_opacity*opacity() + 0.5);

  if(op <= 0)
    return;

  if(op < 255)
    p->setOpacity( op/255.0 );
    paint(p);
  if(op < 255)
    p->setOpacity(1.0);
}

void TileSprite::paint(QPainter* p) {
  if (m_selected) {
	  p->drawPixmap(pos(), m_backselected);
	  p->drawPixmap(pos()+m_faceoffset, m_face);
  } else {
	  p->drawPixmap(pos(), m_backunselected);
	  p->drawPixmap(pos()+m_faceoffset, m_face);
  }
}

void TileSprite::fadeOut() {
    m_dying = true;
    setOpacity(opacity()-25);
    if (opacity() <= 0) {
	//cancel fade and schedule our destruction!
	deleteLater();
	return;
    }
    //keep fading
    QTimer::singleShot(40, this, &TileSprite::fadeOut);
}

void TileSprite::fadeIn() {
    if (m_dying) return;
    setOpacity(opacity()+25);
    if ((opacity() == 255)||(m_dying)) {
	//cancel fade in
	return;
    }
    //keep fading
    QTimer::singleShot(40, this, &TileSprite::fadeIn);
}

QRect TileSprite::rect() const {
    return QRect(pos(), m_backselected.size());
}


