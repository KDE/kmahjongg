/* Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
 * Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

#ifndef FRAMEIMAGE_H
#define FRAMEIMAGE_H

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgamecanvas.h>

#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "BoardLayout.h"

#include <qframe.h>


class QComboBox;
class QPixmap;

/**
 * @short This class implements
 *
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com> */
class FrameImage: public KGameCanvasWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param parent
     * @param initialImageSize */
    FrameImage(QWidget *parent, const QSize &initialImageSize);

    /**
     * Default Destructor */
    ~FrameImage();

    /**
     * Method Description
     *
     * @return QPixmap */
    QPixmap* getPreviewPixmap() {return thePixmap;}

    /**
     * Method Description
     *
     * @param x
     * @param y
     * @param w
     * @param h
     * @param ss
     * @param type */
    void setRect(int x, int y, int w, int h, int ss, int type);

signals:
    /**
     * Signal Description */
    void mousePressed(QMouseEvent *e);

    /**
     * Signal Description */
    void mouseMoved(QMouseEvent *e);

protected:
    /**
     * Event Description */
    void mousePressEvent(QMouseEvent *e);

    /**
     * Event Description */
    void mouseMoveEvent(QMouseEvent *e);

    /**
     * Event Description */
    void resizeEvent(QResizeEvent *e);

    /**
     * Event Description */
    void paintEvent( QPaintEvent* pa );

private:
    int rx;
    int ry;
    int rw;
    int rh;
    int rs;
    int rt;

    QPixmap *thePixmap;
};


#endif
