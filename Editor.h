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


#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "BoardLayout.h"
#include "FrameImage.h"

#include <qframe.h>

#include <kdialog.h>
#include <ktoolbar.h>
#include <kfiledialog.h>


class QLabel;
class KActionCollection;
class QLabel;

/**
 * @short This class implements
 * 
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Editor: public KDialog
{
    Q_OBJECT

public:
    /**
     * Constructor Description
     *
     * @param parent */
    explicit Editor (QWidget *parent = 0);

    /**
     * Default Destructor */
    virtual ~Editor();

    /**
     * Return the tileset that is actually set. */
    const QString getTileset() const;

    /**
     * Set a new tileset. */
    void setTileset(const QString tileset);

public slots:
    /**
     * Load the settings from prefs. */
    void setTilesetFromSettings();

protected slots:
    /**
     * Slot Description */
    void drawFrameMousePressEvent ( QMouseEvent* );

    /**
     * Slot Description */
    void drawFrameMouseMovedEvent ( QMouseEvent *);

    /**
     * Slot Description */
    void loadBoard();

    /**
     * Slot Description 
     *
     * @return @c true if ...
     * @return @c false if ... */
    bool saveBoard();

    /**
     * Slot Description */
    void newBoard();

    /**
     * Slot Description */
    void slotShiftLeft();

    /**
     * Slot Description */
    void slotShiftRight();

    /**
     * Slot Description */
    void slotShiftUp();

    /**
    * Slot Description */
    void slotShiftDown();

    /**
     * Slot Description */
    void slotModeChanged(QAction*);

    /**
     * Slot Description */

protected:
    /**
     * Method Description */
    void resizeEvent(QResizeEvent *event);

    /**
     * Method Description */
    void paintEvent(QPaintEvent *pa);

    /**
     * Method Description */
    void setupToolbar();

    /**
     * Method Description */
    void drawBackground(QPixmap *to);

    /**
     * Method Description
     *
     * @param to destination QPixmap to draw to */
    void drawTiles(QPixmap *to);

    /**
     * Method Description
     * 
     * @return @c true if
     * @return @c false if */
    bool testSave();

    /**
     * Method Description */
    void transformPointToPosition(const QPoint&, POSITION&, bool align);

    /**
     * Method Description 
     *
     * @param p @ref pos
     * @param visible */
    void drawCursor(POSITION &p, bool visible);

    /**
     * Method Description
     * 
     * @param p @ref pos
     * @return @c true if
     * @return @c false if */
    bool canInsert(POSITION &p);

    /**
     * Method Description */
    void statusChanged();

    /**
     * Method Description
     *
     * @return status description */
    QString statusText();

    /**
     * Override the closeEvent(...) method of kdialog..qdialog. */
    void closeEvent(QCloseEvent *e);

    /**
     * Update the tile size. */
    void updateTileSize(const QSize size);

private:
    /**
     * @short Describe enum */
    enum {
        remove = 98,
        insert = 99,
        move = 100
    };

    QString mTileset;

    int borderLeft;
    int borderTop;
    int mode;
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
