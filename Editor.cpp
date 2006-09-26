
#include <QLayout>
#include <qevent.h>
#include <qpainter.h>

#include "Editor.h"
#include "prefs.h"

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>     // Needed to use KLocale
#include <kstandarddirs.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kstdaction.h>
#include <kicon.h>

// When we assign a tile to draw in a slot we do it in order from te following
// table, wrapping on the tile number. It makes the tile layout look more
// random.


Editor::Editor ( QWidget* parent)
    : QDialog( parent ), tiles()
{
    setModal(true);
    clean= true;
    numTiles=0;
    mode = insert;

   QString tile = Prefs::tileSet();
   if (!tiles.loadTileset(tile)) tiles.loadDefault();

    int sWidth = (BoardLayout::width+2)*(tiles.qWidth());
    int sHeight =( BoardLayout::height+2)*tiles.qHeight();

    sWidth += 4*tiles.levelOffset();

    drawFrame = new FrameImage( this, QSize(sWidth, sHeight) );
    drawFrame->setGeometry( 10, 40 ,sWidth ,sHeight);
    drawFrame->setMinimumSize( 0, 0 );
    drawFrame->setMaximumSize( 32767, 32767 );
    drawFrame->setFocusPolicy( Qt::NoFocus );
    drawFrame->setFrameStyle( 49 );
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
   setWindowTitle(KInstance::makeStdCaption(i18n("Edit Board Layout")));


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
    QSize newtiles = tiles.preferredTileSize(event->size(), (BoardLayout::width+2)/2,( BoardLayout::height+2)/2);
    tiles.reloadTileset(newtiles);
}

// ---------------------------------------------------------
void Editor::setupToolbar()
{

    topToolbar = new KToolBar( this, "editToolBar" );
    topToolbar->setToolButtonStyle( Qt::ToolButtonIconOnly );

    actionCollection = new KActionCollection(this);
    // new game
    KAction* newBoard = new KAction(KIcon("filenew"), i18n("New board"), actionCollection, "new_board");
    connect(newBoard, SIGNAL(triggered(bool)), SLOT(newBoard()));
    topToolbar->addAction(newBoard);
    // open game
    KAction* openBoard = new KAction(KIcon("fileopen"), i18n("Open board"), actionCollection, "open_board");
    connect(openBoard, SIGNAL(triggered(bool)), SLOT(loadBoard()));
    topToolbar->addAction(openBoard);
    // save game
    KAction* saveBoard = new KAction(KIcon("filesave"), i18n("Save board"), actionCollection, "save_board");
    connect(saveBoard, SIGNAL(triggered(bool)), SLOT(saveBoard()));
    topToolbar->addAction(saveBoard);
    // NOTE dimsuz: how to port this? is it even needed?
    //topToolbar->setButtonIconSet(ID_TOOL_SAVE,loader->loadIconSet("filesave", K3Icon::Toolbar));

    topToolbar->addSeparator();
#ifdef FUTURE_OPTIONS
    // Select
    KAction* select = new KAction(KIcon("rectangle_select"), i18n("Select"), actionCollection, "select");
    topToolbar->addAction(select);

    // NOTE: use kstdactions?
    KAction* cut = new KAction(KIcon("editcut"), i18n("Cut"), actionCollection, "edit_cut");
    topToolbar->addAction(cut);

    KAction* copy = new KAction(KIcon("editcopy"), i18n("Copy"), actionCollection, "edit_copy");
    topToolbar->addAction(copy);

    KAction* paste = new KAction(KIcon("editpaste"), i18n("Paste"), actionCollection, "edit_paste");
    topToolbar->addAction(paste);

    topToolbar->addSeparator();

    KAction* moveTiles = new KAction(KIcon("move"), i18n("Move tiles"), actionCollection, "move_tiles");
    topToolbar->addAction(moveTiles);
#endif
    KToggleAction* addTiles = new KToggleAction(KIcon("pencil"), i18n("Add tiles"), actionCollection, "add_tiles");
    topToolbar->addAction(addTiles);
    KToggleAction* delTiles = new KToggleAction(KIcon("editdelete"), i18n("Remove tiles"), actionCollection, "del_tiles");
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
    KAction* shiftLeft = new KAction( KIcon("back"), i18n("Shift left"), actionCollection, "shift_left" );
    connect(shiftLeft, SIGNAL(triggered(bool)), SLOT(slotShiftLeft()));
    topToolbar->addAction(shiftLeft);

    KAction* shiftUp = new KAction( KIcon("up"), i18n("Shift up"), actionCollection, "shift_up" );
    connect(shiftUp, SIGNAL(triggered(bool)), SLOT(slotShiftUp()));
    topToolbar->addAction(shiftUp);

    KAction* shiftDown = new KAction( KIcon("down"), i18n("Shift down"), actionCollection, "shift_down" );
    connect(shiftDown, SIGNAL(triggered(bool)), SLOT(slotShiftDown()));
    topToolbar->addAction(shiftDown);

    KAction* shiftRight = new KAction( KIcon("forward"), i18n("Shift right"), actionCollection, "shift_right" );
    connect(shiftRight, SIGNAL(triggered(bool)), SLOT(slotShiftRight()));
    topToolbar->addAction(shiftRight);

    topToolbar->addSeparator();
    KAction* quit = KStdAction::quit(this, SLOT(close()), actionCollection, "quit");
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

	if (x >=BoardLayout::width || x <0 || y >=BoardLayout::height || y <0)
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
					i18n("Save Board Layout" ), KStdGuiItem::save());
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
		"like to save the changes?"),QString::null,KStdGuiItem::save(),KStdGuiItem::dontSave());

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
    int sy = (tiles.height()/2)+tiles.levelOffset();
    int sx = (tiles.width()/2);

    for (int y=0; y<=BoardLayout::height; y++) {
	int nextY=sy+(y*tiles.qHeight());
	p.drawLine(sx, nextY,sx+(BoardLayout::width*tiles.qWidth()), nextY);
    }

    for (int x=0; x<=BoardLayout::width; x++) {
	int nextX=sx+(x*tiles.qWidth());
	p.drawLine(nextX, sy, nextX, sy+BoardLayout::height*tiles.qHeight());
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
    for (int z=0; z<BoardLayout::depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < BoardLayout::height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x=BoardLayout::width-1; x>=0; x--) {
                int sx = x*(tiles.qWidth()  )+xOffset;
                int sy = y*(tiles.qHeight()  )+yOffset;
                if (theBoard.getBoardData(z, y, x) != '1') {
                    continue;
                }
		QPixmap *t;
		tile=(z*BoardLayout::depth)+
			(y*BoardLayout::height)+
				(x*BoardLayout::width);
//		if (mode==remove && currPos.x==x && currPos.y==y && currPos.e==z) {
//                   t = tiles.selectedPixmaps(44));
//		} else {
                   t = tiles.unselectedPixmaps(43);
//		}

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border
                if ((x>1) && (y>0) && theBoard.getBoardData(z,y-1,x-2)=='1'){
                    p.drawPixmap( sx+tiles.levelOffset(), sy,
                            *t, tiles.levelOffset() ,0,
                            t->width()-tiles.levelOffset(),
                            t->height()/2);


                    p.drawPixmap( sx, sy+t->height()/2,
                        *t, 0,t->height()/2,t->width(),t->height()/2);
                } else {

                p.drawPixmap(sx, sy,
                    *t, 0,0, t->width(), t->height());
                }


                tile++;
                tile = tile % 143;
            }
        }
        xOffset +=tiles.levelOffset();
        yOffset -=tiles.levelOffset();
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
    for( z=BoardLayout::depth-1; z>=0; z-- )
    {
        // calculate mouse coordiantes --> position in game board
	// the factor -theTiles.width()/2 must keep track with the
	// offset for blitting in the print zvent (FIX ME)
        x = ((point.x()-tiles.width()/2)-(z+1)*tiles.levelOffset())/ tiles.qWidth();
        y = ((point.y()-tiles.height()/2)+ z*tiles.levelOffset()) / tiles.qHeight();


        // skip when position is illegal
        if (x<0 || x>=BoardLayout::width || y<0 || y>=BoardLayout::height)
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
		    if (!theBoard.tileAbove(mPos) && mPos.e < BoardLayout::depth && theBoard.isTileAt(mPos) ) {
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
    int x = (tiles.width()/2)+(p.e*tiles.levelOffset())+(p.x * tiles.qWidth());
    int y = (tiles.height()/2)-(p.e*tiles.levelOffset())+(p.y * tiles.qHeight());
    int w = tiles.width();
    int h = tiles.height();


    if (p.e==100 || !visible)
	x = -1;
    drawFrame->setRect(x,y,w,h, tiles.levelOffset(), mode-remove);
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


    if (p.e >= BoardLayout::depth)
	return (false);
    if (p.y >BoardLayout::height-2)
	return false;
    if (p.x >BoardLayout::width-2)
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
