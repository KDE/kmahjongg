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

// Qt
#include <QFrame>
#include <QWidget>

// KMahjongg
#include "boardlayout.h"
#include "kmahjonggbackground.h"
#include "kmahjonggtileset.h"

class QPixmap;
class QPixmap;

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com> */
class FrameImage : public QWidget
{
    Q_OBJECT

public:
    explicit FrameImage(QWidget * parent, const QSize & initialImageSize);
    ~FrameImage() override;

    QPixmap * getPreviewPixmap()
    {
        return m_thePixmap;
    }
    void setRect(int x, int y, int w, int h, int ss, int type);

signals:
    void mousePressed(QMouseEvent * e);
    void mouseMoved(QMouseEvent * e);

protected:
    void mousePressEvent(QMouseEvent * e) override;
    void mouseMoveEvent(QMouseEvent * e) override;
    void resizeEvent(QResizeEvent * e) override;
    void paintEvent(QPaintEvent * pa) override;

private:
    int m_rx;
    int m_ry;
    int m_rw;
    int m_rh;
    int m_rs;
    int m_rt;

    QPixmap * m_thePixmap;
};

#endif
