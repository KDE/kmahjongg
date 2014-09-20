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
#include "prefs.h"

#include <QLabel>
#include <qevent.h>
#include <qpainter.h>
#include <QHBoxLayout>
#include <QGridLayout>

#include <kmessagebox.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <kicon.h>


Editor::Editor(QWidget *parent)
    : KDialog( parent ),
    tiles()
{
    setModal(true);
    clean = true;
    numTiles = 0;
    mode = insert;

    QWidget *mainWidget = new QWidget(this);
    setMainWidget(mainWidget);

    QGridLayout *gridLayout = new QGridLayout(mainWidget);
    QVBoxLayout *layout = new QVBoxLayout();

    // setup the tool bar
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
    setCaption(i18n("Edit Board Layout"));

    connect(drawFrame, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(
        drawFrameMousePressEvent(QMouseEvent*)));
    connect(drawFrame, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(
        drawFrameMouseMovedEvent(QMouseEvent*)));

    statusChanged();

    setButtons(KDialog::None);

    update();
}

Editor::~Editor()
{
}

void Editor::setTileset(const QString tileset)
{
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

const QString Editor::getTileset() const
{
    return mTileset;
}

void Editor::updateTileSize(const QSize size)
{
    QSize tileSize = tiles.preferredTileSize(size, theBoard.m_width / 2, theBoard.m_height / 2);
    tiles.reloadTileset(tileSize);

    borderLeft = (drawFrame->size().width() - (theBoard.m_width * tiles.qWidth())) / 2;
    borderTop = (drawFrame->size().height() - (theBoard.m_height * tiles.qHeight())) / 2;
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
    QAction *newBoard = actionCollection->addAction(QLatin1String("new_board"));
    newBoard->setIcon(KIcon(QLatin1String("document-new")));
    newBoard->setText(i18n("New board"));
    connect(newBoard, SIGNAL(triggered(bool)), SLOT(newBoard()));
    topToolbar->addAction(newBoard);

    // open game
    QAction *openBoard = actionCollection->addAction(QLatin1String("open_board"));
    openBoard->setIcon(KIcon(QLatin1String("document-open")));
    openBoard->setText(i18n("Open board"));
    connect(openBoard, SIGNAL(triggered(bool)), SLOT(loadBoard()));
    topToolbar->addAction(openBoard);

    // save game
    QAction *saveBoard = actionCollection->addAction(QLatin1String("save_board"));
    saveBoard->setIcon(KIcon(QLatin1String("document-save")));
    saveBoard->setText(i18n("Save board"));
    connect(saveBoard, SIGNAL(triggered(bool)), SLOT(saveBoard()));
    topToolbar->addAction(saveBoard);
    // NOTE dimsuz: how to port this? is it even needed?
    //topToolbar->setButtonIconSet(ID_TOOL_SAVE,loader->loadIconSet("document-save", KIconLoader::Toolbar));

    topToolbar->addSeparator();


#ifdef FUTURE_OPTIONS


    // Select
    QAction *select = actionCollection->addAction(QLatin1String("select"));
    select->setIcon(KIcon(QLatin1String("rectangle_select")));
    select->setText(i18n("Select"));
    topToolbar->addAction(select);

    // NOTE: use kstandarddactions?
    QAction *cut = actionCollection->addAction(QLatin1String("edit_cut"));
    cut->setIcon(KIcon(QLatin1String("edit-cut")));
    cut->setText(i18n("Cut"));
    topToolbar->addAction(cut);

    QAction *copy = actionCollection->addAction(QLatin1String("edit_copy"));
    copy->setIcon(KIcon(QLatin1String("edit-copy")));
    copy->setText(i18n("Copy"));
    topToolbar->addAction(copy);

    QAction *paste = actionCollection->addAction(QLatin1String("edit_paste"));
    paste->setIcon(KIcon(QLatin1String("edit-paste")));
    paste->setText(i18n("Paste"));
    topToolbar->addAction(paste);

    topToolbar->addSeparator();

    QAction *moveTiles = actionCollection->addAction(QLatin1String("move_tiles"));
    moveTiles->setIcon(KIcon(QLatin1String("move")));
    moveTiles->setText(i18n("Move tiles"));
    topToolbar->addAction(moveTiles);


#endif


    KToggleAction *addTiles = new KToggleAction(KIcon(QLatin1String("draw-freehand")), i18n("Add ti"
        "les"), this);
    actionCollection->addAction(QLatin1String("add_tiles"), addTiles);
    topToolbar->addAction(addTiles);
    KToggleAction *delTiles = new KToggleAction(KIcon(QLatin1String("edit-delete")), i18n("Remove t"
        "iles" ), this);
    actionCollection->addAction(QLatin1String("del_tiles"), delTiles);
    topToolbar->addAction(delTiles);

    QActionGroup *radioGrp = new QActionGroup(this);
    radioGrp->setExclusive(true);
    radioGrp->addAction(addTiles);
    addTiles->setChecked(true);


#ifdef FUTURE_OPTIONS


    radioGrp->addAction(moveTiles);


#endif


    radioGrp->addAction(delTiles);
    connect(radioGrp, SIGNAL(triggered(QAction*)), SLOT(slotModeChanged(QAction*)));

    // board shift

    topToolbar->addSeparator();

    // NOTE: maybe join shiftActions in QActionGroup and create one slot(QAction*) instead of 4 slots? ;)
    // Does this makes sense? dimsuz
    QAction *shiftLeft = actionCollection->addAction(QLatin1String("shift_left"));
    shiftLeft->setIcon(KIcon(QLatin1String("go-previous")));
    shiftLeft->setText(i18n("Shift left"));
    connect(shiftLeft, SIGNAL(triggered(bool)), SLOT(slotShiftLeft()));
    topToolbar->addAction(shiftLeft);

    QAction *shiftUp = actionCollection->addAction(QLatin1String("shift_up"));
    shiftUp->setIcon(KIcon(QLatin1String("go-up")));
    shiftUp->setText(i18n("Shift up"));
    connect(shiftUp, SIGNAL(triggered(bool)), SLOT(slotShiftUp()));
    topToolbar->addAction(shiftUp);

    QAction *shiftDown = actionCollection->addAction(QLatin1String("shift_down"));
    shiftDown->setIcon(KIcon(QLatin1String("go-down")));
    shiftDown->setText(i18n("Shift down"));
    connect(shiftDown, SIGNAL(triggered(bool)), SLOT(slotShiftDown()));
    topToolbar->addAction(shiftDown);

    QAction *shiftRight = actionCollection->addAction(QLatin1String("shift_right"));
    shiftRight->setIcon(KIcon(QLatin1String("go-next")));
    shiftRight->setText(i18n("Shift right"));
    connect(shiftRight, SIGNAL(triggered(bool)), SLOT(slotShiftRight()));
    topToolbar->addAction(shiftRight);

    topToolbar->addSeparator();
    QAction *quit = actionCollection->addAction(KStandardAction::Quit, QLatin1String("quit"), this,
        SLOT(close()));
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

void Editor::statusChanged()
{
    bool canSave = ((numTiles != 0) && ((numTiles & 1) == 0));
    theLabel->setText(statusText());
    actionCollection->action("save_board")->setEnabled(canSave);
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
    if (act == actionCollection->action("move_tiles")) {
        mode = move;
    } else if (act == actionCollection->action("del_tiles")) {
        mode = remove;
    } else if (act == actionCollection->action("add_tiles")) {
        mode = insert;
    }
}

QString Editor::statusText()
{
    QString buf;

    int x = currPos.x;
    int y = currPos.y;
    int z = currPos.e;

    if (z == 100) {
        z = 0;
    } else {
        z = z + 1;
    }

    if (x >= theBoard.m_width || x < 0 || y >= theBoard.m_height || y < 0) {
        x = y = z = 0;
    }

    buf = i18n("Tiles: %1 Pos: %2,%3,%4", numTiles, x, y, z);

    return buf;
}


void Editor::loadBoard()
{
    if (!testSave()) {
        return;
    }

    QUrl url = KFileDialog::getOpenUrl(QUrl(), i18n("*.layout|Board Layout (*.layout)\n*|All File"
        "s"), this, i18n("Open Board Layout"));

    if (url.isEmpty()) {
            return;
    }

    theBoard.loadBoardLayout(url.path());
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
        KMessageBox::sorry(this, i18n( "You can only save with a even number of tiles."));

        return false;
    }

    // get a save file name
    QUrl url = KFileDialog::getSaveUrl(QUrl(), i18n("*.layout|Board Layout (*.layout)\n*|All File"
        "s"), this, i18n("Save Board Layout"));

    if (url.isEmpty()) {
        return false;
    }

    if (!url.isLocalFile()) {
        KMessageBox::sorry(this, i18n("Only saving to local files currently supported."));

        return false;
    }

    QFileInfo f(url.path());
    if (f.exists()) {
        // if it already exists, querie the user for replacement
        int res = KMessageBox::warningContinueCancel(this, i18n("A file with that name already exis"
            "ts. Do you wish to overwrite it?"), i18n("Save Board Layout" ),
            KStandardGuiItem::save());

        if (res != KMessageBox::Continue) {
            return false;
        }
    }

    bool result = theBoard.saveBoardLayout(url.path());

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

    int res;
    res = KMessageBox::warningYesNoCancel(this, i18n("The board has been modified. Would you like t"
        "o save the changes?"), QString(), KStandardGuiItem::save(),KStandardGuiItem::dontSave());

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
    QPixmap *dest=drawFrame->getPreviewPixmap();
    buff = QPixmap(dest->width(), dest->height());
    drawBackground(&buff);
    drawTiles(&buff);
    QPainter p(dest);
    p.drawPixmap(0, 0, buff);
    p.end();

    drawFrame->update();
}

void Editor::drawBackground(QPixmap *pixmap)
{
    QPainter p(pixmap);

    // blast in a white background
    p.fillRect(0, 0, pixmap->width(), pixmap->height(), Qt::white);

    // now put in a grid of tile quater width squares
    int sy = tiles.qHeight();
    int sx = tiles.qWidth();

    for (int y = 0; y <= theBoard.m_height; y++) {
        int nextY = borderTop + (y * tiles.qHeight());
        p.drawLine(borderLeft, nextY, borderLeft + (theBoard.m_width * tiles.qWidth()), nextY);
    }

    for (int x = 0; x <= theBoard.m_width; x++) {
        int nextX = borderLeft + (x * tiles.qWidth());
        p.drawLine(nextX, borderTop, nextX, borderTop + (theBoard.m_height * tiles.qHeight()));
    }
}

void Editor::drawTiles(QPixmap *dest)
{
    QPainter p(dest);

    int shadowX = tiles.width() - tiles.qWidth() * 2 - tiles.levelOffsetX();
    int shadowY = tiles.height() - tiles.qHeight() * 2 - tiles.levelOffsetY();
    short tile = 0;

    int xOffset = -shadowX;
    int yOffset = -tiles.levelOffsetY();

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z = 0; z < theBoard.m_depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < theBoard.m_height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x = theBoard.m_width - 1; x >= 0; x--) {
                int sx = x * tiles.qWidth() + xOffset + borderLeft;
                int sy = y * tiles.qHeight() + yOffset + borderTop;

                if (theBoard.getBoardData(z, y, x) != '1') {
                    continue;
                }

                QPixmap t;
                tile = (z * theBoard.m_depth) + (y * theBoard.m_height) + (x * theBoard.m_width);
//                 if (mode==remove && currPos.x==x && currPos.y==y && currPos.e==z) {
//                     t = tiles.selectedPixmaps(44));
//                 } else {
                t = tiles.unselectedTile(0);
//                 }

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border
                if ((x > 1) && (y > 0) && theBoard.getBoardData(z, y - 1, x - 2) == '1') {
//                     p.drawPixmap( sx+tiles.levelOffsetX(), sy, t, tiles.levelOffsetX() , 0,
//                         t.width() - tiles.levelOffsetX(), t.height() / 2);
// 
//                     p.drawPixmap(sx, sy + t.height() / 2, t, 0, t.height() / 2, t.width(),
//                         t.height() / 2);

                    p.drawPixmap(sx, sy, t, 0, 0, t.width(), t.height());

                    p.drawPixmap(sx - tiles.qWidth() + shadowX + tiles.levelOffsetX(), sy, t, t.width() - tiles.qWidth(), t.height() - tiles.qHeight() - tiles.levelOffsetX() - shadowY,
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

void Editor::transformPointToPosition(const QPoint &point, POSITION &MouseClickPos, bool align)
{
    // convert mouse position on screen to a tile z y x coord
    // different to the one in kmahjongg.cpp since if we hit ground
    // we return a result too.

    short z = 0; // shut the compiler up about maybe uninitialised errors
    short y = 0;
    short x = 0;
    MouseClickPos.e = 100;

    // iterate over z coordinate from top to bottom
    for (z = theBoard.m_depth - 1; z >= 0; --z) {
        // calculate mouse coordiantes --> position in game board
        // the factor -theTiles.width()/2 must keep track with the
        // offset for blitting in the print zvent (FIX ME)
        x = ((point.x() - borderLeft) - (z + 1) * tiles.levelOffsetX()) / tiles.qWidth();
        y = ((point.y() - borderTop) + z * tiles.levelOffsetX()) / tiles.qHeight();

        // skip when position is illegal
        if (x < 0 || x >= theBoard.m_width || y < 0 || y >= theBoard.m_height) {
            continue;
        }

        switch (theBoard.getBoardData(z, y, x)) {
        case (UCHAR) '3':
            if (align) {
                x--;
                y--;
            }

            break;

        case (UCHAR) '2':
            if (align) {
                x--;
            }

            break;

        case (UCHAR) '4':
            if (align) {
                y--;
            }

            break;

        case (UCHAR) '1':
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

void Editor::drawFrameMousePressEvent( QMouseEvent* e )
{
    // we swallow the draw frames mouse clicks and process here

    POSITION mPos;
    transformPointToPosition(e->pos(), mPos, (mode == remove));

    switch (mode) {
    case remove:
        if (!theBoard.tileAbove(mPos) && mPos.e < theBoard.m_depth && theBoard.isTileAt(mPos)) {
            theBoard.deleteTile(mPos);
            numTiles--;
            statusChanged();
            drawFrameMouseMovedEvent(e);
            update();
        }

        break;
    case insert: {
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
    int y = borderTop - ((p.e + 1) * tiles.levelOffsetY()) + (p.y * tiles.qHeight());
    int w = (tiles.qWidth() * 2) + tiles.levelOffsetX();
    int h = (tiles.qHeight() * 2) + tiles.levelOffsetY();

    if (p.e == 100 || !visible) {
        x = -1;
    }

    drawFrame->setRect(x, y, w, h, tiles.levelOffsetX(), mode-remove);
    drawFrame->update();
}

void Editor::drawFrameMouseMovedEvent(QMouseEvent *e)
{
    // we swallow the draw frames mouse moves and process here

    POSITION mPos;
    transformPointToPosition(e->pos(), mPos, (mode == remove));

    if ((mPos.x==currPos.x) && (mPos.y==currPos.y) && (mPos.e==currPos.e)) {
        return;
    }

    currPos = mPos;

    statusChanged();

    switch(mode) {
    case insert: {
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
    case remove:
            drawCursor(currPos, 1);

        break;
    case move:
        break;
    }
}

bool Editor::canInsert(POSITION &p)
{
    // can we inser a tile here. We can iff
    // there are tiles in all positions below us (or we are a ground level)
    // there are no tiles intersecting with us on this level

    if (p.e >= theBoard.m_depth) {
        return false;
    }

    if (p.y > theBoard.m_height - 2) {
        return false;
    }

    if (p.x > theBoard.m_width - 2) {
        return false;
    }

    POSITION n = p;

    if (p.e != 0) {
        n.e -= 1;
        if (!theBoard.allFilled(n)) {
            return false;
        }
    }

    int any = theBoard.anyFilled(p);

    return (!any);
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
    setTileset(Prefs::tileSet());
}


#include "Editor.moc"
