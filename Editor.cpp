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

#include "Editor.h"
#include "FrameImage.h"
#include "prefs.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
#include <KToggleAction>

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QResizeEvent>
#include <QPainter>


Editor::Editor(QWidget *parent)
    : QDialog(parent),
    m_mode(EditMode::insert),
    m_borderLeft(0),
    m_borderTop(0),
    m_numTiles(0),
    m_clean(true),
    m_drawFrame(nullptr),
    m_tiles(),
    m_theLabel(nullptr),
    m_topToolbar(nullptr),
    m_actionCollection(nullptr)
{
    setModal(true);

    QWidget *mainWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainWidget);

    resize(QSize(800, 400));

    QGridLayout *gridLayout = new QGridLayout(mainWidget);
    QVBoxLayout *layout = new QVBoxLayout();

    setupToolbar();
    layout->addWidget(m_topToolbar);

    m_drawFrame = new FrameImage(this, QSize(0, 0));
    m_drawFrame->setFocusPolicy(Qt::NoFocus);
    m_drawFrame->setMouseTracking(true);

    layout->addWidget(m_drawFrame);
    gridLayout->addLayout(layout, 0, 0, 1, 1);

    //toolbar will set our minimum height
    setMinimumHeight(120);

    // tell the user what we do
    setWindowTitle(i18n("Edit Board Layout"));

    connect(m_drawFrame, &FrameImage::mousePressed, this, &Editor::drawFrameMousePressEvent);
    connect(m_drawFrame, &FrameImage::mouseMoved, this, &Editor::drawFrameMouseMovedEvent);

    statusChanged();

    update();
}

Editor::~Editor()
{
}

void Editor::updateTileSize(const QSize size)
{
    const int width = m_theBoard.getWidth();
    const int height = m_theBoard.getHeight();
    const QSize tileSize = m_tiles.preferredTileSize(size, width / 2, height / 2);

    m_tiles.reloadTileset(tileSize);
    m_borderLeft = (m_drawFrame->size().width() - (width * m_tiles.qWidth())) / 2;
    m_borderTop = (m_drawFrame->size().height() - (height * m_tiles.qHeight())) / 2;
}

void Editor::resizeEvent(QResizeEvent *event)
{
    updateTileSize(event->size());
}

void Editor::setupToolbar()
{
    m_topToolbar = new KToolBar(this, "editToolBar");
    m_topToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    m_actionCollection = new KActionCollection(this);

    // new game
    QAction *newBoard = m_actionCollection->addAction(QStringLiteral("new_board"));
    newBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    newBoard->setText(i18n("New board"));
    connect(newBoard, &QAction::triggered, this, &Editor::newBoard);
    m_topToolbar->addAction(newBoard);

    // open game
    QAction *openBoard = m_actionCollection->addAction(QStringLiteral("open_board"));
    openBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    openBoard->setText(i18n("Open board"));
    connect(openBoard, &QAction::triggered, this, &Editor::loadBoard);
    m_topToolbar->addAction(openBoard);

    // save game
    QAction *saveBoard = m_actionCollection->addAction(QStringLiteral("save_board"));
    saveBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));
    saveBoard->setText(i18n("Save board"));
    connect(saveBoard, &QAction::triggered, this, &Editor::saveBoard);
    m_topToolbar->addAction(saveBoard);

    m_topToolbar->addSeparator();

#ifdef FUTURE_OPTIONS
    // Select
    QAction *select = actionCollection->addAction(QLatin1String("select"));
    select->setIcon(QIcon::fromTheme(QLatin1String("rectangle_select")));
    select->setText(i18n("Select"));
    topToolbar->addAction(select);

    QAction *cut = actionCollection->addAction(QLatin1String("edit_cut"));
    cut->setIcon(QIcon::fromTheme(QLatin1String("edit-cut")));
    cut->setText(i18n("Cut"));
    topToolbar->addAction(cut);

    QAction *copy = actionCollection->addAction(QLatin1String("edit_copy"));
    copy->setIcon(QIcon::fromTheme(QLatin1String("edit-copy")));
    copy->setText(i18n("Copy"));
    topToolbar->addAction(copy);

    QAction *paste = actionCollection->addAction(QLatin1String("edit_paste"));
    paste->setIcon(QIcon::fromTheme(QLatin1String("edit-paste")));
    paste->setText(i18n("Paste"));
    topToolbar->addAction(paste);

    topToolbar->addSeparator();

    QAction *moveTiles = actionCollection->addAction(QLatin1String("move_tiles"));
    moveTiles->setIcon(QIcon::fromTheme(QLatin1String("move")));
    moveTiles->setText(i18n("Move tiles"));
    topToolbar->addAction(moveTiles);
#endif

    KToggleAction *addTiles = new KToggleAction(QIcon::fromTheme(QStringLiteral("draw-freehand")), i18n("Add tiles"), this);
    m_actionCollection->addAction(QStringLiteral("add_tiles"), addTiles);
    m_topToolbar->addAction(addTiles);
    KToggleAction *delTiles = new KToggleAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove tiles"), this);
    m_actionCollection->addAction(QStringLiteral("del_tiles"), delTiles);
    m_topToolbar->addAction(delTiles);

    QActionGroup *radioGrp = new QActionGroup(this);
    radioGrp->setExclusive(true);
    radioGrp->addAction(addTiles);
    addTiles->setChecked(true);

#ifdef FUTURE_OPTIONS
    radioGrp->addAction(moveTiles);
#endif

    radioGrp->addAction(delTiles);
    connect(radioGrp, &QActionGroup::triggered, this, &Editor::slotModeChanged);

    // board shift

    m_topToolbar->addSeparator();

    // NOTE: maybe join shiftActions in QActionGroup and create one slot(QAction*) instead of 4 slots? ;)
    // Does this makes sense? dimsuz
    QAction *shiftLeft = m_actionCollection->addAction(QStringLiteral("shift_left"));
    shiftLeft->setIcon(QIcon::fromTheme(QStringLiteral("go-previous")));
    shiftLeft->setText(i18n("Shift left"));
    connect(shiftLeft, &QAction::triggered, this, &Editor::slotShiftLeft);
    m_topToolbar->addAction(shiftLeft);

    QAction *shiftUp = m_actionCollection->addAction(QStringLiteral("shift_up"));
    shiftUp->setIcon(QIcon::fromTheme(QStringLiteral("go-up")));
    shiftUp->setText(i18n("Shift up"));
    connect(shiftUp, &QAction::triggered, this, &Editor::slotShiftUp);
    m_topToolbar->addAction(shiftUp);

    QAction *shiftDown = m_actionCollection->addAction(QStringLiteral("shift_down"));
    shiftDown->setIcon(QIcon::fromTheme(QStringLiteral("go-down")));
    shiftDown->setText(i18n("Shift down"));
    connect(shiftDown, &QAction::triggered, this, &Editor::slotShiftDown);
    m_topToolbar->addAction(shiftDown);

    QAction *shiftRight = m_actionCollection->addAction(QStringLiteral("shift_right"));
    shiftRight->setIcon(QIcon::fromTheme(QStringLiteral("go-next")));
    shiftRight->setText(i18n("Shift right"));
    connect(shiftRight, &QAction::triggered, this, &Editor::slotShiftRight);
    m_topToolbar->addAction(shiftRight);

    m_topToolbar->addSeparator();
    QAction *quit = m_actionCollection->addAction(KStandardAction::Quit, QStringLiteral("quit"), this, SLOT(close()));
    m_topToolbar->addAction(quit);

    // status in the toolbar for now (ick)
    QWidget *hbox = new QWidget(m_topToolbar);
    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();

    m_theLabel = new QLabel(statusText(), hbox);
    layout->addWidget(m_theLabel);
    m_topToolbar->addWidget(hbox);

    m_topToolbar->adjustSize();
    setMinimumWidth(m_topToolbar->width());
}

void Editor::statusChanged() const
{
    const bool canSave = ((m_numTiles != 0) && ((m_numTiles & 1) == 0));
    m_theLabel->setText(statusText());
    m_actionCollection->action(QStringLiteral("save_board"))->setEnabled(canSave);
}

void Editor::slotShiftLeft()
{
    m_theBoard.shiftLeft();
    update();
}

void Editor::slotShiftRight()
{
    m_theBoard.shiftRight();
    update();
}

void Editor::slotShiftUp()
{
    m_theBoard.shiftUp();
    update();
}

void Editor::slotShiftDown()
{
    m_theBoard.shiftDown();
    update();
}

void Editor::slotModeChanged(QAction *act)
{
    if (act == m_actionCollection->action(QStringLiteral("move_tiles"))) {
        m_mode = EditMode::move;
    } else if (act == m_actionCollection->action(QStringLiteral("del_tiles"))) {
        m_mode = EditMode::remove;
    } else if (act == m_actionCollection->action(QStringLiteral("add_tiles"))) {
        m_mode = EditMode::insert;
    }
}

QString Editor::statusText() const
{
    int x = m_curPos.x;
    int y = m_curPos.y;
    int z = m_curPos.z;

    if (z == 100) {
        z = 0;
    } else {
        z = z + 1;
    }

    if (x >= m_theBoard.getWidth() || x < 0 || y >= m_theBoard.getHeight() || y < 0) {
        x = y = z = 0;
    }

    return i18n("Tiles: %1 Pos: %2,%3,%4", m_numTiles, x, y, z);
}

void Editor::loadBoard()
{
    if (!testSave()) {
        return;
    }

    const QString filename = QFileDialog::getOpenFileName(this, i18n("Open Board Layout"), QString(),
                                           i18n("Board Layout (*.layout);;All Files (*)"));

    if (filename.isEmpty()) {
        return;
    }

    m_theBoard.loadBoardLayout(filename);
    update();
}

void Editor::newBoard()
{
    // Clear out the contents of the board. Repaint the screen
    // set values to their defaults.

    if (!testSave()) {
        return;
    }

    m_theBoard.clearBoardLayout();

    m_clean = true;
    m_numTiles = 0;

    statusChanged();
    update();
}

bool Editor::saveBoard()
{
    if (!((m_numTiles != 0) && ((m_numTiles & 1) == 0))) {
        KMessageBox::sorry(this, i18n("You can only save with a even number of tiles."));

        return false;
    }

    // get a save file name
    const QString filename = QFileDialog::getSaveFileName(this, i18n("Save Board Layout"), QString(),
                                           i18n("Board Layout (*.layout);;All Files (*)"));

    if (filename.isEmpty()) {
        return false;
    }

    const QFileInfo f(filename);
    if (f.exists()) {
        // if it already exists, querie the user for replacement
        int res = KMessageBox::warningContinueCancel(this,
                i18n("A file with that name already exists. Do you wish to overwrite it?"),
                i18n("Save Board Layout"), KStandardGuiItem::save());

        if (res != KMessageBox::Continue) {
            return false;
        }
    }

    bool result = m_theBoard.saveBoardLayout(filename);

    if (result == true) {
        m_clean = true;

        return true;
    } else {
        return false;
    }
}

bool Editor::testSave()
{
    // test if a save is required and return true if the app is to continue
    // false if cancel is selected. (if ok then call out to save the board

    if (m_clean) {
        return true;
    }

    const int res = KMessageBox::warningYesNoCancel(this,
            i18n("The board has been modified. Would you like to save the changes?"),
            QString(), KStandardGuiItem::save(),KStandardGuiItem::dontSave());

    if (res == KMessageBox::Yes) {
        // yes to save
        if (saveBoard()) {
            return true;
        } else {
            KMessageBox::sorry(this, i18n("Save failed. Aborting operation."));

            return false;
        }
    } else {
        return (res != KMessageBox::Cancel);
    }
    return true;
}

void Editor::paintEvent(QPaintEvent*)
{
    // The main paint event, draw in the grid and blit in
    // the tiles as specified by the layout.

    // first we layer on a background grid
    QPixmap buff;
    QPixmap *dest = m_drawFrame->getPreviewPixmap();
    buff = QPixmap(dest->width(), dest->height());
    drawBackground(&buff);
    drawTiles(&buff);
    QPainter p(dest);
    p.drawPixmap(0, 0, buff);
    p.end();

    m_drawFrame->update();
}

void Editor::drawBackground(QPixmap *pixmap) const
{
    const int width = m_theBoard.getWidth();
    const int height = m_theBoard.getHeight();
    QPainter p(pixmap);

    // blast in a white background
    p.fillRect(0, 0, pixmap->width(), pixmap->height(), Qt::white);

    // now put in a grid of tile quater width squares
    for (int y = 0; y <= height; ++y) {
        int nextY = m_borderTop + (y * m_tiles.qHeight());
        p.drawLine(m_borderLeft, nextY, m_borderLeft + (width * m_tiles.qWidth()), nextY);
    }

    for (int x = 0; x <= width; ++x) {
        int nextX = m_borderLeft + (x * m_tiles.qWidth());
        p.drawLine(nextX, m_borderTop, nextX, m_borderTop + (height * m_tiles.qHeight()));
    }
}

void Editor::drawTiles(QPixmap *dest)
{
    QPainter p(dest);

    const int width = m_theBoard.getWidth();
    const int height = m_theBoard.getHeight();
    const int depth = m_theBoard.getDepth();
    const int shadowX = m_tiles.width() - m_tiles.qWidth() * 2 - m_tiles.levelOffsetX();
    const int shadowY = m_tiles.height() - m_tiles.qHeight() * 2 - m_tiles.levelOffsetY();
    short tile = 0;

    int xOffset = -shadowX;
    int yOffset = -m_tiles.levelOffsetY();

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z = 0; z < depth; ++z) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < height; ++y) {
            // drawing right to left to prevent border overwrite
            for (int x = width - 1; x >= 0; --x) {
                int sx = x * m_tiles.qWidth() + xOffset + m_borderLeft;
                int sy = y * m_tiles.qHeight() + yOffset + m_borderTop;

                if (m_theBoard.getBoardData(z, y, x) != '1') {
                    continue;
                }

                QPixmap t;
                tile = (z * depth) + (y * height) + (x * width);
                t = m_tiles.unselectedTile(0);

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border
                if ((x > 1) && (y > 0) && m_theBoard.getBoardData(z, y - 1, x - 2) == '1') {
                    p.drawPixmap(sx, sy, t, 0, 0, t.width(), t.height());

                    p.drawPixmap(sx - m_tiles.qWidth() + shadowX + m_tiles.levelOffsetX(), sy, t,
                            t.width() - m_tiles.qWidth(),
                            t.height() - m_tiles.qHeight() - m_tiles.levelOffsetX() - shadowY,
                            m_tiles.qWidth(), m_tiles.qHeight() + m_tiles.levelOffsetX());
                } else {
                    p.drawPixmap(sx, sy, t, 0, 0, t.width(), t.height());
                }

                ++tile;
                tile = tile % 143;
            }
        }

        xOffset += m_tiles.levelOffsetX();
        yOffset -= m_tiles.levelOffsetY();
    }
}

void Editor::transformPointToPosition(const QPoint &point, POSITION &mouseClickPos, bool align) const
{
    // convert mouse position on screen to a tile z y x coord
    // different to the one in kmahjongg.cpp since if we hit ground
    // we return a result too.

    short z = 0;
    short y = 0;
    short x = 0;
    mouseClickPos.z = 100;

    // iterate over z coordinate from top to bottom
    for (z = m_theBoard.getDepth() - 1; z >= 0; --z) {
        // calculate mouse coordiantes --> position in game board
        // the factor -theTiles.width()/2 must keep track with the
        // offset for blitting in the print zvent (FIX ME)
        x = ((point.x() - m_borderLeft) - (z + 1) * m_tiles.levelOffsetX()) / m_tiles.qWidth();
        y = ((point.y() - m_borderTop) + z * m_tiles.levelOffsetX()) / m_tiles.qHeight();

        // skip when position is illegal
        if (x < 0 || x >= m_theBoard.getWidth() || y < 0 || y >= m_theBoard.getHeight()) {
            continue;
        }

        switch (m_theBoard.getBoardData(z, y, x)) {
        case static_cast<UCHAR>('3'):
            if (align) {
                --x;
                --y;
            }

            break;

        case static_cast<UCHAR>('2'):
            if (align) {
                --x;
            }

            break;

        case static_cast<UCHAR>('4'):
            if (align) {
                --y;
            }

            break;

        case static_cast<UCHAR>('1'):
            break;

        default:
            continue;
        }

        // if gameboard is empty, skip
        if (!m_theBoard.getBoardData(z, y, x)) {
            continue;
        }

        // here, position is legal
        mouseClickPos.z = z;
        mouseClickPos.y = y;
        mouseClickPos.x = x;
        mouseClickPos.f = m_theBoard.getBoardData(z, y, x);

        break;
    }

    if (mouseClickPos.z == 100) {
        mouseClickPos.x = x;
        mouseClickPos.y = y;
        mouseClickPos.f=0;
    }
}

void Editor::drawFrameMousePressEvent(QMouseEvent* e)
{
    // we swallow the draw frames mouse clicks and process here

    POSITION mousePos;
    transformPointToPosition(e->pos(), mousePos, (m_mode == EditMode::remove));

    switch (m_mode) {
    case EditMode::remove:
        if (!m_theBoard.tileAbove(mousePos) && mousePos.z < m_theBoard.getDepth() && m_theBoard.isTileAt(mousePos)) {
            m_theBoard.deleteTile(mousePos);
            --m_numTiles;
            statusChanged();
            drawFrameMouseMovedEvent(e);
            update();
        }

        break;
    case EditMode::insert: {
        POSITION n = mousePos;

        if (n.z == 100) {
            n.z = 0;
        } else {
            n.z += 1;
        }

        if (canInsert(n)) {
            m_theBoard.insertTile(n);
            m_clean = false;
            ++m_numTiles;
            statusChanged();
            update();
        }

        break;
    }
    default:
        break;
    }
}

void Editor::drawCursor(POSITION &p, bool visible)
{
    int x = m_borderLeft + (p.z * m_tiles.levelOffsetX()) + (p.x * m_tiles.qWidth());
    const int y = m_borderTop - ((p.z + 1) * m_tiles.levelOffsetY()) + (p.y * m_tiles.qHeight());
    const int w = (m_tiles.qWidth() * 2) + m_tiles.levelOffsetX();
    const int h = (m_tiles.qHeight() * 2) + m_tiles.levelOffsetY();

    if (p.z == 100 || !visible) {
        x = -1;
    }

    m_drawFrame->setRect(x, y, w, h, m_tiles.levelOffsetX(), static_cast<int>(m_mode) - static_cast<int>(EditMode::remove));
    m_drawFrame->update();
}

void Editor::drawFrameMouseMovedEvent(QMouseEvent *e)
{
    // we swallow the draw frames mouse moves and process here

    POSITION mousePos;
    transformPointToPosition(e->pos(), mousePos, (m_mode == EditMode::remove));

    if ((mousePos.x==m_curPos.x) && (mousePos.y==m_curPos.y) && (mousePos.z==m_curPos.z)) {
        return;
    }

    m_curPos = mousePos;

    statusChanged();

    switch (m_mode) {
    case EditMode::insert: {
        POSITION next;
        next = m_curPos;

        if (next.z == 100) {
            next.z = 0;
        } else {
            next.z += 1;
        }

        drawCursor(next, canInsert(next));

        break;
    }
    case EditMode::remove:
            drawCursor(m_curPos, 1);

        break;
    case EditMode::move:
        break;
    }
}

bool Editor::canInsert(POSITION &p) const
{
    // can we inser a tile here. We can iff
    // there are tiles in all positions below us (or we are a ground level)
    // there are no tiles intersecting with us on this level

    if (p.z >= m_theBoard.getDepth()) {
        return false;
    }

    if (p.y > m_theBoard.getHeight() - 2) {
        return false;
    }

    if (p.x > m_theBoard.getWidth() - 2) {
        return false;
    }

    POSITION n = p;

    if (p.z != 0) {
        n.z -= 1;
        if (!m_theBoard.allFilled(n)) {
            return false;
        }
    }

    return !m_theBoard.anyFilled(p);
}

void Editor::closeEvent(QCloseEvent *e)
{
    if (testSave()) {
        m_theBoard.clearBoardLayout();
        m_clean = true;
        m_numTiles = 0;
        statusChanged();
        update();

        // Save the window geometry.
        Prefs::setEditorGeometry(geometry());
        Prefs::self()->save();

        e->accept();
    } else {
        e->ignore();
    }
}

void Editor::setTilesetFromSettings()
{
    const QString tileset(Prefs::tileSet());

    // Exit if the tileset is already set.
    if (tileset == m_tileset) {
        return;
    }

    // Try to load the new tileset.
    if (!m_tiles.loadTileset(tileset)) {
        // Try to load the old one.
        if (!m_tiles.loadTileset(m_tileset)) {
            m_tiles.loadDefault();
        }
    } else {
        // If loading the new tileset was ok, set the new tileset name.
        m_tileset = tileset;
    }

    // Must be called to load the graphics and its informations.
    m_tiles.loadGraphics();

    updateTileSize(size());
}
