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

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgamecanvas.h>

#include <QPixmap>
#include <QObject>

//This class wraps tile drawing, so it can blit the selected or unselected background, plus
//the tileface, positioned in the correct orientation. 

/**
* This class wraps tile drawing, so it can blit the selected or 
* unselected background, plus the tileface, positioned in the correct 
* orientation.
* 
* @author Mauricio Piacentini  <mauricio@tabuleiro.com>
*/
class TileSprite : public QObject, public KGameCanvasItem
{
    Q_OBJECT
public:

    /**
     * Constructor
     *
     * @param canvas
     * @param backunselected
     * @param backselected
     * @param face
     * @param angle
     * @param selected
     */
    TileSprite ( KGameCanvasAbstract* canvas, QPixmap& backunselected, QPixmap& backselected, QPixmap& face, TileViewAngle angle, bool selected);

    /**
     * Deafault destructor */
    ~TileSprite();
    /**
     * Method Description 
     * 
     * @param p
     * @param prect   
     * @param preg
     * @param delta
     * @param cumulative_opacity
     */
    virtual void paintInternal(QPainter* p, const QRect& prect, const QRegion& preg,
                                          const QPoint& delta, double cumulative_opacity);
    /**
     * Method Description @param p */
    virtual void paint(QPainter* p);
    /**
     * Method Description */
    virtual QRect rect() const;
    /**
     * Method Description
     * @param angle
     * @param backunselected
     * @param backselected
     */
    void setAngle(TileViewAngle angle, QPixmap& backunselected, QPixmap& backselected);

    /**
     * Method Description @param scale */
    inline void setScale (double scale){ m_scale=scale;}
    /**
     * Method Description @return double m_scale */
    inline double scale(){ return m_scale;}
    /**
     * Method Description  @param enabled  */
    inline void setSelected (bool enabled){ m_selected = enabled; changed();}
    /**
     * Method Description @return double m_selected */
    inline double selected(){ return m_selected;}
public slots:
    /**
     * Slot Description */
     void fadeOut();
    /**
     * Slot Description */
     void fadeIn();

private:
    void updateOffset();

    QPixmap m_backselected;
    QPixmap m_backunselected;
    QPixmap m_face;
    double    m_scale;
    bool m_selected;
    bool m_dying;
    short m_woffset;
    short m_hoffset;
    TileViewAngle m_angle;
    QPoint m_faceoffset;
};

#endif // TILESPRITE_H
