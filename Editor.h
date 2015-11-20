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

#ifndef EDITOR_H
#define EDITOR_H

#include "BoardLayout.h"
#include "FrameImage.h"
#include "kmahjonggtileset.h"
#include "KmTypes.h"

#include <KToolBar>

#include <QDialog>
#include <QFrame>


class FrameImage;
class QLabel;
class KToolBar;
class KActionCollection;

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Editor: public QDialog
{
    Q_OBJECT

public:
    explicit Editor (QWidget *parent = 0);
    virtual ~Editor();

public slots:
    /**
     * Load the settings from prefs. */
    void setTilesetFromSettings();

protected slots:
    void drawFrameMousePressEvent ( QMouseEvent* );
    void drawFrameMouseMovedEvent ( QMouseEvent *);
    void loadBoard();
    bool saveBoard();
    void newBoard();
    void slotShiftLeft();
    void slotShiftRight();
    void slotShiftUp();
    void slotShiftDown();
    void slotModeChanged(QAction*);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *pa);
    void setupToolbar();
    void drawBackground(QPixmap *to) const;

    /**
     * @param to destination QPixmap to draw to */
    void drawTiles(QPixmap *to);

    bool testSave();
    void transformPointToPosition(const QPoint&, POSITION&, bool align) const;
    void drawCursor(POSITION &p, bool visible);
    bool canInsert(POSITION &p) const;
    void statusChanged() const;
    QString statusText() const;

    /**
     * Override the closeEvent(...) method of qdialog. */
    void closeEvent(QCloseEvent *e);

    /**
     * Update the tile size. */
    void updateTileSize(const QSize size);

private:
    enum class EditMode {
        remove = 98,
        insert = 99,
        move = 100
    };
    EditMode mode;

    QString mTileset;

    int borderLeft;
    int borderTop;
    int numTiles;
    bool clean;

    FrameImage *drawFrame;
    KMahjonggTileset tiles;
    BoardLayout theBoard;
    POSITION currPos;

    QLabel *theLabel;

    KToolBar *topToolbar;
    KActionCollection *actionCollection;
};

#endif
