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
#include <qapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <KDebug>

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
    m_angle = (TileViewAngle) Prefs::angle();

    // Load layout first
    loadBoardLayout(Prefs::layout());

    //Initialize our Game structure
    Game = new GameData(theBoardLayout.board());

    MouseClickPos1.e = Game->m_depth;     // mark tile position as invalid
    MouseClickPos2.e = Game->m_depth;

    //Animation timers
    animateForwardTimer = new QTimer(this);
    animateForwardTimer->setSingleShot(true);
    animateForwardTimer->setInterval(100);
    connect(animateForwardTimer, SIGNAL(timeout()), SLOT(animatingMoveListForward()));
    animateForwardTimer->stop();
    
    animateBackwardsTimer = new QTimer(this);
    animateBackwardsTimer->setSingleShot(true);
    animateBackwardsTimer->setInterval(100);
    connect(animateBackwardsTimer, SIGNAL(timeout()), SLOT(animatingMoveListBackwards()));
    animateBackwardsTimer->stop();
    
    loadSettings();
}

BoardWidget::~BoardWidget(){
  if (Game) delete Game;
}

void BoardWidget::loadSettings(){
    // Load tileset. First try to load the last use tileset

    if (!loadTileset(Prefs::tileSet())){
      kDebug() << "An error occurred when loading the tileset" << Prefs::tileSet() <<"KMahjongg will continue with the default tileset.";
    }

    // Load background
    if( ! loadBackground(Prefs::background(), false ) )
    {
      kDebug() << "An error occurred when loading the background" << Prefs::background() <<"KMahjongg will continue with the default background.";
    }
    setShowMatch( Prefs::showMatchingTiles() );
    
    if (QString::compare(Prefs::layout(), theBoardLayout.path(), Qt::CaseSensitive)!=0) {
      //TODO: WARN USER HERE ABOUT DESTRUCTIVE OPERATION!!!
      loadBoardLayout(Prefs::layout());
      calculateNewGame();
    }
    setDisplayedWidth();
    drawBoard(true);
    //Store our updated settings, some values might have been changed to defaults
    saveSettings();
}

void BoardWidget::resizeEvent ( QResizeEvent * event )
{
    if (event->spontaneous()) return;
    resizeTileset(event->size());
    theBackground.sizeChanged(requiredWidth(), requiredHeight());
    drawBoard(true);
}

void BoardWidget::resizeTileset ( const QSize & wsize )
{
    QSize newtiles = theTiles.preferredTileSize(wsize, requiredHorizontalCells(), requiredVerticalCells());

    theTiles.reloadTileset(newtiles);
    stopMatchAnimation();
}



void BoardWidget::saveSettings(){
  Prefs::setTileSet(theTiles.path());
  Prefs::setLayout(theBoardLayout.path());
  Prefs::setBackground(theBackground.path());
  Prefs::setAngle(m_angle);
  Prefs::self()->writeConfig();
}

void BoardWidget::setDisplayedWidth() {
  //TODO remove method and let the layout handle it
  //for now we just force our resizeEvent() to be called
  resize(width() , height());
}

void BoardWidget::populateSpriteMap() {
    //Delete previous sprites (full update), synchronize state with GameData
    while (!items()->isEmpty())
	delete items()->first();

    //Clear our spritemap as well
    spriteMap.clear();

    //Recreate our background
    QPalette palette;
    palette.setBrush( backgroundRole(), theBackground.getBackground() );
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
	xOffset += theTiles.levelOffsetX()/2;
        yOffset += theTiles.levelOffsetY()/2;

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
		xOffset +=theTiles.levelOffsetX();
		yOffset -=theTiles.levelOffsetY();
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
	xOffset -= theTiles.levelOffsetX()/2;
        yOffset += theTiles.levelOffsetY()/2;
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
		xOffset -=theTiles.levelOffsetX();
		yOffset -=theTiles.levelOffsetY();
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
	xOffset -= theTiles.levelOffsetX()/2;
        yOffset -= theTiles.levelOffsetY()/2;
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
		xOffset -=theTiles.levelOffsetX();
		yOffset +=theTiles.levelOffsetY();
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
	xOffset += theTiles.levelOffsetX()/2;
        yOffset -= theTiles.levelOffsetY()/2;

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
		xOffset +=theTiles.levelOffsetX();
		yOffset +=theTiles.levelOffsetY();
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

void BoardWidget::pause() {
	gamePaused = !gamePaused;
	drawBoard(!gamePaused);
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
        populateSpriteMap();
	drawBoard(true);
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
          hilightTile(Game->getFromPosTable(Pos), true);
        }
    }
    else
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {
          hilightTile(Game->getFromPosTable(Pos), false);
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
      hilightTile(Game->getFromPosTable(Pos), false);
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
        animateForwardTimer->start(); //it is a single shot timer
    } else {
        //start removal
      animateBackwardsTimer->start(); //it is a single shot timer
    }
}

void BoardWidget::animatingMoveListBackwards()
{
    if (Game->TileNum > 0 ) {
        // remove all tiles
        removeTile(Game->MoveListData(Game->TileNum-1), false);
        removeTile(Game->MoveListData(Game->TileNum-1));
        drawTileNumber();
        animateBackwardsTimer->start(); //it is a single shot timer
    } else {
        //end of animation
        stopEndAnimation();
    }
}

void BoardWidget::stopEndAnimation()
{
  animateForwardTimer->stop();
  animateBackwardsTimer->stop();
}

// ---------------------------------------------------------
void BoardWidget::calculateNewGame( int gNumber)
{
    cancelUserSelectedTiles();
    stopMatchAnimation();
    stopEndAnimation();
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
    Game->generateTilePositions();

    // Now use the tile position data to generate tile dependency data.
    // We only need to do this once for each new game.
    Game->generatePositionDepends();

    // Now try to position tiles on the board, 64 tries max.
    for( short nr=0; nr<64; nr++ )
    {
        if( Game->generateStartPosition2() )
        {
            drawBoard(true);
            setStatusText( i18n("Ready. Now it is your turn.") );
            cheatsUsed=0;
            emit gameCalculated();
            return;
        }
    }

    drawBoard(false);
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
		atile->setSelected(true);
	} else {
		Game->setHighlightData(Pos.e,Pos.y,Pos.x,0);
		if (atile)
		atile->setSelected(false);
	}
}



// ---------------------------------------------------------
void BoardWidget::drawBoard(bool showTiles)
{
   if (gamePaused) showTiles = false;
   if (showTiles) {
    populateSpriteMap();
    drawTileNumber();
   } else {
     //Delete previous sprites
     while (!items()->isEmpty())
       delete items()->first();

    //Clear our spritemap as well
     spriteMap.clear();

    //Recreate our background
     QPalette palette;
     palette.setBrush( backgroundRole(), theBackground.getBackground() );
     setPalette( palette );
     setAutoFillBackground (true);
   }
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
    thissprite->show();
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
/*
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
    Game = new GameData(theBoardLayout.board());
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
  if (theBackground.load( pszFileName, requiredWidth(), requiredHeight())) {
    if (theBackground.loadGraphics()) {
      return true;
    }
  } 
  //Try default
    if (theBackground.loadDefault()) {
      if (theBackground.loadGraphics()) {
      }
    } 
    return false;
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
    if (theTiles.loadGraphics()) {
      resizeTileset(size());
      return true;
    }
  } 
  //Tileset or graphics could not be loaded, try default
  if (theTiles.loadDefault()) {
    if (theTiles.loadGraphics()) {
    resizeTileset(size());
    }
  } 
  return false;
}

bool BoardWidget::loadBoardLayout(const QString &file) {
  if (theBoardLayout.load(file)) {
    return true;
  } else {
    if (theBoardLayout.loadDefault()) {
      return false;
    } else {
      return false;
    }
  }
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
	//save angle
	saveSettings();
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
	//save settings
	saveSettings();
}
// shuffle the remaining tiles around, useful if a deadlock ocurrs
// this is a big cheat so we penalise the user.
void BoardWidget::shuffle() {
  Game->shuffle();
  // force a full redraw
  populateSpriteMap();

  // I consider this s very bad cheat so, I punish the user
  // 300 points per use
  cheatsUsed += 15;
  drawTileNumber();
}

QString  BoardWidget::getLayoutName() {
    QString key("Name");
    return theBoardLayout.authorProperty(key);
}


#include "boardwidget.moc"
