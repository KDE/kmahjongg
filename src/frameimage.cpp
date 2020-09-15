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

// own
#include "frameimage.h"

// Qt
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

// KDE

// KMahjongg
#include "editor.h"
#include "prefs.h"

FrameImage::FrameImage(QWidget * parent, const QSize & initialImageSize)
    : QWidget(parent)
    , m_rx(-1)
    , m_ry(0)
    , m_rw(0)
    , m_rh(0)
    , m_rs(0)
    , m_rt(0)
    , m_thePixmap(new QPixmap(initialImageSize))
{
}

FrameImage::~FrameImage()
{
    delete m_thePixmap;
}

void FrameImage::resizeEvent(QResizeEvent * ev)
{
    *m_thePixmap = QPixmap(ev->size());
}

void FrameImage::paintEvent(QPaintEvent * pa)
{
    QPainter p(this);

    QPen line;
    line.setWidth(1);
    line.setColor(Qt::red);
    p.setPen(line);
    p.setBackgroundMode(Qt::OpaqueMode);
    p.setBackground(Qt::black);

    int x = pa->rect().left();
    int y = pa->rect().top();
    int h = pa->rect().height();
    int w = pa->rect().width();

    p.drawPixmap(x, y, *m_thePixmap, x, y, w - 2, h - 2);
    if (m_rx >= 0) {
        p.drawRect(m_rx + m_rs, m_ry, m_rw - m_rs, m_rh - m_rs);
        p.drawLine(m_rx, m_ry + m_rs, m_rx, m_ry + m_rh);
        p.drawLine(m_rx, m_ry + m_rs, m_rx + m_rs, m_ry);
        p.drawLine(m_rx, m_ry + m_rh, m_rx + m_rs, m_ry + m_rh - m_rs);
        p.drawLine(m_rx, m_ry + m_rh, m_rx + m_rw - m_rs, m_ry + m_rh);
        p.drawLine(m_rx + m_rw - m_rs, m_ry + m_rh, m_rx + m_rw, m_ry + m_rh - m_rs);

        int midX = m_rx + m_rs + ((m_rw - m_rs) / 2);
        int midY = m_ry + ((m_rh - m_rs) / 2);
        switch (m_rt) {
            case 0: // delete mode cursor
                p.drawLine(m_rx + m_rs, m_ry, m_rx + m_rw, m_ry + m_rh - m_rs);
                p.drawLine(m_rx + m_rw, m_ry, m_rx + m_rs, m_ry + m_rh - m_rs);

                break;
            case 1: // insert cursor
                p.drawLine(midX, m_ry, midX, m_ry + m_rh - m_rs);
                p.drawLine(m_rx + m_rs, midY, m_rx + m_rw, midY);

                break;
            case 2: // move mode cursor
                p.drawLine(midX, m_ry, m_rx + m_rw, midY);
                p.drawLine(m_rx + m_rw, midY, midX, m_ry + m_rh - m_rs);
                p.drawLine(midX, m_ry + m_rh - m_rs, m_rx + m_rs, midY);
                p.drawLine(m_rx + m_rs, midY, midX, m_ry);

                break;
        }
    }
}

void FrameImage::setRect(int x, int y, int w, int h, int s, int t)
{
    m_rx = x;
    m_ry = y;
    m_rw = w;
    m_rh = h;
    m_rt = t;
    m_rs = s;
}

void FrameImage::mousePressEvent(QMouseEvent * m)
{
    Q_EMIT mousePressed(m);
}

void FrameImage::mouseMoveEvent(QMouseEvent * e)
{
    Q_EMIT mouseMoved(e);
}
