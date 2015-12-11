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
    mode(EditMode::insert),
    numTiles(0),
    clean(true),
    tiles()
{
    setModal(true);

    QWidget *mainWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainWidget);

    resize(QSize(800, 400));

    QGridLayout *gridLayout = new QGridLayout(mainWidget);
    QVBoxLayout *layout = new QVBoxLayout();

    setupToolbar();
    layout->addWidget(topToolbar);

    drawFrame = new FrameImage(this, QSize(0, 0));
    drawFrame->setFocusPolicy(Qt::NoFocus);
    drawFrame->setMouseTracking(true);

    layout->addWidget(drawFrame);
    gridLayout->addLayout(layout, 0, 0, 1, 1);

    //toolbar will set our minimum height
    setMinimumHeight(120);

    // tell the user what we do
    setWindowTitle(i18n("Edit Board Layout"));

    connect(drawFrame, &FrameImage::mousePressed, this, &Editor::drawFrameMousePressEvent);
    connect(drawFrame, &FrameImage::mouseMoved, this, &Editor::drawFrameMouseMovedEvent);

    statusChanged();

    update();
}

Editor::~Editor()
{
}

void Editor::updateTileSize(const QSize size)
{
    const int width = theBoard.getWidth();
    const int height = theBoard.getHeight();
    const QSize tileSize = tiles.preferredTileSize(size, width / 2, height / 2);

    tiles.reloadTileset(tileSize);
    borderLeft = (drawFrame->size().width() - (width * tiles.qWidth())) / 2;
    borderTop = (drawFrame->size().height() - (height * tiles.qHeight())) / 2;
}

void Editor::resizeEvent(QResizeEvent *event)
{
    updateTileSize(event->size());
}

void Editor::setupToolbar()
{
    topToolbar = new KToolBar(this, "editToolBar");
    topToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    actionCollection = new KActionCollection(this);

    // new game
    QAction *newBoard = actionCollection->addAction(QStringLiteral("new_board"));
    newBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    newBoard->setText(i18n("New board"));
    connect(newBoard, &QAction::triggered, this, &Editor::newBoard);
    topToolbar->addAction(newBoard);

    // open game
    QAction *openBoard = actionCollection->addAction(QStringLiteral("open_board"));
    openBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    openBoard->setText(i18n("Open board"));
    connect(openBoard, &QAction::triggered, this, &Editor::loadBoard);
    topToolbar->addAction(openBoard);

    // save game
    QAction *saveBoard = actionCollection->addAction(QStringLiteral("save_board"));
    saveBoard->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));
    saveBoard->setText(i18n("Save board"));
    connect(saveBoard, &QAction::triggered, this, &Editor::saveBoard);
    topToolbar->addAction(saveBoard);

    topToolbar->addSeparator();


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
    actionCollection->addAction(QStringLiteral("add_tiles"), addTiles);
    topToolbar->addAction(addTiles);
    KToggleAction *delTiles = new KToggleAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove tiles"), this);
    actionCollection->addAction(QStringLiteral("del_tiles"), delTiles);
    topToolbar->addAction(delTiles);

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

    topToolbar->addSeparator();

    // NOTE: maybe join shiftActions in QActionGroup and create one slot(QAction*) instead of 4 slots? ;)
    // Does this makes sense? dimsuz
    QAction *shiftLeft = actionCollection->addAction(QStringLiteral("shift_left"));
    shiftLeft->setIcon(QIcon::fromTheme(QStringLiteral("go-previous")));
    shiftLeft->setText(i18n("Shift left"));
    connect(shiftLeft, &QAction::triggered, this, &Editor::slotShiftLeft);
    topToolbar->addAction(shiftLeft);

    QAction *shiftUp = actionCollection->addAction(QStringLiteral("shift_up"));
    shiftUp->setIcon(QIcon::fromTheme(QStringLiteral("go-up")));
    shiftUp->setText(i18n("Shift up"));
    connect(shiftUp, &QAction::triggered, this, &Editor::slotShiftUp);
    topToolbar->addAction(shiftUp);

    QAction *shiftDown = actionCollection->addAction(QStringLiteral("shift_down"));
    shiftDown->setIcon(QIcon::fromTheme(QStringLiteral("go-down")));
    shiftDown->setText(i18n("Shift down"));
    connect(shiftDown, &QAction::triggered, this, &Editor::slotShiftDown);
    topToolbar->addAction(shiftDown);

    QAction *shiftRight = actionCollection->addAction(QStringLiteral("shift_right"));
    shiftRight->setIcon(QIcon::fromTheme(QStringLiteral("go-next")));
    shiftRight->setText(i18n("Shift right"));
    connect(shiftRight, &QAction::triggered, this, &Editor::slotShiftRight);
    topToolbar->addAction(shiftRight);

    topToolbar->addSeparator();
    QAction *quit = actionCollection->addAction(KStandardAction::Quit, QStringLiteral("quit"), this, SLOT(close()));
    topToolbar->addAction(quit);

    // status in the toolbar for now (ick)
    QWidget *hbox = new QWidget(topToolbar);
    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();

    theLabel = new QLabel(statusText(), hbox);
    layout->addWidget(theLabel);
    topToolbar->addWidget(hbox);

    topToolbar->adjustSize();
    setMinimumWidth(topToolbar->width());
}

void Editor::statusChanged() const
{
    bool canSave = ((numTiles != 0) && ((numTiles & 1) == 0));
    theLabel->setText(statusText());
    actionCollection->action(QStringLiteral("save_board"))->setEnabled(canSave);
}

void Editor::slotShiftLeft()
{
    theBoard.shiftLeft();
    update();
}

void Editor::slotShiftRight()
{
    theBoard.shiftRight();
    update();
}

void Editor::slotShiftUp()
{
    theBoard.shiftUp();
    update();
}

void Editor::slotShiftDown()
{
    theBoard.shiftDown();
    update();
}

void Editor::slotModeChanged(QAction *act)
{
    if (act == actionCollection->action(QStringLiteral("move_tiles"))) {
        mode = EditMode::move;
    } else if (act == actionCollection->action(QStringLiteral("del_tiles"))) {
        mode = EditMode::remove;
    } else if (act == actionCollection->action(QStringLiteral("add_tiles"))) {
        mode = EditMode::insert;
    }
}

QString Editor::statusText() const
{
    int x = currPos.x;
    int y = currPos.y;
    int z = currPos.e;

    if (z == 100) {
        z = 0;
    } else {
        z = z + 1;
    }

    if (x >= theBoard.getWidth() || x < 0 || y >= theBoard.getHeight() || y < 0) {
        x = y = z = 0;
    }

    return QString(i18n("Tiles: %1 Pos: %2,%3,%4", numTiles, x, y, z));
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

    theBoard.loadBoardLayout(filename);
    update();
}

void Editor::newBoard()
{
    // Clear out the contents of the board. Repaint the screen
    // set values to their defaults.

    if (!testSave()) {
        return;
    }

    theBoard.clearBoardLayout();

    clean = true;
    numTiles = 0;

    statusChanged();
    update();
}

bool Editor::saveBoard()
{
    if (!((numTiles != 0) && ((numTiles & 1) == 0))) {
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

    bool result = theBoard.saveBoardLayout(filename);

    if (result == true) {
        clean = true;

        return true;
    } else {
        return false;
    }
}

bool Editor::testSave()
{
    // test if a save is required and return true if the app is to continue
    // false if cancel is selected. (if ok then call out to save the board

    if (clean) {
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
    QPixmap *dest = drawFrame->getPreviewPixmap();
    buff = QPixmap(dest->width(), dest->height());
    drawBackground(&buff);
    drawTiles(&buff);
    QPainter p(dest);
    p.drawPixmap(0, 0, buff);
    p.end();

    drawFrame->update();
}

void Editor::drawBackground(QPixmap *pixmap) const
{
    const int width = theBoard.getWidth();
    const int height = theBoard.getHeight();
    QPainter p(pixmap);

    // blast in a white background
    p.fillRect(0, 0, pixmap->width(), pixmap->height(), Qt::white);

    // now put in a grid of tile quater width squares
    for (int y = 0; y <= height; y++) {
        int nextY = borderTop + (y * tiles.qHeight());
        p.drawLine(borderLeft, nextY, borderLeft + (width * tiles.qWidth()), nextY);
    }

    for (int x = 0; x <= width; x++) {
        int nextX = borderLeft + (x * tiles.qWidth());
        p.drawLine(nextX, borderTop, nextX, borderTop + (height * tiles.qHeight()));
    }
}

void Editor::drawTiles(QPixmap *dest)
{
    QPainter p(dest);

    const int width = theBoard.getWidth();
    const int height = theBoard.getHeight();
    const int depth = theBoard.getDepth();
    const int shadowX = tiles.width() - tiles.qWidth() * 2 - tiles.levelOffsetX();
    const int shadowY = tiles.height() - tiles.qHeight() * 2 - tiles.levelOffsetY();
    short tile = 0;

    int xOffset = -shadowX;
    int yOffset = -tiles.levelOffsetY();

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z = 0; z < depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x = width - 1; x >= 0; x--) {
                int sx = x * tiles.qWidth() + xOffset + borderLeft;
                int sy = y * tiles.qHeight() + yOffset + borderTop;

                if (theBoard.getBoardData(z, y, x) != '1') {
                    continue;
                }

                QPixmap t;
                tile = (z * depth) + (y * height) + (x * width);
                t = tiles.unselectedTile(0);

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border
                if ((x > 1) && (y > 0) && theBoard.getBoardData(z, y - 1, x - 2) == '1') {
                    p.drawPixmap(sx, sy, t, 0, 0, t.width(), t.height());

                    p.drawPixmap(sx - tiles.qWidth() + shadowX + tiles.levelOffsetX(), sy, t,
                            t.width() - tiles.qWidth(),
                            t.height() - tiles.qHeight() - tiles.levelOffsetX() - shadowY,
                            tiles.qWidth(), tiles.qHeight() + tiles.levelOffsetX());
                } else {
                    p.drawPixmap(sx, sy, t, 0, 0, t.width(), t.height());
                }

                tile++;
                tile = tile % 143;
            }
        }

        xOffset += tiles.levelOffsetX();
        yOffset -= tiles.levelOffsetY();
    }
}

void Editor::transformPointToPosition(const QPoint &point, POSITION &MouseClickPos, bool align) const
{
    // convert mouse position on screen to a tile z y x coord
    // different to the one in kmahjongg.cpp since if we hit ground
    // we return a result too.

    short z = 0;
    short y = 0;
    short x = 0;
    MouseClickPos.e = 100;

    // iterate over z coordinate from top to bottom
    for (z = theBoard.getDepth() - 1; z >= 0; --z) {
        // calculate mouse coordiantes --> position in game board
        // the factor -theTiles.width()/2 must keep track with the
        // offset for blitting in the print zvent (FIX ME)
        x = ((point.x() - borderLeft) - (z + 1) * tiles.levelOffsetX()) / tiles.qWidth();
        y = ((point.y() - borderTop) + z * tiles.levelOffsetX()) / tiles.qHeight();

        // skip when position is illegal
        if (x < 0 || x >= theBoard.getWidth() || y < 0 || y >= theBoard.getHeight()) {
            continue;
        }

        switch (theBoard.getBoardData(z, y, x)) {
        case static_cast<UCHAR>('3'):
            if (align) {
                x--;
                y--;
            }

            break;

        case static_cast<UCHAR>('2'):
            if (align) {
                x--;
            }

            break;

        case static_cast<UCHAR>('4'):
            if (align) {
                y--;
            }

            break;

        case static_cast<UCHAR>('1'):
            break;

        default:
            continue;
        }

        // if gameboard is empty, skip
        if (!theBoard.getBoardData(z, y, x)) {
            continue;
        }

        // here, position is legal
        MouseClickPos.e = z;
        MouseClickPos.y = y;
        MouseClickPos.x = x;
        MouseClickPos.f = theBoard.getBoardData(z, y, x);

        break;
    }

    if (MouseClickPos.e == 100) {
        MouseClickPos.x = x;
        MouseClickPos.y = y;
        MouseClickPos.f=0;
    }
}

void Editor::drawFrameMousePressEvent(QMouseEvent* e)
{
    // we swallow the draw frames mouse clicks and process here

    POSITION mPos;
    transformPointToPosition(e->pos(), mPos, (mode == EditMode::remove));

    switch (mode) {
    case EditMode::remove:
        if (!theBoard.tileAbove(mPos) && mPos.e < theBoard.getDepth() && theBoard.isTileAt(mPos)) {
            theBoard.deleteTile(mPos);
            numTiles--;
            statusChanged();
            drawFrameMouseMovedEvent(e);
            update();
        }

        break;
    case EditMode::insert: {
        POSITION n = mPos;

        if (n.e == 100) {
            n.e = 0;
        } else {
            n.e += 1;
        }

        if (canInsert(n)) {
            theBoard.insertTile(n);
            clean = false;
            numTiles++;
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
    int x = borderLeft + (p.e * tiles.levelOffsetX()) + (p.x * tiles.qWidth());
    const int y = borderTop - ((p.e + 1) * tiles.levelOffsetY()) + (p.y * tiles.qHeight());
    const int w = (tiles.qWidth() * 2) + tiles.levelOffsetX();
    const int h = (tiles.qHeight() * 2) + tiles.levelOffsetY();

    if (p.e == 100 || !visible) {
        x = -1;
    }

    drawFrame->setRect(x, y, w, h, tiles.levelOffsetX(), static_cast<int>(mode) - static_cast<int>(EditMode::remove));
    drawFrame->update();
}

void Editor::drawFrameMouseMovedEvent(QMouseEvent *e)
{
    // we swallow the draw frames mouse moves and process here

    POSITION mPos;
    transformPointToPosition(e->pos(), mPos, (mode == EditMode::remove));

    if ((mPos.x==currPos.x) && (mPos.y==currPos.y) && (mPos.e==currPos.e)) {
        return;
    }

    currPos = mPos;

    statusChanged();

    switch (mode) {
    case EditMode::insert: {
        POSITION next;
        next = currPos;

        if (next.e == 100) {
            next.e = 0;
        } else {
            next.e += 1;
        }

        drawCursor(next, canInsert(next));

        break;
    }
    case EditMode::remove:
            drawCursor(currPos, 1);

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

    if (p.e >= theBoard.getDepth()) {
        return false;
    }

    if (p.y > theBoard.getHeight() - 2) {
        return false;
    }

    if (p.x > theBoard.getWidth() - 2) {
        return false;
    }

    POSITION n = p;

    if (p.e != 0) {
        n.e -= 1;
        if (!theBoard.allFilled(n)) {
            return false;
        }
    }

    return !theBoard.anyFilled(p);
}

void Editor::closeEvent(QCloseEvent *e)
{
    if (testSave()) {
        theBoard.clearBoardLayout();
        clean = true;
        numTiles = 0;
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
    if (tileset == mTileset) {
        return;
    }

    // Try to load the new tileset.
    if (!tiles.loadTileset(tileset)) {
        // Try to load the old one.
        if (!tiles.loadTileset(mTileset)) {
            tiles.loadDefault();
        }
    } else {
        // If loading the new tileset was ok, set the new tileset name.
        mTileset = tileset;
    }

    // Must be called to load the graphics and its informations.
    tiles.loadGraphics();

    updateTileSize(size());
}
