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

// Qt
#include <QDialog>

// KDE
#include <KToolBar>

// KMahjongg
#include "boardlayout.h"
#include "frameimage.h"
#include "kmahjonggtileset.h"
#include "kmtypes.h"

class FrameImage;
class QLabel;
class KToolBar;
class KActionCollection;

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget * parent = nullptr);
    ~Editor() override;

public slots:
    /**
     * Load the settings from prefs. */
    void setTilesetFromSettings();

protected slots:
    void drawFrameMousePressEvent(QMouseEvent *);
    void drawFrameMouseMovedEvent(QMouseEvent *);
    void loadBoard();
    bool saveBoard();
    void newBoard();
    void slotShiftLeft();
    void slotShiftRight();
    void slotShiftUp();
    void slotShiftDown();
    void slotModeChanged(QAction *);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent * pa) override;
    void setupToolbar();
    void drawBackground(QPixmap * to) const;

    /**
     * @param to destination QPixmap to draw to */
    void drawTiles(QPixmap * to);

    bool testSave();
    void transformPointToPosition(const QPoint &, POSITION &, bool align) const;
    void drawCursor(POSITION & p, bool visible);
    bool canInsert(POSITION & p) const;
    void statusChanged() const;
    QString statusText() const;

    /**
     * Override the closeEvent(...) method of qdialog. */
    void closeEvent(QCloseEvent * e) override;

    /**
     * Update the tile size. */
    void updateTileSize(const QSize size);

private:
    enum class EditMode {
        remove = 98,
        insert = 99,
        move = 100
    };
    EditMode m_mode;

    QString m_tileset;

    int m_borderLeft;
    int m_borderTop;
    int m_numTiles;
    bool m_clean;

    FrameImage * m_drawFrame;
    KMahjonggTileset m_tiles;
    BoardLayout m_theBoard;
    POSITION m_curPos;

    QLabel * m_theLabel;

    KToolBar * m_topToolbar;
    KActionCollection * m_actionCollection;
};

#endif
