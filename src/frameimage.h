/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>
    SPDX-FileCopyrightText: 2006 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FRAMEIMAGE_H
#define FRAMEIMAGE_H

// Qt
#include <QWidget>

// LibKMahjongg
#include <KMahjonggTileset>
#include <KMahjonggBackground>

// KMahjongg
#include "boardlayout.h"

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

Q_SIGNALS:
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
