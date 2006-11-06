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

#ifndef TILESPRITE_H
#define TILESPRITE_H

#include "KmTypes.h"
#include <kgamecanvas.h>

#include <QPixmap>

//This class wraps tile drawing, so it can blit the selected or unselected background, plus
//the tileface, positioned in the correct orientation. 

class TileSprite : public KGameCanvasItem
{
public:
    TileSprite ( KGameCanvasAbstract* canvas, QPixmap backunselected, QPixmap backselected, QPixmap face, TileViewAngle angle, bool selected);
    ~TileSprite();

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    void setAngle(TileViewAngle angle, QPixmap backunselected, QPixmap backselected);

    inline void setScale (double scale){ m_scale=scale;};
    inline double scale(){ return m_scale;};
    inline void setSelected (bool enabled){ m_selected = enabled; changed();};
    inline double selected(){ return m_selected;};

private:
    void updateOffset();

    QPixmap m_backselected;
    QPixmap m_backunselected;
    QPixmap m_face;
    double    m_scale;
    bool m_selected;
    short m_woffset;
    short m_hoffset;
    TileViewAngle m_angle;
    QPoint m_faceoffset;
};

#endif // TILESPRITE_H
