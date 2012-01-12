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

#include "FrameImage.h"
#include "prefs.h"

#include <qevent.h>
#include <qimage.h>
#include <qpainter.h>
#include <qtextstream.h>

#include <kcomponentdata.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <KStandardGuiItem>
#include <kimageio.h>
#include <kvbox.h>


FrameImage::FrameImage(QWidget *parent, const QSize &initialImageSize)
  : KGameCanvasWidget(parent)
{
    rx = -1;
    thePixmap = new QPixmap(initialImageSize);
}

FrameImage::~FrameImage()
{
    delete thePixmap;
}

void FrameImage::resizeEvent(QResizeEvent *ev)
{
    *thePixmap = QPixmap(ev->size());
}

void FrameImage::paintEvent(QPaintEvent *pa)
{
    //QFrame::paintEvent(pa);

    QPainter p(this);

    QPen line;
    line.setStyle(Qt::DotLine);
    line.setWidth(2);
    line.setColor(Qt::yellow);
    p.setPen(line);
    p.setBackgroundMode(Qt::OpaqueMode);
    p.setBackground(Qt::black);

    int x = pa->rect().left();
    int y = pa->rect().top();
    int h = pa->rect().height();
    int w  = pa->rect().width();

//     p.drawPixmap(x+frameWidth(),y+frameWidth(),*thePixmap,x+frameWidth(),y+frameWidth(),w-(2*frameWidth()),h-(2*frameWidth()));
    p.drawPixmap(x, y, *thePixmap, x, y, w - 2, h - 2);
    if (rx >=0) {
        p.drawRect(rx + rs, ry, rw - rs, rh - rs);
        p.drawLine(rx, ry + rs, rx, ry + rh);
        p.drawLine(rx, ry + rs, rx + rs, ry);
        p.drawLine(rx, ry + rh, rx + rs, ry + rh - rs);
        p.drawLine(rx, ry + rh, rx + rw - rs, ry + rh);
        p.drawLine(rx + rw - rs, ry + rh, rx + rw, ry + rh - rs);

        int midX = rx + rs + ((rw - rs) / 2);
        int midY = ry + ((rh - rs) / 2);
        switch (rt) {
        case 0:  // delete mode cursor
            p.drawLine(rx + rs, ry, rx + rw, ry + rh - rs);
            p.drawLine(rx + rw, ry, rx + rs, ry + rh - rs);

            break;
        case 1: // insert cursor
            p.drawLine(midX, ry, midX, ry + rh - rs);
            p.drawLine(rx + rs, midY, rx + rw, midY);

            break;
        case 2: // move mode cursor
            p.drawLine(midX, ry, rx + rw, midY);
            p.drawLine(rx + rw, midY, midX, ry + rh - rs);
            p.drawLine(midX, ry + rh - rs, rx + rs, midY);
            p.drawLine(rx + rs, midY, midX, ry);

            break;
        }
    }
}

void FrameImage::setRect(int x, int y, int w, int h, int s, int t)
{
    rx = x;
    ry = y;
    rw = w;
    rh = h;
    rt = t;
    rs = s;
}

void FrameImage::mousePressEvent(QMouseEvent *m)
{
    emit mousePressed(m);
}

void FrameImage::mouseMoveEvent(QMouseEvent *e)
{
    emit mouseMoved(e);
}


#include "FrameImage.moc"
