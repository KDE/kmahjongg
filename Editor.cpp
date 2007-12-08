/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>

    Kmahjongg is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Editor.h"

#include <QLayout>
#include <QLabel>
#include <qevent.h>
#include <qpainter.h>

#include "prefs.h"

#include <kmessagebox.h>
#include <kcomponentdata.h>
#include <klocale.h>     // Needed to use KLocale
#include <kstandarddirs.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <kicon.h>

// When we assign a tile to draw in a slot we do it in order from te following
// table, wrapping on the tile number. It makes the tile layout look more
// random.


Editor::Editor ( QWidget* parent)
    : KDialog( parent ), tiles()
{
    setModal(true);
    clean= true;
    numTiles=0;
    mode = insert;

    QString tile = Prefs::tileSet();
    if (!tiles.loadTileset(tile)) tiles.loadDefault();

    //TODO delay this initialization, must define board dimensions
    int sWidth = ( theBoard.m_width+2)*(tiles.qWidth());
    int sHeight =( theBoard.m_height+2)*tiles.qHeight();

    sWidth += 4*tiles.levelOffsetX();

    drawFrame = new FrameImage( this, QSize(sWidth, sHeight) );
    drawFrame->setGeometry( 10, 40 ,sWidth ,sHeight);
    drawFrame->setMinimumSize( 0, 0 );
    drawFrame->setMaximumSize( 32767, 32767 );
    drawFrame->setFocusPolicy( Qt::NoFocus );
    //drawFrame->setFrameStyle( 49 );
    drawFrame->setMouseTracking(true);

    // setup the tool bar
    setupToolbar();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(topToolbar,0);
    layout->addWidget(drawFrame,1);
    layout->activate();

    resize( sWidth+60, sHeight+60);
    //toolbar will set our minimum height
    setMinimumHeight(120);

    // tell the user what we do
    setCaption(i18n("Edit Board Layout"));

    connect( drawFrame, SIGNAL(mousePressed(QMouseEvent *) ),
             SLOT(drawFrameMousePressEvent(QMouseEvent *)));
    connect( drawFrame, SIGNAL(mouseMoved(QMouseEvent *) ),
             SLOT(drawFrameMouseMovedEvent(QMouseEvent *)));

    statusChanged();

    update();
}



Editor::~Editor()
{
}

void Editor::resizeEvent ( QResizeEvent * event )
{
    QSize newtiles = tiles.preferredTileSize(event->size(), (theBoard.m_width+2)/2,( theBoard.m_height+2)/2);
    tiles.reloadTileset(newtiles);
}

// ---------------------------------------------------------
void Editor::setupToolbar()
{

    topToolbar = new KToolBar( this, "editToolBar" );
    topToolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );

    actionCollection = new KActionCollection(this);
    // new game
    QAction* newBoard = actionCollection->addAction("new_board");
    newBoard->setIcon(KIcon("document-new"));
    newBoard->setText(i18n("New board"));
    connect(newBoard, SIGNAL(triggered(bool)), SLOT(newBoard()));
    topToolbar->addAction(newBoard);
    // open game
    QAction* openBoard = actionCollection->addAction("open_board");
    openBoard->setIcon(KIcon("document-open"));
    openBoard->setText(i18n("Open board"));
    connect(openBoard, SIGNAL(triggered(bool)), SLOT(loadBoard()));
    topToolbar->addAction(openBoard);
    // save game
    QAction* saveBoard = actionCollection->addAction("save_board");
    saveBoard->setIcon(KIcon("document-save"));
    saveBoard->setText(i18n("Save board"));
    connect(saveBoard, SIGNAL(triggered(bool)), SLOT(saveBoard()));
    topToolbar->addAction(saveBoard);
    // NOTE dimsuz: how to port this? is it even needed?
    //topToolbar->setButtonIconSet(ID_TOOL_SAVE,loader->loadIconSet("document-save", KIconLoader::Toolbar));

    topToolbar->addSeparator();
#ifdef FUTURE_OPTIONS
    // Select
    QAction* select = actionCollection->addAction("select");
    select->setIcon(KIcon("rectangle_select"));
    select->setText(i18n("Select"));
    topToolbar->addAction(select);

    // NOTE: use kstandarddactions?
    QAction* cut = actionCollection->addAction("edit_cut");
    cut->setIcon(KIcon("edit-cut"));
    cut->setText(i18n("Cut"));
    topToolbar->addAction(cut);

    QAction* copy = actionCollection->addAction("edit_copy");
    copy->setIcon(KIcon("edit-copy"));
    copy->setText(i18n("Copy"));
    topToolbar->addAction(copy);

    QAction* paste = actionCollection->addAction("edit_paste");
    paste->setIcon(KIcon("edit-paste"));
    paste->setText(i18n("Paste"));
    topToolbar->addAction(paste);

    topToolbar->addSeparator();

    QAction* moveTiles = actionCollection->addAction("move_tiles");
    moveTiles->setIcon(KIcon("move"));
    moveTiles->setText(i18n("Move tiles"));
    topToolbar->addAction(moveTiles);
#endif
    KToggleAction* addTiles = new KToggleAction(KIcon("draw-freehand"), i18n("Add tiles"), this);
    actionCollection->addAction("add_tiles", addTiles);
    topToolbar->addAction(addTiles);
    KToggleAction* delTiles = new KToggleAction(KIcon("edit-delete"), i18n("Remove tiles"), this);
    actionCollection->addAction("del_tiles", delTiles);
    topToolbar->addAction(delTiles);

    QActionGroup* radioGrp = new QActionGroup(this);
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
    QAction* shiftLeft = actionCollection->addAction("shift_left");
    shiftLeft->setIcon(KIcon("go-previous"));
    shiftLeft->setText(i18n("Shift left"));
    connect(shiftLeft, SIGNAL(triggered(bool)), SLOT(slotShiftLeft()));
    topToolbar->addAction(shiftLeft);

    QAction* shiftUp = actionCollection->addAction("shift_up");
    shiftUp->setIcon(KIcon("go-up"));
    shiftUp->setText(i18n("Shift up"));
    connect(shiftUp, SIGNAL(triggered(bool)), SLOT(slotShiftUp()));
    topToolbar->addAction(shiftUp);

    QAction* shiftDown = actionCollection->addAction("shift_down");
    shiftDown->setIcon(KIcon("go-down"));
    shiftDown->setText(i18n("Shift down"));
    connect(shiftDown, SIGNAL(triggered(bool)), SLOT(slotShiftDown()));
    topToolbar->addAction(shiftDown);

    QAction* shiftRight = actionCollection->addAction("shift_right");
    shiftRight->setIcon(KIcon("go-next"));
    shiftRight->setText(i18n("Shift right"));
    connect(shiftRight, SIGNAL(triggered(bool)), SLOT(slotShiftRight()));
    topToolbar->addAction(shiftRight);

    topToolbar->addSeparator();
    QAction* quit = actionCollection->addAction(KStandardAction::Quit, "quit",
                                                this, SLOT(close()));
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

void Editor::statusChanged() {
    bool canSave = ((numTiles !=0) && ((numTiles & 1) == 0));
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

void Editor::slotModeChanged(QAction* act)
{
    if(act == actionCollection->action("move_tiles"))
    {
        mode = move;
    }
    else if(act == actionCollection->action("del_tiles"))
    {
        mode = remove;
    }
    else if(act == actionCollection->action("add_tiles"))
    {
        mode = insert;
    }
}

QString Editor::statusText() {
	QString buf;

	int x=currPos.x;
	int y=currPos.y;
	int z= currPos.e;

	if (z == 100)
		z = 0;
	else
		z=z+1;

	if (x >= theBoard.m_width || x <0 || y >=theBoard.m_height || y <0)
		x = y = z = 0;

	buf = i18n("Tiles: %1 Pos: %2,%3,%4", numTiles, x, y, z);
	return buf;
}


void Editor::loadBoard() {

    if ( !testSave() )
	return;

    KUrl url = KFileDialog::getOpenUrl(
				KUrl(),
				i18n("*.layout|Board Layout (*.layout)\n"
				"*|All Files"),
				this,
				i18n("Open Board Layout" ));

   if ( url.isEmpty() )
        return;


    theBoard.loadBoardLayout( url.path() );
    update();
}


// Clear out the contents of the board. Repaint the screen
// set values to their defaults.
void Editor::newBoard() {
    if (!testSave())
	return;

    theBoard.clearBoardLayout();

    clean=true;
    numTiles=0;
    statusChanged();
    update();
}

bool Editor::saveBoard() {
    // get a save file name
    KUrl url = KFileDialog::getSaveUrl(
				KUrl(),
				i18n("*.layout|Board Layout (*.layout)\n"
				"*|All Files"),
				this,
				i18n("Save Board Layout" ));

   if ( url.isEmpty() )
       return false;

   if( !url.isLocalFile() )
   {
      KMessageBox::sorry( this, i18n( "Only saving to local files currently supported." ) );
      return false;
   }


    QFileInfo f( url.path() );
    if ( f.exists() ) {
	// if it already exists, querie the user for replacement
	int res=KMessageBox::warningContinueCancel(this,
			i18n("A file with that name "
					   "already exists. Do you "
					   "wish to overwrite it?"),
					i18n("Save Board Layout" ), KStandardGuiItem::save());
	if (res != KMessageBox::Continue)
		return false;
    }

    bool result = theBoard.saveBoardLayout( url.path() );
    if (result==true){
        clean = true;
        return true;
    } else {
	return false;
    }
}

// test if a save is required and return true if the app is to continue
// false if cancel is selected. (if ok then call out to save the board
bool Editor::testSave()
{

    if (clean)
	return(true);

    int res;
    res=KMessageBox::warningYesNoCancel(this,
	i18n("The board has been modified. Would you "
		"like to save the changes?"), QString(), KStandardGuiItem::save(),KStandardGuiItem::dontSave());

    if (res == KMessageBox::Yes) {
	// yes to save
	if (saveBoard()) {
  	    return true;
	} else {
	    KMessageBox::sorry(this, i18n("Save failed. Aborting operation."));
	}
    } else {
	return (res != KMessageBox::Cancel);
    }
    return(true);
}


// The main paint event, draw in the grid and blit in
// the tiles as specified by the layout.

void Editor::paintEvent( QPaintEvent*  ) {
    // first we layer on a background grid
    QPixmap buff;
    QPixmap *dest=drawFrame->getPreviewPixmap();
    buff = QPixmap(dest->width(), dest->height());
    drawBackground(&buff);
    drawTiles(&buff);
    QPainter p(dest);
    p.drawPixmap(0,0, buff);

    drawFrame->update();
}

void Editor::drawBackground(QPixmap *pixmap) {

    QPainter p(pixmap);

    // blast in a white background
    p.fillRect(0,0,pixmap->width(), pixmap->height(), Qt::white);


    // now put in a grid of tile quater width squares
    int sy = (tiles.height()/2)+tiles.levelOffsetX();
    int sx = (tiles.width()/2);

    for (int y=0; y<=theBoard.m_height; y++) {
	int nextY=sy+(y*tiles.qHeight());
	p.drawLine(sx, nextY,sx+(theBoard.m_width*tiles.qWidth()), nextY);
    }

    for (int x=0; x<=theBoard.m_width; x++) {
	int nextX=sx+(x*tiles.qWidth());
	p.drawLine(nextX, sy, nextX, sy+theBoard.m_height*tiles.qHeight());
    }
}

void Editor::drawTiles(QPixmap *dest) {

    QPainter p(dest);

    int xOffset = tiles.width()/2;
    int yOffset = tiles.height()/2;
    short tile = 0;

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z=0; z< theBoard.m_depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < theBoard.m_height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x= theBoard.m_width-1; x>=0; x--) {
                int sx = x*(tiles.qWidth()  )+xOffset;
                int sy = y*(tiles.qHeight()  )+yOffset;
                if (theBoard.getBoardData(z, y, x) != '1') {
                    continue;
                }
		QPixmap t;
		tile=(z*theBoard.m_depth)+
			(y*theBoard.m_height)+
				(x*theBoard.m_width);
//		if (mode==remove && currPos.x==x && currPos.y==y && currPos.e==z) {
//                   t = tiles.selectedPixmaps(44));
//		} else {
                   t = tiles.unselectedTile(0);
//		}

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border
                if ((x>1) && (y>0) && theBoard.getBoardData(z,y-1,x-2)=='1'){
                    p.drawPixmap( sx+tiles.levelOffsetX(), sy,
                            t, tiles.levelOffsetX() ,0,
                            t.width()-tiles.levelOffsetX(),
                            t.height()/2);


                    p.drawPixmap( sx, sy+t.height()/2,
                        t, 0,t.height()/2,t.width(),t.height()/2);
                } else {

                p.drawPixmap(sx, sy,
                    t, 0,0, t.width(), t.height());
                }


                tile++;
                tile = tile % 143;
            }
        }
        xOffset +=tiles.levelOffsetX();
        yOffset -=tiles.levelOffsetY();
    }
}


// convert mouse position on screen to a tile z y x coord
// different to the one in kmahjongg.cpp since if we hit ground
// we return a result too.

void Editor::transformPointToPosition(
        const QPoint& point,
        POSITION& MouseClickPos,
	bool align)
{

    short z = 0; // shut the compiler up about maybe uninitialised errors
    short y = 0;
    short x = 0;
    MouseClickPos.e = 100;

    // iterate over z coordinate from top to bottom
    for( z=theBoard.m_depth-1; z>=0; z-- )
    {
        // calculate mouse coordiantes --> position in game board
	// the factor -theTiles.width()/2 must keep track with the
	// offset for blitting in the print zvent (FIX ME)
        x = ((point.x()-tiles.width()/2)-(z+1)*tiles.levelOffsetX())/ tiles.qWidth();
        y = ((point.y()-tiles.height()/2)+ z*tiles.levelOffsetX()) / tiles.qHeight();


        // skip when position is illegal
        if (x<0 || x>=theBoard.m_width || y<0 || y>=theBoard.m_height)
		continue;

        //
        switch( theBoard.getBoardData(z,y,x) )
        {
	case (UCHAR)'3':    if (align) {
				    x--;
                                    y--;
	                        }
                                break;

            case (UCHAR)'2':    if (align)
				    x--;
                                break;

            case (UCHAR)'4':    if (align)
				    y--;
                                break;

            case (UCHAR)'1':    break;

            default :           continue;
        }
        // if gameboard is empty, skip
        if ( ! theBoard.getBoardData(z,y,x) )
		continue;

        // here, position is legal
        MouseClickPos.e = z;
        MouseClickPos.y = y;
        MouseClickPos.x = x;
        MouseClickPos.f = theBoard.getBoardData(z,y,x);
	break;
    }
    if (MouseClickPos.e == 100) {
    	MouseClickPos.x = x;
    	MouseClickPos.y = y;
    	MouseClickPos.f=0;
    }
}


// we swallow the draw frames mouse clicks and process here
void Editor::drawFrameMousePressEvent( QMouseEvent* e )
{

	POSITION mPos;
	transformPointToPosition(e->pos(), mPos, (mode == remove));

	switch (mode) {
		case remove:
		    if (!theBoard.tileAbove(mPos) && mPos.e < theBoard.m_depth && theBoard.isTileAt(mPos) ) {
			theBoard.deleteTile(mPos);
			numTiles--;
			statusChanged();
			drawFrameMouseMovedEvent(e);
                        update();
		    }
		break;
		case insert: {
		    POSITION n = mPos;
		    if (n.e == 100)
			n.e = 0;
		    else
			n.e += 1;
		    if (canInsert(n)) {
			theBoard.insertTile(n);
			numTiles++;
			statusChanged();
			update();
		    }
		  }
		break;
		default:
		break;
	}

}


void Editor::drawCursor(POSITION &p, bool visible)
{
    int x = (tiles.width()/2)+(p.e*tiles.levelOffsetX())+(p.x * tiles.qWidth());
    int y = (tiles.height()/2)-(p.e*tiles.levelOffsetX())+(p.y * tiles.qHeight());
    int w = tiles.width();
    int h = tiles.height();


    if (p.e==100 || !visible)
	x = -1;
    drawFrame->setRect(x,y,w,h, tiles.levelOffsetX(), mode-remove);
    drawFrame->update();
}



// we swallow the draw frames mouse moves and process here
void Editor::drawFrameMouseMovedEvent( QMouseEvent* e ){

    POSITION mPos;
    transformPointToPosition(e->pos(), mPos, (mode == remove));

    if ((mPos.x==currPos.x) && (mPos.y==currPos.y) && (mPos.e==currPos.e))
	return;
    currPos = mPos;

    statusChanged();

    switch(mode) {
	case insert: {
		POSITION next;
		next = currPos;
		if (next.e == 100)
			next.e = 0;
		else
			next.e += 1;

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

// can we inser a tile here. We can iff
//	there are tiles in all positions below us (or we are a ground level)
//	there are no tiles intersecting with us on this level

bool Editor::canInsert(POSITION &p) {


    if (p.e >= theBoard.m_depth)
	return (false);
    if (p.y >theBoard.m_height-2)
	return false;
    if (p.x >theBoard.m_width-2)
	return false;

    POSITION n = p;
    if (p.e != 0) {
	n.e -= 1;
	if (!theBoard.allFilled(n)) {
	    return(false);
	}
    }
    int any = theBoard.anyFilled(p);
    return(!any);

}



#include "Editor.moc"
