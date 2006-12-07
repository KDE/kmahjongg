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

#include "boardwidget.h"
#include "prefs.h"

#include <kmessagebox.h>
#include <krandom.h>
#include <QTimer>
#include <qpainter.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <QFile>
#include <qapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <QtDebug>

/**
 * Constructor.
 * Loads tileset and background bitmaps.
 */
BoardWidget::BoardWidget( QWidget* parent )
  : KGameCanvasWidget( parent ), theTiles()
{
    QPalette palette;
    palette.setColor( backgroundRole(), Qt::black );
    setPalette(palette);

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()),
             this, SLOT(helpMoveTimeout()) );

    TimerState = Stop;
    gamePaused = false;
    iTimerStep = 0;
    matchCount = 0;
    showMatch = false;
    showHelp = false;

    //memset( &Game->Mask, 0, sizeof( Game->Mask ) );
    gameGenerationNum = 0;
    m_angle = NE;

    // Load layout first
    QString tFile;

    getFileOrDefault(Prefs::layout(), "layout", tFile);
    if( ! loadBoardLayout(tFile) )
    {
	KMessageBox::information(this,
		   i18n("An error occurred when loading the board layout %1\n"
                "KMahjongg will continue with the default layout.", tFile));
    }

    //Initialize our Game structure
    Game = new GameData(&theBoardLayout);

    MouseClickPos1.e = Game->m_depth;     // mark tile position as invalid
    MouseClickPos2.e = Game->m_depth;

    //Now apply our visual settings
    // Load tileset. First try to load the last use tileset
    getFileOrDefault(Prefs::tileSet(), "tileset", tFile);

    if (!loadTileset(tFile)){
        KMessageBox::information(this,
                           i18n("An error occurred when loading the tileset file %1\n"
                                "KMahjongg will continue with the default tileset.", tFile));
    }

    getFileOrDefault(Prefs::background(), "bgnd", tFile);

    // Load background
    if( ! loadBackground(tFile, false ) )
    {
	KMessageBox::information(this,
		   i18n("An error occurred when loading the background image\n%1", tFile)+
		   i18n("KMahjongg will continue with the default background."));
    }

    //setDisplayedWidth();
    loadSettings();
}

BoardWidget::~BoardWidget(){
  saveSettings();
  if (Game) delete Game;
}

void BoardWidget::loadSettings(){
  setDisplayedWidth();
  theBackground.tile = Prefs::tiledBackground();
  updateScaleMode();
  drawBoard(true);
}

void BoardWidget::resizeEvent ( QResizeEvent * event )
{
    resizeTileset(event->size());
    theBackground.sizeChanged(requiredWidth(), requiredHeight());
    drawBoard(true);
}

void BoardWidget::resizeTileset ( const QSize & wsize )
{
    //qDebug() << "resized:" << event->oldSize() << event->size();
    QSize newtiles = theTiles.preferredTileSize(wsize, requiredHorizontalCells(), requiredVerticalCells());
    //qDebug() << "new tilesize:" << newtiles;
    theTiles.reloadTileset(newtiles);
    stopMatchAnimation();
}



void BoardWidget::saveSettings(){
  // Preview can't handle this.  TODO
  //KConfig *config=KGlobal::config();
  //config->setGroup("General");

  //config->writePathEntry("Tileset_file", tileFile);
  //config->writePathEntry("Background_file", backgroundFile);
  //config->writePathEntry("Layout_file", layout);
}

void BoardWidget::getFileOrDefault(const QString &filename, const QString &type, QString &res)
{
	QString picsPos = "pics/";
	picsPos += "default.";
	picsPos += type;

	if (QFile::exists(filename)) {
		res = filename;
	}
    else {
        res = KStandardDirs::locate("appdata", picsPos);
	}

    if (res.isEmpty()) {
		KMessageBox::error(this, i18n("KMahjongg could not locate the file: %1\n"
                                      "or the default file of type: %2\n"
                                      "KMahjongg will now terminate", filename, type) );
		qApp->quit();
	}
}

void BoardWidget::setDisplayedWidth() {
  //TODO remove method and let the layout handle it
  //for now we just force our resizeEvent() to be called
  resize(width() , height());
}

void BoardWidget::populateSpriteMap() {
    QPixmap  back;

    //Delete previous sprites (full update), synchronize state with GameData
    while (!items()->isEmpty())
	delete items()->first();

    //Clear our spritemap as well
    spriteMap.clear();

    //Recreate our background
    back = theBackground.getBackground();
    QPalette palette;
    palette.setBrush( backgroundRole(), QBrush( back ) );
    setPalette( palette );
    setAutoFillBackground (true);

    //create the sprites
    for (int z=0; z<Game->m_depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < Game->m_height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x=Game->m_width-1; x>=0; x--) {

		// skip if no tile to display
		if (!Game->tilePresent(z,y,x))
			continue;

                QPixmap s;
		QPixmap us;
		QPixmap f;
		bool selected = false;
		s= theTiles.selectedTile(m_angle);
		us= theTiles.unselectedTile(m_angle);
		f= theTiles.tileface(Game->BoardData(z,y,x)-TILE_OFFSET);
		if (Game->HighlightData(z,y,x)) {
		    selected = true;
		}

		  TileSprite * thissprite = new TileSprite(this, us, s, f, m_angle, selected);

		  spriteMap.insert(TileCoord(x,y,z), thissprite);
            }
        }
    }

    updateSpriteMap();
}


void BoardWidget::updateSpriteMap() {
    // initial offset on the screen of tile 0,0
    // think of it as what is left if you add all tilefaces (qwidth/heigh*2) plus one (wholetile-shadow(3dindent)-tileface), and divide by 2
    int xOffset = (width() - (Game->m_width*(theTiles.qWidth())) - (theTiles.width()-(theTiles.qWidth()*2)))/2;;
    int yOffset = (height() - (Game->m_height*(theTiles.qHeight())) - (theTiles.height()-(theTiles.qHeight()*2)))/2;;

    // we iterate over the depth stacking order. Each successive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    switch (m_angle)
    {
	case NW:
	//remove shadow from margin calculation
	xOffset += theTiles.levelOffset()/2;
        yOffset += theTiles.levelOffset()/2;

	//Position
	for (int z=0; z<Game->m_depth; z++) {
		// we draw down the board so the tile below over rights our border
		for (int y = 0; y < Game->m_height; y++) {
		// drawing right to left to prevent border overwrite
		for (int x=Game->m_width-1; x>=0; x--) {
			int sx = x*(theTiles.qWidth()  )+xOffset;
			int sy = y*(theTiles.qHeight()  )+yOffset;
	
			// skip if no tile to display
			if (!Game->tilePresent(z,y,x))
				continue;
	
			TileSprite * thissprite =spriteMap.value(TileCoord(x,y,z));
	
			if (thissprite) thissprite->moveTo(sx, sy);
			if (thissprite) thissprite->show();
		}
		}
		xOffset +=theTiles.levelOffset();
		yOffset -=theTiles.levelOffset();
	}
	//Layer
	for (int z=0; z<Game->m_depth; z++) {
		// start drawing in diagonal for correct layering. For this we double the board, 
		//actually starting outside of it, in the bottom right corner, so our first diagonal ends
		// at the actual top right corner of the board
		for (int x=Game->m_width*2; x>=0; x--) {
		// reset the offset
		int offset = 0;
		for (int y=Game->m_height-1; y>=0; y--) {
			if (Game->tilePresent(z,y,x-offset))
			{
				TileSprite * thissprite =spriteMap.value(TileCoord(x-offset,y,z));
				if (thissprite) thissprite->raise();
			}
			//at each pass, move one place to the left
			offset++;
		}
		}
	}
	break;

	case NE:
	xOffset -= theTiles.levelOffset()/2;
        yOffset += theTiles.levelOffset()/2;
	//Position
	for (int z=0; z<Game->m_depth; z++) {
		// we draw down the board so the tile below over rights our border
		for (int y = 0; y < Game->m_height; y++) {
		// drawing right to left to prevent border overwrite
		for (int x=0; x<=Game->m_width-1; x++) {
			int sx = x*(theTiles.qWidth()  )+xOffset;
			int sy = y*(theTiles.qHeight()  )+yOffset;
	
			// skip if no tile to display
			if (!Game->tilePresent(z,y,x))
				continue;
	
			TileSprite * thissprite =spriteMap.value(TileCoord(x,y,z));
	
			if (thissprite) thissprite->moveTo(sx, sy);
			if (thissprite) thissprite->show();
		}
		}
		xOffset -=theTiles.levelOffset();
		yOffset -=theTiles.levelOffset();
	}
	//Layer
	for (int z=0; z<Game->m_depth; z++) {
		// start drawing in diagonal for correct layering. For this we double the board, 
		//actually starting outside of it, in the bottom right corner, so our first diagonal ends
		// at the actual top right corner of the board
		for (int x=-(Game->m_width); x<=Game->m_width-1; x++) {
		// reset the offset
		int offset = 0;
		for (int y=Game->m_height-1; y>=0; y--) {
			if (Game->tilePresent(z,y,x+offset))
			{
				TileSprite * thissprite =spriteMap.value(TileCoord(x+offset,y,z));
				if (thissprite) thissprite->raise();
			}
			//at each pass, move one place to the right
			offset++;
		}
		}
	}
	break;

	case SE:
	xOffset -= theTiles.levelOffset()/2;
        yOffset -= theTiles.levelOffset()/2;
	//Position
	for (int z=0; z<Game->m_depth; z++) {
		for (int y = Game->m_height-1; y >= 0; y--) {
		for (int x=0; x<=Game->m_width-1; x++) {
			int sx = x*(theTiles.qWidth()  )+xOffset;
			int sy = y*(theTiles.qHeight()  )+yOffset;
	
			if (!Game->tilePresent(z,y,x))
				continue;
	
			TileSprite * thissprite =spriteMap.value(TileCoord(x,y,z));
	
			if (thissprite) thissprite->moveTo(sx, sy);
			if (thissprite) thissprite->show();
		}
		}
		xOffset -=theTiles.levelOffset();
		yOffset +=theTiles.levelOffset();
	}
	//Layer
	for (int z=0; z<Game->m_depth; z++) {
		for (int x=-(Game->m_width); x<=Game->m_width-1; x++) {
		int offset = 0;
		for (int y=0; y<Game->m_height; y++) {
			if (Game->tilePresent(z,y,x+offset))
			{
				TileSprite * thissprite =spriteMap.value(TileCoord(x+offset,y,z));
				if (thissprite) thissprite->raise();
			}
			offset++;
		}
		}
	}
	break;

	case SW:
	xOffset += theTiles.levelOffset()/2;
        yOffset -= theTiles.levelOffset()/2;

	//Position
	for (int z=0; z<Game->m_depth; z++) {
		for (int y = Game->m_height-1; y >= 0; y--) {
		for (int x=Game->m_width-1; x>=0; x--) {
			int sx = x*(theTiles.qWidth()  )+xOffset;
			int sy = y*(theTiles.qHeight()  )+yOffset;

			if (!Game->tilePresent(z,y,x))
				continue;
	
			TileSprite * thissprite =spriteMap.value(TileCoord(x,y,z));
	
			if (thissprite) thissprite->moveTo(sx, sy);
			if (thissprite) thissprite->show();
		}
		}
		xOffset +=theTiles.levelOffset();
		yOffset +=theTiles.levelOffset();
	}
	//Layer
	for (int z=0; z<Game->m_depth; z++) {
		for (int x=Game->m_width*2; x>=0; x--) {
		int offset = 0;
		for (int y=0; y<Game->m_height; y++) {
			if (Game->tilePresent(z,y,x-offset))
			{
				TileSprite * thissprite =spriteMap.value(TileCoord(x-offset,y,z));
				if (thissprite) thissprite->raise();
			}
			offset++;
		}
		}
	}
	break;
   }
}

// for a given cell x y calc how that cell is shadowed
// returnd left = width of left hand side shadow
// t = height of top shadow
// c = width and height of corner shadow

/*

// ---------------------------------------------------------
void BoardWidget::paintEvent( QPaintEvent* pa )
{
    QPixmap  *back;

    int xx = pa->rect().left();
    int xheight = pa->rect().height();
    int xwidth  = pa->rect().width();

    back = theBackground.getBackground();

    QPainter p;
    if (gamePaused) {
        // If the game is paused, then blank out the board.
        // We tolerate no cheats around here folks..
        p.begin(this);
        p.drawPixmap( xx, pa->rect().top(),
                *back, xx, pa->rect().top(), xwidth, xheight );
        p.end();
	return;
    }

    // if the repaint is because of a window redraw after a move
    // or a menu roll up, then just blit in the last rendered image
    if (!updateBackBuffer) {
        p.begin(this);
    	p.drawPixmap( xx,pa->rect().top(),
		backBuffer, xx, pa->rect().top(), xwidth, xheight );
        p.end();
	return;
    }

    // update the complete drawArea

    backBuffer = QPixmap(back->width(), back->height());

    // erase out with the background
    p.begin(&backBuffer);
    p.drawPixmap( xx, pa->rect().top(),
                *back, xx,pa->rect().top(), back->width(), back->height() );

    // initial offset on the screen of tile 0,0
    int xOffset = theTiles.width()/2;
    int yOffset = theTiles.height()/2;
    //short tile = 0;


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
		if (!Game->tilePresent(z,y,x))
			continue;

                QPixmap *t;
		if (Game->hilighted[z][y][x]) {
		   t= theTiles.selectedPixmaps(
				Game->Board[z][y][x]-TILE_OFFSET);
		} else {
		   t= theTiles.unselectedPixmaps(
				Game->Board[z][y][x]-TILE_OFFSET);
                }

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border

                //TODO convert to QGV and let it layer everything properly
                    p.drawPixmap(  sx, sy, *t, 0,0, t->width(), t->height() );


            }
        }
        xOffset +=theTiles.levelOffset();
        yOffset -=theTiles.levelOffset();
    }


    // Now we add the list of canceled tiles
//TODO major overhaul of this functionality
if (Prefs::showRemoved()) {

    // we start blitting as usuall right to left, top to bottom, first
    // we calculate the start pos of the first tile, allowing space for
    // the upwards at rightwards creep when stacking in 3d
    unsigned short xPos = backBuffer.width()-(3*theTiles.levelOffset())-theTiles.width();
    unsigned short yPos = (3*theTiles.levelOffset());

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

	    stackTiles(&p, tile, last, xPos, yPos);
	    stackTiles(&p, TILE_ROD+pos, removedRod[pos],
		xPos - (1*(theTiles.width() - theTiles.levelOffset())) , yPos);
	    stackTiles(&p, TILE_BAMBOO+pos, removedBamboo[pos],
		xPos - (2*(theTiles.width() - theTiles.levelOffset())) , yPos);
	    stackTiles(&p, TILE_CHARACTER+pos, removedCharacter[pos],
		xPos - (3*(theTiles.width() - theTiles.levelOffset())) , yPos);



	yPos += theTiles.height()-theTiles.levelOffset();
    }
} //removed

    updateBackBuffer=false;
    p.end(); //backbuffer
    p.begin(this);
    p.drawPixmap(xx,pa->rect().top(), backBuffer, xx, pa->rect().top(), xwidth, xheight);
    p.end();
}*/

void BoardWidget::stackTiles(QPainter* p, unsigned char t, unsigned short h, unsigned short x,unsigned  short y)
{
    int ss = theTiles.levelOffset();
    QPen line;
    p->setBackgroundMode(Qt::OpaqueMode);
    p->setBackground(Qt::black);

    line.setWidth(1);
    line.setColor(Qt::white);
    p->save();
    p->setPen(line);
    int x2 = x+theTiles.width()-ss-1;
    int y2 = y+theTiles.height()-1;
    p->drawLine(x, y+ss, x2, y+ss);
    p->drawLine(x, y+ss, x, y2);
    p->drawLine(x2, y+ss, x2, y2);
    p->drawLine(x+1, y2, x2, y2);

   // p.fillRect(x+1, y+ss+1, theTiles.width()-ss-2, theTiles.height()-ss-2, QBrush(lightGray));

    for (unsigned short pos=0; pos < h; pos++) {
       QPixmap pix = theTiles.tileface(t-TILE_OFFSET);
       p->drawPixmap( x+(pos*ss), y-(pos*ss),
                    pix );
    }
    p->restore();
}


void BoardWidget::pause() {
	gamePaused = !gamePaused;
	drawBoard(true);
}

void BoardWidget::gameLoaded()
{
	int i;
	Game->initialiseRemovedTiles();
	i = Game->TileNum;
	// use the history of moves to put in the removed tiles area the correct tiles
	while (i < Game->MaxTileNum )
	{
		Game->setRemovedTilePair(Game->MoveListData(i), Game->MoveListData(i+1));
		i +=2;
	}
	drawBoard();
}

// ---------------------------------------------------------
int BoardWidget::undoMove()
{
    cancelUserSelectedTiles();

    if( Game->TileNum < Game->MaxTileNum )
    {

        Game->clearRemovedTilePair(Game->MoveListData(Game->TileNum), Game->MoveListData(Game->TileNum+1));
        putTileInBoard( Game->MoveListData(Game->TileNum), false );
        Game->TileNum++;
        putTileInBoard( Game->MoveListData(Game->TileNum) );
        Game->TileNum++;
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

    if( Game->findMove( TimerPos1, TimerPos2 ) )
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
    {
        timer->setSingleShot(true);
        timer->start( ANIMSPEED );
    }
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
    setStatusText( i18n("Now it is you again.") );
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
                if( ! Game->findMove( TimerPos1, TimerPos2 ) )
	        {
                    // if computer has won
	            if( Game->TileNum == 0 )
                    {
                        animateMoveList();
                    }
                    // else computer has lost
                    else
                    {
                        setStatusText( i18n("Your computer has lost the game.") );
                        while( Game->TileNum < Game->MaxTileNum )
                        {
                            putTileInBoard( Game->MoveListData(Game->TileNum), false );
                            Game->TileNum++;
                            putTileInBoard( Game->MoveListData(Game->TileNum) );
                            Game->TileNum++;
                            drawTileNumber();
                        }
                    }
                    TimerState = Stop;
                    //startDemoMode();
		    //do not loop demo
		    stopDemoMode();
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
                Game->setRemovedTilePair(TimerPos1, TimerPos2);
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
            hilightTile(Game->PosTable[Pos], true);
        }
    }
    else
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {
            hilightTile(Game->PosTable[Pos], false);
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
        hilightTile(Game->PosTable[Pos], false);
    }
    TimerState = Stop;
    matchCount = 0;
}

void BoardWidget::redoMove()
{

	Game->setRemovedTilePair(Game->MoveListData(Game->TileNum-1),Game->MoveListData(Game->TileNum-2));
        removeTile(Game->MoveListData(Game->TileNum-1), false);
        removeTile(Game->MoveListData(Game->TileNum-1));
        drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::animateMoveList()
{
    setStatusText( i18n("Congratulations. You have won!") );
    animatingMoveListForward();

    /*if (Prefs::playAnimation())
    {
        while( Game->TileNum < Game->MaxTileNum )
        {
            // put back all tiles
            putTileInBoard(Game->MoveListData(Game->TileNum));
            Game->TileNum++;
            putTileInBoard(Game->MoveListData(Game->TileNum), false);
            Game->TileNum++;
            drawTileNumber();
        }
        while( Game->TileNum > 0 )
        {
            // remove all tiles
            removeTile(Game->MoveListData(Game->TileNum-1), false);
            removeTile(Game->MoveListData(Game->TileNum-1));
            drawTileNumber();
        }
    }

    calculateNewGame();*/
}

void BoardWidget::animatingMoveListForward()
{
    if (Game->TileNum < Game->MaxTileNum) {
        // put back all tiles
        putTileInBoard(Game->MoveListData(Game->TileNum));
        Game->TileNum++;
        putTileInBoard(Game->MoveListData(Game->TileNum), false);
        Game->TileNum++;
        drawTileNumber();
        QTimer::singleShot(200, this, SLOT(animatingMoveListForward()));
    } else {
        //start removal
        QTimer::singleShot(200, this, SLOT(animatingMoveListBackwards()));
    }
}

void BoardWidget::animatingMoveListBackwards()
{
    if (Game->TileNum > 0 ) {
        // remove all tiles
        removeTile(Game->MoveListData(Game->TileNum-1), false);
        removeTile(Game->MoveListData(Game->TileNum-1));
        drawTileNumber();
        QTimer::singleShot(200, this, SLOT(animatingMoveListBackwards()));
    } else {
        //end of animation
        //calculateNewGame();
    }
}

// ---------------------------------------------------------
void BoardWidget::calculateNewGame( int gNumber)
{
    cancelUserSelectedTiles();
    stopMatchAnimation();
    Game->initialiseRemovedTiles();
    setStatusText( i18n("Calculating new game...") );


    if( !loadBoard())
    {
        setStatusText( i18n("Error converting board information!") );
        return;
    }

    if (gNumber == -1) {
    	gameGenerationNum = KRandom::random();
    } else {
	gameGenerationNum = gNumber;
    }

    Game->random.setSeed(gameGenerationNum);

    // Translate Game->Map to an array of POSITION data.  We only need to
    // do this once for each new game.
    //memset(Game->tilePositions, 0, sizeof(Game->tilePositions));
    Game->generateTilePositions();

    // Now use the tile position data to generate tile dependency data.
    // We only need to do this once for each new game.
    Game->generatePositionDepends();

    // Now try to position tiles on the board, 64 tries max.
    for( short nr=0; nr<64; nr++ )
    {
        if( Game->generateStartPosition2() )
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
// This function replaces the old method of hilighting by
// modifying color 21 to color 20. This was single tileset
// specific. We now have two tile faces, one selected one not.

void BoardWidget::hilightTile( POSITION& Pos, bool on, bool doRepaint )
{
	TileSprite * atile = 0;

	TileCoord coord = TileCoord(Pos.x,Pos.y,Pos.e);

	if (spriteMap.contains(coord)) {
	  atile = spriteMap.value(coord);
	}

	if (on) {
		Game->setHighlightData(Pos.e,Pos.y,Pos.x,1);
		if (atile)
		//atile->setPixmap(*(theTiles.selectedPixmaps(
				//Game->BoardData(Pos.e,Pos.y,Pos.x)-TILE_OFFSET)));
		atile->setSelected(true);
	} else {
		Game->setHighlightData(Pos.e,Pos.y,Pos.x,0);
		if (atile)
		//atile->setPixmap(*(theTiles.unselectedPixmaps(
				//Game->BoardData(Pos.e,Pos.y,Pos.x)-TILE_OFFSET)));
		atile->setSelected(false);
	}
}



// ---------------------------------------------------------
void BoardWidget::drawBoard(bool )
{
   populateSpriteMap();
   drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::putTileInBoard( POSITION& Pos, bool doRepaint )
{
    short E=Pos.e;
    short Y=Pos.y;
    short X=Pos.x;

    // we ensure that any tile we put on has highlighting off
    Game->putTile( E, Y, X, Pos.f );
    Game->setHighlightData(E,Y,X,0);

    QPixmap s;
    QPixmap us;
    QPixmap f;
    s= theTiles.selectedTile(m_angle);
    us= theTiles.unselectedTile(m_angle);
    f= theTiles.tileface(Game->BoardData(E,Y,X)-TILE_OFFSET);
    TileSprite * thissprite = new TileSprite(this, us, s, f, m_angle, false);
    //thissprite->moveTo(sx, sy);
    //thissprite->setOpacity(0);
    thissprite->show();
    //thissprite->fadeIn();
    spriteMap.insert(TileCoord(X,Y,E), thissprite);

    updateSpriteMap();
}


// ---------------------------------------------------------
//TODO move this to Game after handling the repaint situation
void BoardWidget::removeTile( POSITION& Pos , bool doRepaint)
{
    short E = Pos.e;
    short Y = Pos.y;
    short X = Pos.x;

    Game->TileNum--;                    // Eine Figur weniger
    Game->setMoveListData(Game->TileNum,Pos); // Position ins Protokoll eintragen

    TileSprite * thissprite =spriteMap.value(TileCoord(X,Y,E));
    if (thissprite) delete thissprite;
    //fade out, TileSprite will delete itself
    //if (thissprite) thissprite->fadeOut();

    spriteMap.remove(TileCoord(X,Y,E));
    // remove tile from game board
    Game->putTile( E, Y, X, 0 );
}

// ---------------------------------------------------------
void BoardWidget::mousePressEvent ( QMouseEvent* event )
{
    if (gamePaused)
        return;

    if( event->button() == Qt::LeftButton )
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

        if( MouseClickPos1.e == Game->m_depth )       // first tile
        {
            transformPointToPosition( event->pos(), MouseClickPos1 );

            if( MouseClickPos1.e != Game->m_depth && showMatch )
            {
                matchCount = Game->findAllMatchingTiles( MouseClickPos1 );
                TimerState = Match;
                iTimerStep = 1;
                matchAnimationTimeout();
                cheatsUsed++;
            }
        }
        else                                // second tile
        {
            transformPointToPosition( event->pos(), MouseClickPos2 );
            if( MouseClickPos2.e == Game->m_depth )
            {
                cancelUserSelectedTiles();
            }
            else
            {
                if( Game->isMatchingTile( MouseClickPos1, MouseClickPos2 ) )
                {
                    // update the removed tiles (we do this before the remove below
                    // so that we only require 1 screen paint for both actions)
                    Game->setRemovedTilePair(MouseClickPos1, MouseClickPos2);

                    // now we remove the tiles from the board*t, 
                    removeTile(MouseClickPos1, false);
                    removeTile(MouseClickPos2);

                    // removing a tile means redo is impossible without
                    // a further undo.
                    Game->allow_redo=false;
                    demoModeChanged(false);
                    drawTileNumber();

                    // if no tiles are left, the player has `won`, so celebrate
                    if( Game->TileNum == 0 )
                    {
                        gameOver(Game->MaxTileNum,cheatsUsed);
                    }
                    // else if no more moves are possible, display the sour grapes dialog
                    else if( ! Game->findMove( TimerPos1, TimerPos2 ) )
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
                MouseClickPos1.e = Game->m_depth;     // mark tile position as invalid
                MouseClickPos2.e = Game->m_depth;
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
    int E,X,Y;

    TileSprite * clickedItem = NULL;
    clickedItem = (TileSprite *) itemAt(point);
    if (!clickedItem) {
	//no item under mouse
	qDebug() << "no tile registered";
	return;
    }

    TileCoord coord = spriteMap.key(clickedItem);
    if (coord.isNull()) {
	qDebug() << "null tile coordinates, clicked on background?";
	return;
    }

    E = coord.z();
    X = coord.x();
    Y = coord.y();

    //sanity checking
    if (X<0 || X>= Game->m_width || Y<0 || Y>= Game->m_height) return;

    // if gameboard is empty, skip 
    //sanity checking
    if ( ! Game->BoardData(E,Y,X) ) {
	qDebug() << "Tile not in BoardData. Fading out?";
	return;
    }

    // tile must be 'free' (nothing left, right or above it)
    //Optimization, skip "over" test for the top layer
    if( E < Game->m_depth-1 )
    {
        if( Game->BoardData(E+1,Y,X) || (Y< Game->m_height-1 && Game->BoardData(E+1,Y+1,X)) ||
            (X< Game->m_width-1 && Game->BoardData(E+1,Y,X+1)) ||
	    (X< Game->m_width-1 && Y< Game->m_height-1 && Game->BoardData(E+1,Y+1,X+1)) )
            return;
    }

    // No left test on left edge
    if (( X > 0) && (Game->BoardData(E,Y,X-1) || Game->BoardData(E,Y+1,X-1))) {
	if ((X< Game->m_width-2) && (Game->BoardData(E,Y,X+2) || Game->BoardData(E,Y+1,X+2))) {
            return;
	}
    }

    // if we reach here, position is legal
    MouseClickPos.e = E;
    MouseClickPos.y = Y;
    MouseClickPos.x = X;
    MouseClickPos.f = Game->BoardData(E,Y,X);
    // give visible feedback
    hilightTile( MouseClickPos );
}

// ---------------------------------------------------------
bool BoardWidget::loadBoard( )
{
    if (Game) delete Game;
    Game = new GameData(&theBoardLayout);
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
    //if( ! theBackground.load( pszFileName, requiredWidth(), requiredHeight()) )
if( ! theBackground.load( pszFileName, requiredWidth(), requiredHeight()) )
    {
        if( bShowError )
            KMessageBox::sorry(this, i18n("Failed to load image:\n%1", pszFileName) );
        return( false );
    }
    Prefs::setBackground(pszFileName);
    Prefs::writeConfig();
    return true;
}

// ---------------------------------------------------------
void BoardWidget::drawTileNumber()
{
    emit tileNumberChanged( Game->MaxTileNum, Game->TileNum, Game->moveCount( ) );
}

// ---------------------------------------------------------
void BoardWidget::cancelUserSelectedTiles()
{
    if( MouseClickPos1.e != Game->m_depth )
    {
        hilightTile( MouseClickPos1, false ); // redraw tile
        MouseClickPos1.e = Game->m_depth;    // mark tile invalid
    }
}

// ---------------------------------------------------------
bool BoardWidget::loadTileset(const QString &path) {

  if (theTiles.loadTileset(path)) {
    Prefs::setTileSet(path);
    Prefs::writeConfig();
    resizeTileset(size());
    return true;
  } else {
    if (theTiles.loadDefault()) {
      Prefs::setTileSet(theTiles.path());
      Prefs::writeConfig();
      resizeTileset(size());
      return false;
    } else {
      return false;
    }
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

int BoardWidget::requiredHorizontalCells()
{
	int res = (Game->m_width/2);
	/*if (Prefs::showRemoved()) 
		res = res + 3; //space for removed tiles*/
	return res;
}

int BoardWidget::requiredVerticalCells()
{
	int res = (Game->m_height/2);
	return res;
}

// calculate the required window width (board + removed tiles)
int BoardWidget::requiredWidth() {
	//int res = ((BoardLayout::width+12)* theTiles.qWidth());
	int res = width();
	return res;
}

// calculate the required window height (board + removed tiles)
int BoardWidget::requiredHeight() {

	//int res = ((BoardLayout::height+3)* theTiles.qHeight());
	int res = height();
	return res;
}

void BoardWidget::angleSwitchCCW() {
	switch (m_angle) 
	{
	    case NW : 
		m_angle = NE;
		break;
	    case NE : 
		m_angle = SE;
		break;
	    case SE : 
		m_angle = SW;
		break;
	    case SW : 
		m_angle = NW;
		break;
	}
	
	QHashIterator<TileCoord, TileSprite *> i(spriteMap);
 	while (i.hasNext()) {
     	    i.next();
	    QPixmap u = theTiles.unselectedTile(m_angle);
	    QPixmap s = theTiles.selectedTile(m_angle);
     	    i.value()->setAngle(m_angle, u, s  );
 	}
	//re-position and re-layer
	updateSpriteMap();
}

void BoardWidget::angleSwitchCW() {
	switch (m_angle) 
	{
	    case NW : 
		m_angle = SW;
		break;
	    case NE : 
		m_angle = NW;
		break;
	    case SE : 
		m_angle = NE;
		break;
	    case SW : 
		m_angle = SE;
		break;
	}
	
	QHashIterator<TileCoord, TileSprite *> i(spriteMap);
 	while (i.hasNext()) {
     	    i.next();
     	    QPixmap u = theTiles.unselectedTile(m_angle);
	    QPixmap s = theTiles.selectedTile(m_angle);
     	    i.value()->setAngle(m_angle, u, s  );
 	}
	//re-position and re-layer
	updateSpriteMap();
}
// shuffle the remaining tiles around, useful if a deadlock ocurrs
// this is a big cheat so we penalise the user.
void BoardWidget::shuffle() {
	int count = 0;
	// copy positions and faces of the remaining tiles into
	// the pos table
	for (int e=0; e<Game->m_depth; e++) {
	    for (int y=0; y<Game->m_height; y++) {
		for (int x=0; x<Game->m_width; x++) {
		    if (Game->BoardData(e,y,x) && Game->MaskData(e,y,x) == '1') {
			Game->PosTable[count].e = e;
			Game->PosTable[count].y = y;
			Game->PosTable[count].x = x;
			Game->PosTable[count].f = Game->BoardData(e,y,x);
			count++;
		    }
		}
	    }

	}


	// now lets randomise the faces, selecting 400 pairs at random and
	// swapping the faces.
	for (int ran=0; ran < 400; ran++) {
		int pos1 = Game->random.getLong(count);
		int pos2 = Game->random.getLong(count);
		if (pos1 == pos2)
			continue;
		BYTE f = Game->PosTable[pos1].f;
		Game->PosTable[pos1].f = Game->PosTable[pos2].f;
		Game->PosTable[pos2].f = f;
	}

	// put the rearranged tiles back.
	for (int p=0; p<count; p++)
		Game->putTile(Game->PosTable[p]);


	// force a full redraw
	populateSpriteMap();

	// I consider this s very bad cheat so, I punish the user
	// 300 points per use
	cheatsUsed += 15;
	drawTileNumber();
}


#include "boardwidget.moc"
