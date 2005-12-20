#include "boardwidget.h"
#include "prefs.h"

#include <kmessagebox.h>
#include <kapplication.h>
#include <qtimer.h>
#include <qpainter.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <qfile.h>
#include <kconfig.h>

/**
 * Constructor.
 * Loads tileset and background bitmaps.
 */
BoardWidget::BoardWidget( QWidget* parent, const char *name )
  : QWidget( parent, name ), theTiles(false)
{
    setBackgroundColor( QColor( 0,0,0 ) );

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()),
             this, SLOT(helpMoveTimeout()) );

    TimerState = Stop;
    gamePaused = false;
    iTimerStep = 0;
    matchCount = 0;
    showMatch = false;
    showHelp = false;
    MouseClickPos1.e = BoardLayout::depth;     // mark tile position as invalid
    MouseClickPos2.e = BoardLayout::depth;
    memset( &Game.Mask, 0, sizeof( Game.Mask ) );
    Game.MaxTileNum = 0;
    gameGenerationNum = 0;

    // initially we force a redraw
    updateBackBuffer=true;

    // Load tileset. First try to load the last use tileset
    QString tFile;
    getFileOrDefault(Prefs::tileSet(), "tileset", tFile);

    if (!loadTileset(tFile)){
        KMessageBox::error(this,
                           i18n("An error occurred when loading the tileset file %1\n"
                                "KMahjongg will now terminate.").arg(tFile));
        kapp->quit();
    }

    getFileOrDefault(Prefs::background(), "bgnd", tFile);

    // Load background
    if( ! loadBackground(tFile, false ) )
    {
	KMessageBox::error(this,
		   i18n("An error occurred when loading the background image\n%1").arg(tFile)+
		   i18n("KMahjongg will now terminate."));
	kapp->quit();
    }

    getFileOrDefault(Prefs::layout(), "layout", tFile);
    if( ! loadBoardLayout(tFile) )
    {
	KMessageBox::error(this,
		   i18n("An error occurred when loading the board layout %1\n"
                "KMahjongg will now terminate.").arg(tFile));
	kapp->quit();
    }
    setDisplayedWidth();
    loadSettings();
}

BoardWidget::~BoardWidget(){
  saveSettings();
}

void BoardWidget::loadSettings(){
  theBackground.tile = Prefs::tiledBackground();

  setDisplayedWidth();
  tileSizeChanged();
  updateScaleMode();
  drawBoard(true);
}

void BoardWidget::saveSettings(){
  // Preview can't handle this.  TODO
  //KConfig *config=kapp->config();
  //config->setGroup("General");

  //config->writePathEntry("Tileset_file", tileFile);
  //config->writePathEntry("Background_file", backgroundFile);
  //config->writePathEntry("Layout_file", layout);
}

void BoardWidget::getFileOrDefault(QString filename, QString type, QString &res)
{
	QString picsPos = "pics/";
	picsPos += "default.";
	picsPos += type;

	if (QFile::exists(filename)) {
		res = filename;
	}
    else {
        res = locate("appdata", picsPos);
	}

    if (res.isEmpty()) {
		KMessageBox::error(this, i18n("KMahjongg could not locate the file: %1\n"
                                      "or the default file of type: %2\n"
                                      "KMahjongg will now terminate").arg(filename).arg(type) );
		kapp->quit();
	}
}

void BoardWidget::setDisplayedWidth() {
  if (Prefs::showRemoved())
    setFixedSize( requiredWidth() , requiredHeight());
  else
    setFixedSize( requiredWidth() - ((theTiles.width())*4)
		, requiredHeight());
}

// for a given cell x y calc how that cell is shadowed
// returnd left = width of left hand side shadow
// t = height of top shadow
// c = width and height of corner shadow

void BoardWidget::calcShadow(int e, int y, int x, int &l, int &t, int &c) {

	l = t = c = 0;
	if ((Game.shadowHeight(e,y,x) != 0) ||
	   (Game.shadowHeight(e,y-1,x) != 0) ||
	   (Game.shadowHeight(e,y,x-1) != 0)) {
		return;
	}
	int a,b;

	a=Game.shadowHeight(e,y,x-2);
	b=Game.shadowHeight(e,y-1,x-2);
	if (a != 0 || b != 0)
	   l = (a>b) ? a : b;
	a=Game.shadowHeight(e,y-2,x);
	b=Game.shadowHeight(e,y-2,x-1);
	if (a != 0 || b != 0)
	   t = (a>b) ? a : b;

	c = Game.shadowHeight(e, y-2, x-2);
}

// draw a triangular shadow from the top right to the bottom left.
// one such shadow is a right hand edge of a shadow line.
// if a second shadow botton left to top right is rendered over it
// then the shadow becomes a box (ie in the middle of the run)

void BoardWidget::shadowTopLeft(int depth, int sx, int sy, int rx, int ry, QPixmap *src, bool flag) {
	if (depth) {
		int shadowPixels= (depth+1) * theTiles.shadowSize();
		int xOffset=theTiles.qWidth()-shadowPixels;
	    	for (int p=0; p<shadowPixels; p++) {
             	    bitBlt( &backBuffer,
			    sx+xOffset,	sy+p,
                    	    src,
			    rx+xOffset,	ry+p,
			    shadowPixels-p,
			    1, CopyROP );
		}
		// Now aafter rendering the triangle, fill in the rest of
		// the quater width
		if (flag && ((theTiles.qWidth() - shadowPixels) > 0))
             	    bitBlt( &backBuffer,
			    sx,	sy,
                    	    src,
			    rx,	ry,
			    theTiles.qWidth() - shadowPixels,
			    shadowPixels, CopyROP );
	}
}

// Second triangular shadow generator see above
void BoardWidget::shadowBotRight(int depth, int sx, int sy, int rx, int ry, QPixmap *src, bool flag) {
	if (depth) {
		int shadowPixels= (depth+1) * theTiles.shadowSize();
		int xOffset=theTiles.qWidth();
	    	for (int p=0; p<shadowPixels; p++) {
             	    bitBlt( &backBuffer,
			    sx+xOffset-p, 	/* step to shadow right start */
			    sy+p,		/* down for each line */
                    	    src,
			    rx+xOffset-p,	/* step to shadow right start */
			    ry+p,
			    p, 			/* increace width each line down */
			    1, CopyROP );
		}
		if (flag && ((theTiles.qHeight() - shadowPixels) >0))
             	    bitBlt( &backBuffer,
			    sx+xOffset-shadowPixels,
			    sy+shadowPixels,
                    	    src,
			    rx+xOffset-shadowPixels,
			    ry+shadowPixels,
			    shadowPixels,
			    theTiles.qHeight()-shadowPixels, CopyROP );

	}
}




void BoardWidget::shadowArea(int z, int y, int x, int sx, int sy,int rx, int ry, QPixmap *src)
{
	// quick check to see if we are obscured
	if (z < BoardLayout::depth-1) 	{
	    if ((x >= 0) && (y<BoardLayout::height)) {
		if (Game.Mask[z+1][y][x] && Game.Board[z+1][y][x]) {
			return;
		}
	    }
	}




	// offset to pass tile depth indicator
	sx+=theTiles.shadowSize();
	rx+=theTiles.shadowSize();



	// We shadow the top right hand edge of the tile with a
	// triangular border. If the top shadow covers it all
	// well and good, otherwise if its smaller, part of the
	// triangle will show through.

	shadowTopLeft(Game.shadowHeight(z+1, y-1, x), sx, sy, rx,ry,src, true);
	shadowBotRight(Game.shadowHeight(z+1, y, x+1), sx, sy, rx, ry, src, true);
	shadowTopLeft(Game.shadowHeight(z+1, y-1, x+1), sx, sy, rx,ry,src, false);
	shadowBotRight(Game.shadowHeight(z+1, y-1, x+1), sx, sy, rx, ry, src, false);

	return;

}

// ---------------------------------------------------------
void BoardWidget::paintEvent( QPaintEvent* pa )
{
    QPixmap  *back;

    int xx = pa->rect().left();
    int xheight = pa->rect().height();
    int xwidth  = pa->rect().width();

    back = theBackground.getBackground();

    if (gamePaused) {
        // If the game is paused, then blank out the board.
        // We tolerate no cheats around here folks..
        bitBlt( this, xx, pa->rect().top(),
                back, xx, pa->rect().top(), xwidth, xheight, CopyROP );
	return;
    }

    // if the repaint is because of a window redraw after a move
    // or a menu roll up, then just blit in the last rendered image
    if (!updateBackBuffer) {
    	bitBlt(this, xx,pa->rect().top(),
		&backBuffer, xx, pa->rect().top(), xwidth, xheight, CopyROP);
	return;
    }

    // update the complete drawArea

    backBuffer.resize(back->width(), back->height());

    // erase out with the background
    bitBlt( &backBuffer, xx, pa->rect().top(),
                back, xx,pa->rect().top(), back->width(), back->height(), CopyROP );

    // initial offset on the screen of tile 0,0
    int xOffset = theTiles.width()/2;
    int yOffset = theTiles.height()/2;
    //short tile = 0;

    // shadow the background first
    if (Prefs::showShadows()) {
        for (int by=0; by <BoardLayout::height+1; by++)
	    for (int bx=-1; bx < BoardLayout::width+1; bx++)
 	        shadowArea(-1, by, bx,
			bx*theTiles.qWidth()+xOffset-theTiles.shadowSize(),
			by*theTiles.qHeight()+yOffset+theTiles.shadowSize(),
			bx*theTiles.qWidth()+xOffset-theTiles.shadowSize(),
			by*theTiles.qHeight()+yOffset+theTiles.shadowSize(),
			theBackground.getShadowBackground());
    }




    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z=0; z<BoardLayout::depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < BoardLayout::height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x=BoardLayout::width-1; x>=0; x--) {
                int sx = x*(theTiles.qWidth()  )+xOffset;
                int sy = y*(theTiles.qHeight()  )+yOffset;



		// skip if no tile to display
		if (!Game.tilePresent(z,y,x))
			continue;

                QPixmap *t;
		QPixmap *s;
		if (Game.hilighted[z][y][x]) {
		   t= theTiles.selectedPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		   s= theTiles.selectedShadowPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		} else {
		   t= theTiles.unselectedPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		   s= theTiles.unselectedShadowPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
                }

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border

                if (x > 1 && y > 0 && Game.tilePresent(z, y-1, x-2)){
                    bitBlt( &backBuffer,
			    sx+theTiles.shadowSize(), sy,
                            t, theTiles.shadowSize() ,0,
		            t->width()-theTiles.shadowSize(),
		  	    t->height()/2, CopyROP );
                    bitBlt( &backBuffer, sx, sy+t->height()/2,
                        t, 0,t->height()/2,t->width(),t->height()/2,CopyROP);
                } else {

                bitBlt( &backBuffer, sx, sy,
                    t, 0,0, t->width(), t->height(), CopyROP );
                }


		if (Prefs::showShadows() && z<BoardLayout::depth - 1) {
		    for (int xp = 0; xp <= 1; xp++)  {
			for (int yp=0; yp <= 1; yp++) {
				shadowArea(z, y+yp, x+xp,
					sx+(xp*theTiles.qWidth()),
					sy+(yp*theTiles.qHeight()),
					xp*theTiles.qWidth(),
					yp*theTiles.qHeight(),
					s);
			}
		    }

		}



            }
        }
        xOffset +=theTiles.shadowSize();
        yOffset -=theTiles.shadowSize();
    }


    // Now we add the list of cancelled tiles

    // we start blitting as usuall right to left, top to bottom, first
    // we calculate the start pos of the first tile, allowing space for
    // the upwards at rightwards creep when stacking in 3d
    unsigned short xPos = backBuffer.width()-(3*theTiles.shadowSize())-theTiles.width();
    unsigned short yPos = (3*theTiles.shadowSize());

    for (int pos=0; pos < 9; pos++) {
	int last = 0;
	int tile=0;
	// dragon?
	if (pos >= 0 && pos < 3) {
		last = removedDragon[pos];
		tile = TILE_DRAGON+pos;
	} else {
	    //Wind?
	    if (pos >= 3 && pos < 7) {
	        last = removedWind[pos-3];
		tile = TILE_WIND+pos-3;
	    } else {
		if (pos == 7) {
		    for (int t=0; t<4;t++) {
		        if (removedFlower[t]) {
			    last++;
			    tile=TILE_FLOWER+t;
			}
		    }
		} else {
		    for (int t=0; t<4;t++) {
		        if (removedSeason[t]) {
			    last++;
			    tile=TILE_SEASON+t;
			}
		    }
		}
	    }
	}

	    stackTiles(tile, last, xPos, yPos);
	    stackTiles(TILE_ROD+pos, removedRod[pos],
		xPos - (1*(theTiles.width() - theTiles.shadowSize())) , yPos);
	    stackTiles(TILE_BAMBOO+pos, removedBamboo[pos],
		xPos - (2*(theTiles.width() - theTiles.shadowSize())) , yPos);
	    stackTiles(TILE_CHARACTER+pos, removedCharacter[pos],
		xPos - (3*(theTiles.width() - theTiles.shadowSize())) , yPos);



	yPos += theTiles.height()-theTiles.shadowSize();
    }

    updateBackBuffer=false;
    bitBlt(this, xx,pa->rect().top(), &backBuffer, xx, pa->rect().top(), xwidth, xheight, CopyROP);


}

void BoardWidget::stackTiles(unsigned char t, unsigned short h, unsigned short x,unsigned  short y)
{

    int ss = theTiles.shadowSize();
    QPainter p(&backBuffer);
    QPen line;
    p.setBackgroundMode(OpaqueMode);
    p.setBackgroundColor(black);




    line.setWidth(1);
    line.setColor(white);
    p.setPen(line);
    int x2 = x+theTiles.width()-ss-1;
    int y2 = y+theTiles.height()-1;
    p.drawLine(x, y+ss, x2, y+ss);
    p.drawLine(x, y+ss, x, y2);
    p.drawLine(x2, y+ss, x2, y2);
    p.drawLine(x+1, y2, x2, y2);

   // p.fillRect(x+1, y+ss+1, theTiles.width()-ss-2, theTiles.height()-ss-2, QBrush(lightGray));

    for (unsigned short pos=0; pos < h; pos++) {
       QPixmap *p = theTiles.unselectedPixmaps(t-TILE_OFFSET);
       bitBlt( &backBuffer, x+(pos*ss), y-(pos*ss),
                    p, 0,0, p->width(), p->height(), CopyROP );
    }

}


void BoardWidget::pause() {
	gamePaused = !gamePaused;
	drawBoard(true);
}

void BoardWidget::gameLoaded()
{
	int i;
	initialiseRemovedTiles();
	i = Game.TileNum;
	// use the history of moves to put in the removed tiles area the correct tiles
	while (i < Game.MaxTileNum )
	{
		setRemovedTilePair(Game.MoveList[i], Game.MoveList[i+1]);
		i +=2;
	}
	drawBoard();
}

// ---------------------------------------------------------
int BoardWidget::undoMove()
{
    cancelUserSelectedTiles();

    if( Game.TileNum < Game.MaxTileNum )
    {

        clearRemovedTilePair(Game.MoveList[Game.TileNum], Game.MoveList[Game.TileNum+1]);
        putTile( Game.MoveList[Game.TileNum], false );
        Game.TileNum++;
        putTile( Game.MoveList[Game.TileNum] );
        Game.TileNum++;
        drawTileNumber();
        setStatusText( i18n("Undo operation done successfully.") );
	return 1;
    }
    else {
        setStatusText(i18n("What do you want to undo? You have done nothing!"));
	return 0;
	}
}

// ---------------------------------------------------------
void BoardWidget::helpMove()
{
    cancelUserSelectedTiles();
    if (showHelp) helpMoveStop();

    if( findMove( TimerPos1, TimerPos2 ) )
    {
        cheatsUsed++;
        iTimerStep = 1;
        showHelp = true;
        helpMoveTimeout();
    }
    else
        setStatusText( i18n("Sorry, you have lost the game.") );
}
// ---------------------------------------------------------
void BoardWidget::helpMoveTimeout()
{
    if( iTimerStep & 1 )
    {
        hilightTile( TimerPos1, true, false );
        hilightTile( TimerPos2, true );
    }
    else
    {
        hilightTile( TimerPos1, false, false );
        hilightTile( TimerPos2, false );
    }
    // restart timer
    if( iTimerStep++ < 8 )
        timer->start( ANIMSPEED , true );
    else
        showHelp = false;
}
// ---------------------------------------------------------

void BoardWidget::helpMoveStop()
{
    timer->stop();
    iTimerStep = 8;
    hilightTile( TimerPos1, false, false );
    hilightTile( TimerPos2, false );
    showHelp = false;
}

// ---------------------------------------------------------
void BoardWidget::startDemoMode()
{
    calculateNewGame();

    if( TimerState == Stop )
    {
        TimerState = Demo;
        iTimerStep = 0;
        emit demoModeChanged( true );
        setStatusText( i18n("Demo mode. Click mousebutton to stop.") );
        demoMoveTimeout();
    }
}
// ---------------------------------------------------------
void BoardWidget::stopDemoMode()
{
    TimerState = Stop;    // stop demo
    calculateNewGame();
    setStatusText( i18n("Now it's you again.") );
    emit demoModeChanged( false );
    emit gameCalculated();
}
// ---------------------------------------------------------
void BoardWidget::demoMoveTimeout()
{
    if( TimerState == Demo )
    {
        switch( iTimerStep++ % 6 )
        {
            // at firts, find new matching tiles
            case 0:
                if( ! findMove( TimerPos1, TimerPos2 ) )
	        {
                    // if computer has won
	            if( Game.TileNum == 0 )
                    {
                        animateMoveList();
                    }
                    // else computer has lost
                    else
                    {
                        setStatusText( i18n("Your computer has lost the game.") );
                        while( Game.TileNum < Game.MaxTileNum )
                        {
                            putTile( Game.MoveList[Game.TileNum], false );
                            Game.TileNum++;
                            putTile( Game.MoveList[Game.TileNum] );
                            Game.TileNum++;
                            drawTileNumber();
                        }
                    }
                    TimerState = Stop;
                    startDemoMode();
                    return;
                }
                break;
	    // hilight matching tiles two times
            case 1:
            case 3:
                hilightTile( TimerPos1, true, false );
                hilightTile( TimerPos2, true );
            break;

            case 2:
            case 4:
                hilightTile( TimerPos1, false, false );
                hilightTile( TimerPos2, false );
                break;
	    // remove matching tiles from game board
            case 5:
                setRemovedTilePair(TimerPos1, TimerPos2);
                removeTile( TimerPos1, false );
                removeTile( TimerPos2 );
                drawTileNumber();
                break;
        }
        // restart timer
        QTimer::singleShot( ANIMSPEED, this, SLOT( demoMoveTimeout() ) );
    }
}

// ---------------------------------------------------------
void BoardWidget::setShowMatch( bool show )
{
    if( showMatch )
        stopMatchAnimation();
    showMatch = show;
}
// ---------------------------------------------------------
void BoardWidget::matchAnimationTimeout()
{
    if (matchCount == 0)
        return;

    if( iTimerStep++ & 1 )
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {


            hilightTile(PosTable[Pos], true);
        }
    }
    else
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {
            hilightTile(PosTable[Pos], false);
        }
    }
    if( TimerState == Match )
        QTimer::singleShot( ANIMSPEED, this, SLOT( matchAnimationTimeout() ) );
}
// ---------------------------------------------------------
void BoardWidget::stopMatchAnimation()
{
    for(short Pos = 0; Pos < matchCount; Pos++)
    {
        hilightTile(PosTable[Pos], false);
    }
    TimerState = Stop;
    matchCount = 0;
}

void BoardWidget::redoMove()
{

	setRemovedTilePair(Game.MoveList[Game.TileNum-1],Game.MoveList[Game.TileNum-2]);
        removeTile(Game.MoveList[Game.TileNum-1], false);
        removeTile(Game.MoveList[Game.TileNum-1]);
        drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::animateMoveList()
{
    setStatusText( i18n("Congratulations. You have won!") );

    if (Prefs::playAnimation())
    {
        while( Game.TileNum < Game.MaxTileNum )
        {
            // put back all tiles
            putTile(Game.MoveList[Game.TileNum]);
            Game.TileNum++;
            putTile(Game.MoveList[Game.TileNum], false);
            Game.TileNum++;
            drawTileNumber();
        }
        while( Game.TileNum > 0 )
        {
            // remove all tiles
            removeTile(Game.MoveList[Game.TileNum-1], false);
            removeTile(Game.MoveList[Game.TileNum-1]);
            drawTileNumber();
        }
    }

    calculateNewGame();
}

// ---------------------------------------------------------
void BoardWidget::calculateNewGame( int gNumber)
{
    cancelUserSelectedTiles();
    stopMatchAnimation();
    initialiseRemovedTiles();
    setStatusText( i18n("Calculating new game...") );


    if( !loadBoard())
    {
        setStatusText( i18n("Error converting board information!") );
        return;
    }

    if (gNumber == -1) {
    	gameGenerationNum = kapp->random();
    } else {
	gameGenerationNum = gNumber;
    }

    random.setSeed(gameGenerationNum);

    // Translate Game.Map to an array of POSITION data.  We only need to
    // do this once for each new game.
    memset(tilePositions, 0, sizeof(tilePositions));
    generateTilePositions();

    // Now use the tile position data to generate tile dependency data.
    // We only need to do this once for each new game.
    generatePositionDepends();

    // Now try to position tiles on the board, 64 tries max.
    for( short nr=0; nr<64; nr++ )
    {
        if( generateStartPosition2() )
        {
            drawBoard();
            setStatusText( i18n("Ready. Now it is your turn.") );
			cheatsUsed=0;
	    return;
        }
    }

    drawBoard();
    setStatusText( i18n("Error generating new game!") );
}

// ---------------------------------------------------------
// Generate the position data for the layout from contents of Game.Map.
void BoardWidget::generateTilePositions() {

    numTiles = 0;

    for (int z=0; z< BoardLayout::depth; z++) {
        for (int y=0; y<BoardLayout::height; y++) {
            for (int x=0; x<BoardLayout::width; x++) {
                Game.Board[z][y][x] = 0;
                if (Game.Mask[z][y][x] == '1') {
                    tilePositions[numTiles].x = x;
                    tilePositions[numTiles].y = y;
                    tilePositions[numTiles].e = z;
                    tilePositions[numTiles].f = 254;
                    numTiles++;
                }
            }
        }
    }
}

// ---------------------------------------------------------
// Generate the dependency data for the layout from the position data.
// Note that the coordinates of each tile in tilePositions are those of
// the upper left quarter of the tile.
void BoardWidget::generatePositionDepends() {

    // For each tile,
    for (int i = 0; i < numTiles; i++) {

        // Get its basic position data
        int x = tilePositions[i].x;
        int y = tilePositions[i].y;
        int z = tilePositions[i].e;

        // LHS dependencies
        positionDepends[i].lhs_dep[0] = tileAt(x-1, y,   z);
        positionDepends[i].lhs_dep[1] = tileAt(x-1, y+1, z);

        // Make them unique
        if (positionDepends[i].lhs_dep[1] == positionDepends[i].lhs_dep[0]) {
            positionDepends[i].lhs_dep[1] = -1;
        }

        // RHS dependencies
        positionDepends[i].rhs_dep[0] = tileAt(x+2, y,   z);
        positionDepends[i].rhs_dep[1] = tileAt(x+2, y+1, z);

        // Make them unique
        if (positionDepends[i].rhs_dep[1] == positionDepends[i].rhs_dep[0]) {
            positionDepends[i].rhs_dep[1] = -1;
        }

        // Turn dependencies
        positionDepends[i].turn_dep[0] = tileAt(x,   y,   z+1);
        positionDepends[i].turn_dep[1] = tileAt(x+1, y,   z+1);
        positionDepends[i].turn_dep[2] = tileAt(x+1, y+1, z+1);
        positionDepends[i].turn_dep[3] = tileAt(x,   y+1, z+1);

        // Make them unique
        for (int j = 0; j < 3; j++) {
            for (int k = j+1; k < 4; k++) {
                if (positionDepends[i].turn_dep[j] ==
                    positionDepends[i].turn_dep[k]) {
                    positionDepends[i].turn_dep[k] = -1;
                }
            }
        }

        // Placement dependencies
        positionDepends[i].place_dep[0] = tileAt(x,   y,   z-1);
        positionDepends[i].place_dep[1] = tileAt(x+1, y,   z-1);
        positionDepends[i].place_dep[2] = tileAt(x+1, y+1, z-1);
        positionDepends[i].place_dep[3] = tileAt(x,   y+1, z-1);

        // Make them unique
        for (int j = 0; j < 3; j++) {
            for (int k = j+1; k < 4; k++) {
                if (positionDepends[i].place_dep[j] ==
                    positionDepends[i].place_dep[k]) {
                    positionDepends[i].place_dep[k] = -1;
                }
            }
        }

        // Filled and free indicators.
        positionDepends[i].filled = false;
        positionDepends[i].free   = false;
    }
}

// ---------------------------------------------------------
// x, y, z are the coordinates of a *quarter* tile.  This returns the
// index (in positions) of the tile at those coordinates or -1 if there
// is no tile at those coordinates.  Note that the coordinates of each
// tile in positions are those of the upper left quarter of the tile.
int BoardWidget::tileAt(int x, int y, int z) {

    for (int i = 0; i < numTiles; i++) {
        if (tilePositions[i].e == z) {
            if ((tilePositions[i].x == x   && tilePositions[i].y == y) ||
                (tilePositions[i].x == x-1 && tilePositions[i].y == y) ||
                (tilePositions[i].x == x-1 && tilePositions[i].y == y-1) ||
                (tilePositions[i].x == x   && tilePositions[i].y == y-1)) {

                return i;
            }
        }
    }
    return -1;
}

// ---------------------------------------------------------
bool BoardWidget::generateSolvableGame() {

    // Initially we want to mark positions on layer 0 so that we have only
    // one free position per apparent horizontal line.
    for (int i = 0; i < numTiles; i++) {

        // Pick a random tile on layer 0
        int position, cnt = 0;
        do {
            position = (int) random.getLong(numTiles);
            if (cnt++ > (numTiles*numTiles)) {
                return false; // bail
            }
        } while (tilePositions[position].e != 0);

        // If there are no other free positions on the same apparent
        // horizontal line, we can mark that position as free.
        if (onlyFreeInLine(position)) {
            positionDepends[position].free = true;
        }
    }

    // Check to make sure we really got them all.  Very important for
    // this algorithm.
    for (int i = 0; i < numTiles; i++) {
        if (tilePositions[i].e == 0 && onlyFreeInLine(i)) {
            positionDepends[i].free = true;
        }
    }

    // Get ready to place the tiles
    int lastPosition = -1;
    int position = -1;
    int position2 = -1;

    // For each position,
    for (int i = 0; i < numTiles; i++) {

        // If this is the first tile in a 144 tile set,
        if ((i % 144) == 0) {

            // Initialise the faces to allocate. For the classic
            // dragon board there are 144 tiles. So we allocate and
            // randomise the assignment of 144 tiles. If there are > 144
            // tiles we will reallocate and re-randomise as we run out.
            // One advantage of this method is that the pairs to assign are
            // non-linear. In kmahjongg 0.4, If there were > 144 the same
            // allocation series was followed. So 154 = 144 + 10 rods.
            // 184 = 144 + 40 rods (20 pairs) which overwhemed the board
            // with rods and made deadlock games more likely.
            randomiseFaces();
        }

        // If this is the first half of a pair, there is no previous
        // position for the pair.
        if ((i & 1) == 0) {
            lastPosition = -1;
        }

        // Select a position for the tile, relative to the position of
        // the last tile placed.
        if ((position = selectPosition(lastPosition)) < 0) {
            return false; // bail
        }
        if (i < numTiles-1) {
            if ((position2 = selectPosition(lastPosition)) < 0) {
                return false; // bail
            }
            if (tilePositions[position2].e > tilePositions[position].e) {
                position = position2;  // higher is better
            }
        }

        // Place the tile.
        placeTile(position, tilePair[i % 144]);

        // Remember the position
        lastPosition = position;
    }

    // The game is solvable.
    return true;
}

// ---------------------------------------------------------
// Determines whether it is ok to mark this position as "free" because
// there are no other positions marked "free" in its apparent horizontal
// line.
bool BoardWidget::onlyFreeInLine(int position) {

    int i, i0, w;
    int lin, rin, out;
    static int nextLeft[BoardLayout::maxTiles];
    static int nextRight[BoardLayout::maxTiles];

    /* Check left, starting at position */
    lin = 0;
    out = 0;
    nextLeft[lin++] = position;
    do {
        w = nextLeft[out++];
        if (positionDepends[w].free || positionDepends[w].filled) {
            return false;
        }
        if ((i = positionDepends[w].lhs_dep[0]) != -1) {
            nextLeft[lin++] = i;
        }
        i0 = i;
        if ((i = positionDepends[w].lhs_dep[1]) != -1 && i0 != i) {
            nextLeft[lin++] = i;
        }
    }
    while (lin > out) ;

    /* Check right, starting at position */
    rin = 0;
    out = 0;
    nextRight[rin++] = position;
    do {
        w = nextRight[out++];
        if (positionDepends[w].free || positionDepends[w].filled) {
            return false;
        }
        if ((i = positionDepends[w].rhs_dep[0]) != -1) {
            nextRight[rin++] = i;
        }
        i0 = i;
        if ((i = positionDepends[w].rhs_dep[1]) != -1 && i0 != i) {
            nextRight[rin++] = i;
        }
    }
    while (rin > out) ;

    // Here, the position can be marked "free"
    return true;
}

// ---------------------------------------------------------
int BoardWidget::selectPosition(int lastPosition) {

    int position, cnt = 0;
    bool goodPosition = false;

    // while a good position has not been found,
    while (!goodPosition) {

        // Select a random, but free, position.
        do {
              position = random.getLong(numTiles);
            if (cnt++ > (numTiles*numTiles)) {
                return -1; // bail
            }
        } while (!positionDepends[position].free);

        // Found one.
        goodPosition = true;

        // If there is a previous position to take into account,
        if (lastPosition != -1) {

            // Check the new position against the last one.
            for (int i = 0; i < 4; i++) {
                if (positionDepends[position].place_dep[i] == lastPosition) {
                    goodPosition = false;  // not such a good position
                }
            }
            for (int i = 0; i < 2; i++) {
                if ((positionDepends[position].lhs_dep[i] == lastPosition) ||
                    (positionDepends[position].rhs_dep[i] == lastPosition)) {
                    goodPosition = false;  // not such a good position
                }
            }
        }
    }

    return position;
}

// ---------------------------------------------------------
void BoardWidget::placeTile(int position, int tile) {

    // Install the tile in the specified position
    tilePositions[position].f = tile;
    Game.putTile(tilePositions[position]);

    // Update position dependency data
    positionDepends[position].filled = true;
    positionDepends[position].free = false;

    // Now examine the tiles near this to see if this makes them "free".
    int depend;
    for (int i = 0; i < 4; i++) {
        if ((depend = positionDepends[position].turn_dep[i]) != -1) {
            updateDepend(depend);
        }
    }
    for (int i = 0; i < 2; i++) {
        if ((depend = positionDepends[position].lhs_dep[i]) != -1) {
            updateDepend(depend);
        }
        if ((depend = positionDepends[position].rhs_dep[i]) != -1) {
            updateDepend(depend);
        }
    }
}

// ---------------------------------------------------------
// Updates the free indicator in the dependency data for a position
// based on whether the positions on which it depends are filled.
void BoardWidget::updateDepend(int position) {

    // If the position is valid and not filled
    if (position >= 0 && !positionDepends[position].filled) {

        // Check placement depends.  If they are not filled, the
        // position cannot become free.
        int depend;
        for (int i = 0; i < 4; i++) {
            if ((depend = positionDepends[position].place_dep[i]) != -1) {
                if (!positionDepends[depend].filled) {
                    return ;
                }
            }
        }

        // If position is first free on apparent horizontal, it is
        // now free to be filled.
          if (onlyFreeInLine(position)) {
              positionDepends[position].free = true;
            return;
        }

        // Assume no LHS positions to fill
        bool lfilled = false;

          // If positions to LHS
        if ((positionDepends[position].lhs_dep[0] != -1) ||
            (positionDepends[position].lhs_dep[1] != -1)) {

            // Assume LHS positions filled
            lfilled = true;

            for (int i = 0; i < 2; i++) {
                if ((depend = positionDepends[position].lhs_dep[i]) != -1) {
                    if (!positionDepends[depend].filled) {
                         lfilled = false;
                    }
                }
            }
        }

        // Assume no RHS positions to fill
        bool rfilled = false;

          // If positions to RHS
        if ((positionDepends[position].rhs_dep[0] != -1) ||
            (positionDepends[position].rhs_dep[1] != -1)) {

            // Assume LHS positions filled
            rfilled = true;

            for (int i = 0; i < 2; i++) {
                if ((depend = positionDepends[position].rhs_dep[i]) != -1) {
                    if (!positionDepends[depend].filled) {
                        rfilled = false;
                    }
                }
            }
        }

          // If positions to left or right are filled, this position
        // is now free to be filled.
          positionDepends[position].free = (lfilled || rfilled);
    }
}

// ---------------------------------------------------------
bool BoardWidget::generateStartPosition2() {

	// For each tile,
	for (int i = 0; i < numTiles; i++) {

		// Get its basic position data
		int x = tilePositions[i].x;
		int y = tilePositions[i].y;
		int z = tilePositions[i].e;

		// Clear Game.Board at that position
		Game.Board[z][y][x] = 0;

		// Clear tile placed/free indicator(s).
		positionDepends[i].filled = false;
		positionDepends[i].free   = false;

		// Set tile face blank
		tilePositions[i].f = 254;
	}

	// If solvable games should be generated,
	if (Prefs::solvableGames()) {

		if (generateSolvableGame()) {
    		Game.TileNum = Game.MaxTileNum;
			return true;
		} else {
			return false;
		}
	}

	// Initialise the faces to allocate. For the classic
	// dragon board there are 144 tiles. So we allocate and
	// randomise the assignment of 144 tiles. If there are > 144
	// tiles we will reallocate and re-randomise as we run out.
	// One advantage of this method is that the pairs to assign are
	// non-linear. In kmahjongg 0.4, If there were > 144 the same
	// allocation series was followed. So 154 = 144 + 10 rods.
	// 184 = 144 + 40 rods (20 pairs) which overwhemed the board
	// with rods and made deadlock games more likely.

	int remaining = numTiles;
	randomiseFaces();

	for (int tile=0; tile <numTiles; tile+=2) {
		int p1;
		int p2;

		if (remaining > 2) {
			p2 = p1 = random.getLong(remaining-2);
			int bail = 0;
			while (p1 == p2) {
				p2 = random.getLong(remaining-2);

				if (bail >= 100) {
					if (p1 != p2) {
						break;
					}
				}
				if ((tilePositions[p1].y == tilePositions[p2].y) &&
				    (tilePositions[p1].e == tilePositions[p2].e)) {
					// skip if on same y line
					bail++;
					p2=p1;
					continue;
				}
			}
		} else {
			p1 = 0;
			p2 = 1;
		}
		POSITION a, b;
		a = tilePositions[p1];
		b = tilePositions[p2];
		tilePositions[p1] = tilePositions[remaining - 1];
		tilePositions[p2] = tilePositions[remaining - 2];
		remaining -= 2;

		getFaces(a, b);
		Game.putTile(a);
		Game.putTile(b);
	}

    Game.TileNum = Game.MaxTileNum;
	return 1;
}

void BoardWidget::getFaces(POSITION &a, POSITION &b) {
	a.f = tilePair[tilesUsed];
	b.f = tilePair[tilesUsed+1];
	tilesUsed += 2;

	if (tilesUsed >= 144) {
		randomiseFaces();
	}
}

void BoardWidget::randomiseFaces() {
	int nr;
	int numAlloced=0;
	// stick in 144 tiles in pairsa.

        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_CHARACTER+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_BAMBOO+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_ROD+(nr/4); // 4*9 Tiles
        for( nr=0; nr<4;   nr++)
		tilePair[numAlloced++] = TILE_FLOWER+nr;         // 4 Tiles
        for( nr=0; nr<4;   nr++)
		tilePair[numAlloced++] = TILE_SEASON+nr;         // 4 Tiles
        for( nr=0; nr<4*4; nr++)
		tilePair[numAlloced++] = TILE_WIND+(nr/4);  // 4*4 Tiles
        for( nr=0; nr<3*4; nr++)
		tilePair[numAlloced++] = TILE_DRAGON+(nr/4);     // 3*4 Tiles


	//randomise. Keep pairs together. Ie take two random
	//odd numbers (n,x) and swap n, n+1 with x, x+1

	int at=0;
	int to=0;
	for (int r=0; r<200; r++) {


		to=at;
		while (to==at) {
			to = random.getLong(144);

			if ((to & 1) != 0)
				to--;

		}
		UCHAR tmp = tilePair[at];
		tilePair[at] = tilePair[to];
		tilePair[to] = tmp;
		tmp = tilePair[at+1];
		tilePair[at+1] = tilePair[to+1];
		tilePair[to+1] = tmp;


		at+=2;
		if (at >= 144)
			at =0;
	}

	tilesAllocated = numAlloced;
	tilesUsed = 0;
}


// ---------------------------------------------------------
bool isFlower( UCHAR Tile )
{
    return( Tile >= TILE_FLOWER  &&  Tile <=TILE_FLOWER+3 );
}
bool isSeason( UCHAR Tile )
{
    return( Tile >= TILE_SEASON  &&  Tile <=TILE_SEASON+3 );
}
bool isBamboo(UCHAR t) {
    return( t >= TILE_BAMBOO && t <TILE_BAMBOO+9);
}
bool isCharacter(UCHAR t) {
    return( t >= TILE_CHARACTER && t <TILE_CHARACTER + 9);
}
bool isRod(UCHAR t) {
    return( t >= TILE_ROD && t <TILE_ROD + 9);
}
bool isDragon(UCHAR t) {
    return( t >= TILE_DRAGON && t < TILE_DRAGON +3);
}
bool isWind(UCHAR t) {
    return( t >= TILE_WIND && t < TILE_WIND +4);
}


bool BoardWidget::isMatchingTile( POSITION& Pos1, POSITION& Pos2 )
{
    // don't compare 'equal' positions
    if( memcmp( &Pos1, &Pos2, sizeof(POSITION) ) )
    {
        UCHAR FA = Pos1.f;
        UCHAR FB = Pos2.f;

        if( (FA == FB)
         || ( isFlower( FA ) && isFlower( FB ) )
         || ( isSeason( FA ) && isSeason( FB ) ) )
            return( true );
    }
    return( false );
}

// ---------------------------------------------------------
bool BoardWidget::findMove( POSITION& posA, POSITION& posB )
{
    short Pos_Ende = Game.MaxTileNum;  // Ende der PosTable

    for( short E=0; E<BoardLayout::depth; E++ )
    {
        for( short Y=0; Y<BoardLayout::height-1; Y++ )
        {
            for( short X=0; X<BoardLayout::width-1; X++ )
            {
                if( Game.Mask[E][Y][X] != (UCHAR) '1' )
                    continue;
                if( ! Game.Board[E][Y][X] )
                    continue;
                if( E < 4 )
                {
                    if( Game.Board[E+1][Y][X] || Game.Board[E+1][Y+1][X] ||
                        Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                        continue;
                }
                if( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
                    (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
                    continue;

                Pos_Ende--;
                PosTable[Pos_Ende].e = E;
                PosTable[Pos_Ende].y = Y;
                PosTable[Pos_Ende].x = X;
                PosTable[Pos_Ende].f = Game.Board[E][Y][X];




            }
        }
    }

 //   PosTable[0].e = BoardLayout::depth;  // 1. Paar noch nicht gefunden
    iPosCount = 0;  // Hier Anzahl der gefunden Paare merken


    // The new tile layout with non-contiguos horizantle spans
    // can lead to huge numbers of matching pairs being exposed.
    // we alter the loop to bail out when BoardLayout::maxTiles/2 pairs are found
    // (or less);
    while( Pos_Ende < Game.MaxTileNum-1 && iPosCount <BoardLayout::maxTiles-2)
    {
        for( short Pos = Pos_Ende+1; Pos < Game.MaxTileNum; Pos++)
        {
            if( isMatchingTile(PosTable[Pos], PosTable[Pos_Ende]) )
            {
		if (iPosCount <BoardLayout::maxTiles-2) {
                	PosTable[iPosCount++] = PosTable[Pos_Ende];
                	PosTable[iPosCount++] = PosTable[Pos];
		}
            }
        }
        Pos_Ende++;
    }

    if( iPosCount>=2 )
    {
        random.setSeed(0); // WABA: Why is the seed reset?
        short Pos = random.getLong(iPosCount) & -2;  // Gerader Wert
        posA = PosTable[Pos];
        posB = PosTable[Pos+1];

        return( true );
    }
    else
        return( false );
}

int BoardWidget::moveCount( )
{
    short Pos_Ende = Game.MaxTileNum;  // end of PosTable

    for( short E=0; E<BoardLayout::depth; E++ )
    {
        for( short Y=0; Y<BoardLayout::height-1; Y++ )
        {
            for( short X=0; X<BoardLayout::width-1; X++ )
            {
                if( Game.Mask[E][Y][X] != (UCHAR) '1' )
                    continue;
                if( ! Game.Board[E][Y][X] )
                    continue;
                if( E < 4 )
                {
                    if( Game.Board[E+1][Y][X] || Game.Board[E+1][Y+1][X] ||
                        Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                        continue;
                }
                if( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
                    (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
                    continue;

                Pos_Ende--;
                PosTable[Pos_Ende].e = E;
                PosTable[Pos_Ende].y = Y;
                PosTable[Pos_Ende].x = X;
                PosTable[Pos_Ende].f = Game.Board[E][Y][X];

            }
        }
    }

    iPosCount = 0;  // store number of pairs found

    while( Pos_Ende < Game.MaxTileNum-1 && iPosCount <BoardLayout::maxTiles-2)
    {
        for( short Pos = Pos_Ende+1; Pos < Game.MaxTileNum; Pos++)
        {
            if( isMatchingTile(PosTable[Pos], PosTable[Pos_Ende]) )
            {
		if (iPosCount <BoardLayout::maxTiles-2) {
                	PosTable[iPosCount++] = PosTable[Pos_Ende];
                	PosTable[iPosCount++] = PosTable[Pos];
		}
            }
        }
        Pos_Ende++;
    }

    return iPosCount/2;
}




// ---------------------------------------------------------
short BoardWidget::findAllMatchingTiles( POSITION& posA )
{
    short Pos = 0;

    for( short E=0; E<BoardLayout::depth; E++ )
    {
        for( short Y=0; Y<BoardLayout::height-1; Y++ )
        {
            for( short X=0; X<BoardLayout::width-1; X++ )
            {
                if( Game.Mask[E][Y][X] != (UCHAR) '1' )
                    continue;
                if( ! Game.Board[E][Y][X] )
                    continue;
                if( E < 4 )
                {
                    if( Game.Board[E+1][Y][X] || Game.Board[E+1][Y+1][X] ||
                        Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                        continue;
                }
                if( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
                    (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
                    continue;

                PosTable[Pos].e = E;
                PosTable[Pos].y = Y;
                PosTable[Pos].x = X;
                PosTable[Pos].f = Game.Board[E][Y][X];

                if( isMatchingTile(posA, PosTable[Pos]) )
                    Pos++;
            }
        }
    }
    return Pos;
}



// ---------------------------------------------------------
// This function replaces the old method of hilighting by
// modifying color 21 to color 20. This was single tileset
// specific. We now have two tile faces, one selected one not.

void BoardWidget::hilightTile( POSITION& Pos, bool on, bool doRepaint )
{

	if (on) {
		Game.hilighted[Pos.e][Pos.y][Pos.x]=1;
	} else {
		Game.hilighted[Pos.e][Pos.y][Pos.x]=0;
	}
	if (doRepaint) {
		updateBackBuffer=true;
		if (testWFlags(WNoAutoErase)) 
			update(); 
		else 
		{ 
			setWFlags(getWFlags() | WNoAutoErase ); 
			update(); 
			setWFlags(getWFlags() & (~WNoAutoErase) ); 
		} 
	}
}



// ---------------------------------------------------------
void BoardWidget::drawBoard(bool )
{
   updateBackBuffer=true;
   if (testWFlags(WNoAutoErase)) 
      update(); 
   else 
   { 
      setWFlags(getWFlags() | WNoAutoErase ); 
      update(); 
      setWFlags(getWFlags() & (~WNoAutoErase) ); 
   } 
   drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::putTile( POSITION& Pos, bool doRepaint )
{
    short E=Pos.e;
    short Y=Pos.y;
    short X=Pos.x;

	// we ensure that any tile we put on has highlighting off
    Game.putTile( E, Y, X, Pos.f );
	Game.hilighted[E][Y][X] = 0;
    if (doRepaint) {
	updateBackBuffer=true;
	if (testWFlags(WNoAutoErase)) 
		update(); 
	else 
	{ 
		setWFlags(getWFlags() | WNoAutoErase ); 
		update(); 
		setWFlags(getWFlags() & (~WNoAutoErase) ); 
	} 
    }
}


// ---------------------------------------------------------
void BoardWidget::removeTile( POSITION& Pos , bool doRepaint)
{

    short E = Pos.e;
    short Y = Pos.y;
    short X = Pos.x;

    Game.TileNum--;                    // Eine Figur weniger
    Game.MoveList[Game.TileNum] = Pos; // Position ins Protokoll eintragen



    // remove tile from game board
    Game.putTile( E, Y, X, 0 );
    if (doRepaint) {
        updateBackBuffer=true;
	if (testWFlags(WNoAutoErase)) 
		update(); 
	else 
	{ 
		setWFlags(getWFlags() | WNoAutoErase ); 
		update(); 
		setWFlags(getWFlags() & (~WNoAutoErase) ); 
	} 
    }
}

// ---------------------------------------------------------
void BoardWidget::mousePressEvent ( QMouseEvent* event )
{
    if (gamePaused)
        return;

    if( event->button() == LeftButton )
    {
        if( TimerState == Demo )
        {
            stopDemoMode();
        }
        else if( showMatch )
        {
            stopMatchAnimation();
        }

        if( showHelp ) // stop hilighting tiles
            helpMoveStop();

        if( MouseClickPos1.e == BoardLayout::depth )       // first tile
        {
            transformPointToPosition( event->pos(), MouseClickPos1 );

            if( MouseClickPos1.e != BoardLayout::depth && showMatch )
            {
                matchCount = findAllMatchingTiles( MouseClickPos1 );
                TimerState = Match;
                iTimerStep = 1;
                matchAnimationTimeout();
                cheatsUsed++;
            }
        }
        else                                // second tile
        {
            transformPointToPosition( event->pos(), MouseClickPos2 );
            if( MouseClickPos2.e == BoardLayout::depth )
            {
                cancelUserSelectedTiles();
            }
            else
            {
                if( isMatchingTile( MouseClickPos1, MouseClickPos2 ) )
                {
                    // update the removed tiles (we do this before the remove below
                    // so that we only require 1 screen paint for both actions)
                    setRemovedTilePair(MouseClickPos1, MouseClickPos2);

                    // now we remove the tiles from the board
                    removeTile(MouseClickPos1, false);
                    removeTile(MouseClickPos2);

                    // removing a tile means redo is impossible without
                    // a further undo.
                    Game.allow_redo=false;
                    demoModeChanged(false);
                    drawTileNumber();

                    // if no tiles are left, the player has `won`, so celebrate
                    if( Game.TileNum == 0 )
                    {
                        gameOver(Game.MaxTileNum,cheatsUsed);
                    }
                    // else if no more moves are possible, display the sour grapes dialog
                    else if( ! findMove( TimerPos1, TimerPos2 ) )
                    {
                        KMessageBox::information(this, i18n("Game over: You have no moves left."));
                    }
                }
                else
                {
                    // redraw tiles in normal state
                    hilightTile( MouseClickPos1, false, false );
                    hilightTile( MouseClickPos2, false );
                }
                MouseClickPos1.e = BoardLayout::depth;     // mark tile position as invalid
                MouseClickPos2.e = BoardLayout::depth;
            }
        }
    }
}


// ----------------------------------------------------------
/**
    Transform window point to board position.

    @param  point          Input: Point in window coordinates
    @param  MouseClickPos  Output: Position in game board
*/
void BoardWidget::transformPointToPosition(
        const QPoint& point,
        POSITION&     MouseClickPos
    )
{
    short E,X,Y;

    // iterate over E coordinate from top to bottom
    for( E=BoardLayout::depth-1; E>=0; E-- )
    {
        // calculate mouse coordiantes --> position in game board
	// the factor -theTiles.width()/2 must keep track with the
	// offset for blitting in the print Event (FIX ME)
        X = ((point.x()-theTiles.width()/2)- (E+1)*theTiles.shadowSize()) / theTiles.qWidth();
        Y = ((point.y()-theTiles.height()/2) + E*theTiles.shadowSize()) / theTiles.qHeight();


	// changed to allow x == 0
        // skip when position is illegal
        if (X<0 || X>=BoardLayout::width || Y<0 || Y>=BoardLayout::height)
		continue;

        //
        switch( Game.Mask[E][Y][X] )
        {
            case (UCHAR)'3':    X--;Y--;
                                break;

            case (UCHAR)'2':    X--;
                                break;

            case (UCHAR)'4':    Y--;
                                break;

            case (UCHAR)'1':    break;

            default :           continue;
        }
        // if gameboard is empty, skip
        if ( ! Game.Board[E][Y][X] ) continue;
        // tile must be 'free' (nothing left, right or above it)
        if( E < 4 )
        {
            if( Game.Board[E+1][Y][X]   || Game.Board[E+1][Y+1][X] ||
                (X<BoardLayout::width-2 && Game.Board[E+1][Y][X+1]) ||
	        (X<BoardLayout::width-2 && Game.Board[E+1][Y+1][X+1]) )
                continue;
        }

	// No left test on left edge
        if (( X > 0) && (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1])) {
		if ((X<BoardLayout::width-2) && (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2])) {


            	continue;
		}
	}

        // here, position is legal
        MouseClickPos.e = E;
        MouseClickPos.y = Y;
        MouseClickPos.x = X;
        MouseClickPos.f = Game.Board[E][Y][X];
        // give visible feedback
        hilightTile( MouseClickPos );
        break;
    }
}

// ---------------------------------------------------------
bool BoardWidget::loadBoard( )
{
    GAMEDATA newGame;

    memset( &newGame, 0, sizeof( newGame ) );
    theBoardLayout.copyBoardLayout((UCHAR *) newGame.Mask, newGame.MaxTileNum);
    Game = newGame;
    return(true);
}

// ---------------------------------------------------------
void BoardWidget::setStatusText( const QString & pszText )
{
    emit statusTextChanged( pszText, gameGenerationNum );
}



// ---------------------------------------------------------
bool BoardWidget::loadBackground(
        const QString& pszFileName,
        bool        bShowError
    )
{
    if( ! theBackground.load( pszFileName, requiredWidth(), requiredHeight()) )
    {
        if( bShowError )
            KMessageBox::sorry(this, i18n("Failed to load image:\n%1").arg(pszFileName) );
        return( false );
    }
    Prefs::setBackground(pszFileName);
    Prefs::writeConfig();
    return true;
}

// ---------------------------------------------------------
void BoardWidget::drawTileNumber()
{
    emit tileNumberChanged( Game.MaxTileNum, Game.TileNum, moveCount( ) );
}

// ---------------------------------------------------------
void BoardWidget::cancelUserSelectedTiles()
{
    if( MouseClickPos1.e != BoardLayout::depth )
    {
        hilightTile( MouseClickPos1, false ); // redraw tile
        MouseClickPos1.e = BoardLayout::depth;    // mark tile invalid
    }
}

// ---------------------------------------------------------
void BoardWidget::setRemovedTilePair(POSITION &a, POSITION &b) {

	if (isFlower(a.f)) {
		removedFlower[a.f-TILE_FLOWER]++;
		removedFlower[b.f-TILE_FLOWER]++;
		return;
	}

	if (isSeason(a.f)) {
		removedSeason[a.f-TILE_SEASON]++;
		removedSeason[b.f-TILE_SEASON]++;
		return;
	}
	if (isCharacter(a.f)) {
		removedCharacter[a.f - TILE_CHARACTER]+=2;
		return;
	}

	if (isBamboo(a.f)) {
		removedBamboo[a.f - TILE_BAMBOO]+=2;
		return;
	}
	if (isRod(a.f)) {
		removedRod[a.f - TILE_ROD]+=2;
		return;
	}
	if (isDragon(a.f)){
		removedDragon[a.f - TILE_DRAGON]+=2;
		return;
	}
	if (isWind(a.f)){
		removedWind[a.f - TILE_WIND]+=2;
		return;
	}
}

// ---------------------------------------------------------
void BoardWidget::clearRemovedTilePair(POSITION &a, POSITION &b) {

        if (isFlower(a.f)) {
                removedFlower[a.f-TILE_FLOWER]--;
                removedFlower[b.f-TILE_FLOWER]--;
                return;
        }

        if (isSeason(a.f)) {
                removedSeason[a.f-TILE_SEASON]--;
                removedSeason[b.f-TILE_SEASON]--;
                return;
        }
        if (isCharacter(a.f)) {
                removedCharacter[a.f - TILE_CHARACTER]-=2;
                return;
        }

        if (isBamboo(a.f)) {
                removedBamboo[a.f - TILE_BAMBOO]-=2;
                return;
        }
        if (isRod(a.f)){
                removedRod[a.f - TILE_ROD]-=2;
                return;
        }
        if (isDragon(a.f)){
                removedDragon[a.f - TILE_DRAGON]-=2;
                return;
        }
        if (isWind(a.f)){
                removedWind[a.f - TILE_WIND]-=2;
                return;
        }
}


// ---------------------------------------------------------
void BoardWidget::initialiseRemovedTiles() {
	for (int pos=0; pos<9; pos++) {
		removedCharacter[pos]=0;
		removedBamboo[pos]=0;
		removedRod[pos]=0;
		removedDragon[pos %3] = 0;
		removedFlower[pos % 4] = 0;
		removedWind[pos % 4] = 0;
		removedSeason[pos % 4] = 0;

	}

}

// ---------------------------------------------------------
bool BoardWidget::loadTileset(const QString &path) {

  if (theTiles.loadTileset(path)) {
    Prefs::setTileSet(path);
    Prefs::writeConfig();
    return true;
  } else {
    return false;
  }

}

bool BoardWidget::loadBoardLayout(const QString &file) {
  if (theBoardLayout.loadBoardLayout(file)) {
    Prefs::setLayout(file);
    Prefs::writeConfig();
    return true;
  }
  return false;
}

void BoardWidget::updateScaleMode() {


	theBackground.scaleModeChanged();
}



// calculate the required window width (board + removed tiles)
int BoardWidget::requiredWidth() {
	int res = ((BoardLayout::width+12)* theTiles.qWidth());

	return(res);
}

// calculate the required window height (board + removed tiles)
int BoardWidget::requiredHeight() {

	int res = ((BoardLayout::height+3)* theTiles.qHeight());
	return(res);
}

void BoardWidget::tileSizeChanged() {
	theTiles.setScaled(Prefs::miniTiles());
	theBackground.sizeChanged(requiredWidth(), requiredHeight());

}

// shuffle the remaining tiles around, useful if a deadlock ocurrs
// this is a big cheat so we penalise the user.
void BoardWidget::shuffle() {
	int count = 0;
	// copy positions and faces of the remaining tiles into
	// the pos table
	for (int e=0; e<BoardLayout::depth; e++) {
	    for (int y=0; y<BoardLayout::height; y++) {
		for (int x=0; x<BoardLayout::width; x++) {
		    if (Game.Board[e][y][x] && Game.Mask[e][y][x] == '1') {
			PosTable[count].e = e;
			PosTable[count].y = y;
			PosTable[count].x = x;
			PosTable[count].f = Game.Board[e][y][x];
			count++;
		    }
		}
	    }

	}


	// now lets randomise the faces, selecting 400 pairs at random and
	// swapping the faces.
	for (int ran=0; ran < 400; ran++) {
		int pos1 = random.getLong(count);
		int pos2 = random.getLong(count);
		if (pos1 == pos2)
			continue;
		BYTE f = PosTable[pos1].f;
		PosTable[pos1].f = PosTable[pos2].f;
		PosTable[pos2].f = f;
	}

	// put the rearranged tiles back.
	for (int p=0; p<count; p++)
		Game.putTile(PosTable[p]);


	// force a redraw

	updateBackBuffer=true;
       repaint(false);


	// I consider this s very bad cheat so, I punish the user
	// 300 points per use
	cheatsUsed += 15;
	drawTileNumber();
}


#include "boardwidget.moc"
