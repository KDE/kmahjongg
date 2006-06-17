
#include <stdlib.h>
#include <kapplication.h>
#include <qlayout.h>
#include <qpainter.h>

#include "Editor.h"
#include "prefs.h"

#include <kmessagebox.h>
#include <klocale.h>     // Needed to use KLocale
#include <kiconloader.h> //
#include <kstandarddirs.h>
#include <ktoolbarradiogroup.h>

#define ID_TOOL_NEW  100
#define ID_TOOL_LOAD 101
#define ID_TOOL_SAVE 102
#define ID_TOOL_ADD 103
#define ID_TOOL_DEL 104
#define ID_TOOL_MOVE 105
#define ID_TOOL_SELECT 106
#define ID_TOOL_CUT 107
#define ID_TOOL_COPY 108
#define ID_TOOL_PASTE 109
#define ID_TOOL_LEFT 110
#define ID_TOOL_RIGHT 111
#define ID_TOOL_UP 112
#define ID_TOOL_DOWN 113

#define ID_TOOL_STATUS 199

#define ID_META_EXIT 201



// When we assign a tile to draw in a slot we do it in order from te following
// table, wrapping on the tile number. It makes the tile layout look more
// random.


Editor::Editor
(
	QWidget* parent,
	const char* name
)
    :
    QDialog( parent, name, true, 0 ), tiles(false)
{

    clean= true;
    numTiles=0;
    mode = insert;

    int sWidth = (BoardLayout::width+2)*(tiles.qWidth());
    int sHeight =( BoardLayout::height+2)*tiles.qHeight();

    sWidth += 4*tiles.shadowSize();

    drawFrame = new FrameImage( this, "drawFrame" );
    drawFrame->setGeometry( 10, 40 ,sWidth ,sHeight);
    drawFrame->setMinimumSize( 0, 0 );
    drawFrame->setMaximumSize( 32767, 32767 );
    drawFrame->setFocusPolicy( QWidget::NoFocus );
    drawFrame->setBackgroundMode( QWidget::PaletteBackground );
    drawFrame->setFrameStyle( 49 );
    drawFrame->setMouseTracking(true);

   // setup the tool bar
   setupToolbar();

   QVBoxLayout *layout = new QVBoxLayout(this, 1);
   layout->addWidget(topToolbar,0);
   layout->addWidget(drawFrame,1);
   layout->activate();

    resize( sWidth+60, sHeight+60);
    setMinimumSize( sWidth+60, sHeight+60);
    setMaximumSize( sWidth+60, sHeight+60);

   QString tile = Prefs::tileSet();
   tiles.loadTileset(tile);

   // tell the user what we do
   setCaption(kapp->makeStdCaption(i18n("Edit Board Layout")));


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

// ---------------------------------------------------------
void Editor::setupToolbar()
{

    KIconLoader *loader = KGlobal::iconLoader();
    topToolbar = new KToolBar( this, "editToolBar" );
    KToolBarRadioGroup *radio = new KToolBarRadioGroup(topToolbar);

    // new game
    topToolbar->insertButton(loader->loadIcon("filenew", KIcon::Toolbar),
            ID_TOOL_NEW, true, i18n("New board"));
    // open game
    topToolbar->insertButton(loader->loadIcon("fileopen", KIcon::Toolbar),
            ID_TOOL_LOAD, true, i18n("Open board"));
    // save game
    topToolbar->insertButton(loader->loadIcon("filesave", KIcon::Toolbar),
            ID_TOOL_SAVE, true, i18n("Save board"));
    topToolbar->setButtonIconSet(ID_TOOL_SAVE,loader->loadIconSet("filesave", KIcon::Toolbar));
    
#ifdef FUTURE_OPTIONS
    // Select
    topToolbar->insertSeparator();
    topToolbar->insertButton(loader->loadIcon("rectangle_select", KIcon::Toolbar),
            ID_TOOL_SELECT, true, i18n("Select"));
    topToolbar->insertButton(loader->loadIcon("editcut", KIcon::Toolbar),
            ID_TOOL_CUT, true, i18n("Cut"));
    topToolbar->insertButton(loader->loadIcon("editcopy", KIcon::Toolbar),
            ID_TOOL_COPY, true, i18n("Copy"));
    topToolbar->insertButton(loader->loadIcon("editpaste", KIcon::Toolbar),
            ID_TOOL_PASTE, true, i18n("Paste"));

    topToolbar->insertSeparator();
    topToolbar->insertButton(loader->loadIcon("move", KIcon::Toolbar),
            ID_TOOL_MOVE, true, i18n("Move tiles"));
#endif
    topToolbar->insertButton(loader->loadIcon("pencil", KIcon::Toolbar),
            ID_TOOL_ADD, true, i18n("Add tiles"));
    topToolbar->insertButton(loader->loadIcon("editdelete", KIcon::Toolbar),
            ID_TOOL_DEL, true, i18n("Remove tiles"));

    topToolbar->setToggle(ID_TOOL_ADD);
    topToolbar->setToggle(ID_TOOL_MOVE);
    topToolbar->setToggle(ID_TOOL_DEL);
    topToolbar->toggleButton(ID_TOOL_ADD);
    radio->addButton(ID_TOOL_ADD);
#ifdef FUTURE_OPTIONS
    radio->addButton(ID_TOOL_MOVE);
#endif
    radio->addButton(ID_TOOL_DEL);

    // board shift

    topToolbar->insertSeparator();
    topToolbar->insertButton(loader->loadIcon("back", KIcon::Toolbar),
            ID_TOOL_LEFT, true, i18n("Shift left"));
    topToolbar->insertButton(loader->loadIcon("up", KIcon::Toolbar),
            ID_TOOL_UP, true, i18n("Shift up"));
    topToolbar->insertButton(loader->loadIcon("down", KIcon::Toolbar),
            ID_TOOL_DOWN, true, i18n("Shift down"));
    topToolbar->insertButton(loader->loadIcon("forward", KIcon::Toolbar),
            ID_TOOL_RIGHT, true, i18n("Shift right"));

    topToolbar->insertSeparator();
    topToolbar->insertButton(loader->loadIcon("exit", KIcon::Toolbar),
            ID_META_EXIT, true, i18n("Exit"));

    // status in the toolbar for now (ick)

    theLabel = new QLabel(statusText(), topToolbar);
    int lWidth = theLabel->sizeHint().width();

    topToolbar->insertWidget(ID_TOOL_STATUS,lWidth, theLabel );
     topToolbar->alignItemRight( ID_TOOL_STATUS, true );

    //addToolBar(topToolbar);
   connect( topToolbar,  SIGNAL(clicked(int) ), SLOT( topToolbarOption(int) ) );

    topToolbar->updateRects(0);
     topToolbar->setFullSize(true);
    topToolbar->setBarPos(KToolBar::Top);
//    topToolbar->enableMoving(false);
    topToolbar->adjustSize();
    setMinimumWidth(topToolbar->width());


}

void Editor::statusChanged() {
	bool canSave = ((numTiles !=0) && ((numTiles & 1) == 0));
	theLabel->setText(statusText());
 	topToolbar->setItemEnabled( ID_TOOL_SAVE, canSave);
}


void Editor::topToolbarOption(int option) {

    switch(option) {
	case ID_TOOL_NEW:
		newBoard();
		break;
	case ID_TOOL_LOAD:
		loadBoard();
		break;
	case ID_TOOL_SAVE:
		saveBoard();
		break;
	case ID_TOOL_LEFT:
		theBoard.shiftLeft();
		repaint(false);
	        break;
	case ID_TOOL_RIGHT:
		theBoard.shiftRight();
		repaint(false);
	        break;
	case ID_TOOL_UP:
		theBoard.shiftUp();
		repaint(false);
	        break;
	case ID_TOOL_DOWN:
		theBoard.shiftDown();
		repaint(false);
	        break;
	case ID_TOOL_DEL:
			mode=remove;
		break;

	case ID_TOOL_MOVE:
			mode=move;
		break;

	case ID_TOOL_ADD:
			mode = insert;
		break;
	case ID_META_EXIT:
			close();
		break;

	default:

	break;
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

	buf = i18n("Tiles: %1 Pos: %2,%3,%4").arg(numTiles).arg(x).arg(y).arg(z);
	return buf;
}


void Editor::loadBoard() {

    if ( !testSave() )
	return;

    KURL url = KFileDialog::getOpenURL(
				NULL,
				i18n("*.layout|Board Layout (*.layout)\n"
				"*|All Files"),
				this,
				i18n("Open Board Layout" ));

   if ( url.isEmpty() )
        return;


    theBoard.loadBoardLayout( url.path() );

    repaint(false);
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
    repaint(false);
}

bool Editor::saveBoard() {
    // get a save file name
    KURL url = KFileDialog::getSaveURL(
				NULL,
				i18n("*.layout|Board Layout (*.layout)\n"
				"*|All Files"),
				this,
				i18n("Save Board Layout" ));
   if( url.isEmpty() ) return false;
   if( !url.isLocalFile() )
   {
      KMessageBox::sorry( this, i18n( "Only saving to local files currently supported." ) );
      return false;
   }

   if ( url.isEmpty() )
       return false;

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
    buff.resize(dest->width(), dest->height());
    drawBackground(&buff);
    drawTiles(&buff);
    bitBlt(dest, 0,0,&buff, 0,0,buff.width(), buff.height(), CopyROP);

    drawFrame->repaint(false);
}

void Editor::drawBackground(QPixmap *pixmap) {

    QPainter p(pixmap);

    // blast in a white background
    p.fillRect(0,0,pixmap->width(), pixmap->height(), QColor(white));


    // now put in a grid of tile quater width squares
    int sy = (tiles.height()/2)+tiles.shadowSize();
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

    QString tile1 = Prefs::tileSet();
    tiles.loadTileset(tile1);


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

                    bitBlt( dest,
                            sx+tiles.shadowSize(), sy,
                            t, tiles.shadowSize() ,0,
                            t->width()-tiles.shadowSize(),
                            t->height()/2, CopyROP );


                    bitBlt( dest, sx, sy+t->height()/2,
                        t, 0,t->height()/2,t->width(),t->height()/2,CopyROP);
                } else {

                bitBlt( dest, sx, sy,
                    t, 0,0, t->width(), t->height(), CopyROP );
                }


                tile++;
                tile = tile % 143;
            }
        }
        xOffset +=tiles.shadowSize();
        yOffset -=tiles.shadowSize();
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
        x = ((point.x()-tiles.width()/2)-(z+1)*tiles.shadowSize())/ tiles.qWidth();
        y = ((point.y()-tiles.height()/2)+ z*tiles.shadowSize()) / tiles.qHeight();


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
			repaint(false);
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
			repaint(false);
		    }
		  }
		break;
		default:
		break;
	}

}


void Editor::drawCursor(POSITION &p, bool visible)
{
    int x = (tiles.width()/2)+(p.e*tiles.shadowSize())+(p.x * tiles.qWidth());
    int y = (tiles.height()/2)-(p.e*tiles.shadowSize())+(p.y * tiles.qHeight());
    int w = tiles.width();
    int h = tiles.height();


    if (p.e==100 || !visible)
	x = -1;
    drawFrame->setRect(x,y,w,h, tiles.shadowSize(), mode-remove);
    drawFrame->repaint(false);



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
