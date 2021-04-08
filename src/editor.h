/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>
    SPDX-FileCopyrightText: 2006 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef EDITOR_H
#define EDITOR_H

// Qt
#include <QDialog>

// KF
#include <KToolBar>

// LibKMahjongg
#include <KMahjonggTileset>

// KMahjongg
#include "boardlayout.h"
#include "frameimage.h"
#include "kmtypes.h"

class FrameImage;
class QLabel;
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

public Q_SLOTS:
    /**
     * Load the settings from prefs. */
    void setTilesetFromSettings();

protected Q_SLOTS:
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
