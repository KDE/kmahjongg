/*

    kmahjongg, the classic mahjongg game for KDE project

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no 

    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.    

*/

#include <qmsgbox.h>
#include <qtimer.h> 
#include <qaccel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kmahjongg.moc"
#include "version.h"

//----------------------------------------------------------
// Defines
//----------------------------------------------------------
#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2

#define ANIMSPEED    200

// tiles symbol names:
#define TILE_OFFSET      2

#define TILE_SERIENINE1  ( 0 + TILE_OFFSET)
#define TILE_SERIENINE2  ( 9 + TILE_OFFSET)
#define TILE_SERIENINE3  (18 + TILE_OFFSET)
#define TILE_SEASON      (27 + TILE_OFFSET)
#define TILE_DIRECTION   (31 + TILE_OFFSET)
#define TILE_DRAGON      (36 + TILE_OFFSET)
#define TILE_FLOWER      (39 + TILE_OFFSET)

#define TX           40              // Tile width
#define TY           56              // Tile heigth
#define TSOX         4               // Tile shadow x offset
#define TSOY         4               // Tile shadow y offset
#define TX4          ((TX-TSOX)/2)   // Tile quarter width
#define TY4          ((TY-TSOY)/2)   // Tile quarter height
#define SX           (XMax*TX4)      // screen width
#define SY           (YMax*TY4)      // screen height
#define SYOFFSET     TSOY            // screen Y offset
 
#define ID_GAME_NEW_START        10
#define ID_GAME_NEW_END          30
#define ID_GAME_NEW              101
#define ID_GAME_HELP             102
#define ID_GAME_DEMO             103
#define ID_GAME_UNDO             104
#define ID_GAME_SHOW             105
#define ID_GAME_QUIT             106

#define ID_VIEW_BACKGROUND_LOAD  201
#define ID_VIEW_STATUSBAR        202

#define CONFIG_STATUSBAR_VIS     "StatusBar_visible"

// use this macro, to mark strings as candidates for translation
// without generating code
#undef i18n
#define i18n( msg)              msg


//----------------------------------------------------------
// STATICS
//----------------------------------------------------------
const char DEFAULTTILESET[]    = "kmahjongg.bmp";
const char DEFAULTBACKGROUND[] = "kmahjongg_bgnd.bmp";

extern char ClassicMask[];
extern char TowerMask[];
extern char PyramideMask[];
extern char TriangleMask[];

BOARDINFO BoardWidget::BoardInfo[] = {
  { i18n( "Classic" ),  ClassicMask  },
  { i18n( "Tower"),     TowerMask    },
  { i18n( "Triangle"),  TriangleMask },
  { i18n( "Pyramide"),  PyramideMask },
  { NULL,       NULL     }
};

KLocale* locale;


// ---------------------------------------------------------
int main( int argc, char** argv )
{
    KApplication a( argc, argv, "kmahjongg");
    locale = a.getLocale();

    KMahjonggWidget w;

    w.show();

    return( a.exec() );
}


// ---------------------------------------------------------
/**
    Constructor.
*/
KMahjonggWidget::KMahjonggWidget()
{
    setCaption( kapp->getCaption() );

    // read configuration
    KConfig* conf = kapp->getConfig();
    conf->setGroup("");
    bShowStatusbar = conf->readNumEntry( CONFIG_STATUSBAR_VIS, TRUE );

    setupStatusBar();
    setupMenuBar();

    // init board widget
    bw = new BoardWidget( this );
    bw->move( 0, pMenuBar->height()+1 );
    setView( bw );

    bDemoModeActive = false;
    bShowMatchingTiles = false;

    updateStatusbar( bShowStatusbar );

    connect( bw, SIGNAL( statusTextChanged(const char*) ),
                 SLOT( showStatusText(const char*) ) );

    connect( bw, SIGNAL( message(const char*) ),
                 SLOT( showMessage(const char*) ) );

    connect( bw, SIGNAL( tileNumberChanged(int,int) ),
                 SLOT( showTileNumber(int,int) ) );

    connect( bw, SIGNAL( demoModeChanged(bool) ),
                 SLOT( demoModeChanged(bool) ) );

    startNewGame( 0 );
}

// ---------------------------------------------------------
KMahjonggWidget::~KMahjonggWidget()
{
    delete pStatusBar;
    delete pMenuBar;
}

// ---------------------------------------------------------
void KMahjonggWidget::setupStatusBar()
{
    pStatusBar = new KStatusBar( this );

    pStatusBar->insertItem( "0000/0000", ID_STATUS_TILENUMBER );
    pStatusBar->insertItem( "",          ID_STATUS_MESSAGE    );
    pStatusBar->changeItem( "",          ID_STATUS_TILENUMBER );
    pStatusBar->setInsertOrder( KStatusBar::RightToLeft );
    pStatusBar->setAlignment( ID_STATUS_TILENUMBER, AlignCenter );
    pStatusBar->show();
    setStatusBar( pStatusBar );
}

// ---------------------------------------------------------
void KMahjonggWidget::updateStatusbar( bool bShow )
{
    pMenuBar->setItemChecked( ID_VIEW_STATUSBAR, bShow );
    enableStatusBar( bShow ? KStatusBar::Show : KStatusBar::Hide );

    setFixedSize( bw->size() + 
                  QSize( 2, (bShow ? pStatusBar->height() : 0)
                       + 2 + pMenuBar->height() ) );
}

// ---------------------------------------------------------
void KMahjonggWidget::setupMenuBar()
{
    // Menu IDs from ID_GAME_NEW_START ... ID_GAME_NEW_END
    // are reserved for new game index numbers
    QPopupMenu* board = new QPopupMenu;
    for( int i=0; BoardWidget::BoardInfo[i].pszName; i++ )
    {
        board->insertItem( locale->translate(BoardWidget::BoardInfo[i].pszName),
                           ID_GAME_NEW_START + i );
    }
    QPopupMenu *game = new QPopupMenu;
    game->setCheckable( true );
    game->insertItem( locale->translate("&Start new game"),    board, ID_GAME_NEW );
    game->insertSeparator();
    game->insertItem( locale->translate("&Help me"),           ID_GAME_HELP );
    game->insertItem( locale->translate("&Undo last move"),    ID_GAME_UNDO );
    game->insertItem( locale->translate("&Demo mode"),         ID_GAME_DEMO );
    game->insertItem( locale->translate("Show &matching tiles"), ID_GAME_SHOW );
    game->insertSeparator();
    game->insertItem( locale->translate("&Quit"),              ID_GAME_QUIT );

    QPopupMenu *view = new QPopupMenu;
    view->setCheckable( true );
    view->insertItem( locale->translate("&Load Background image..."),  ID_VIEW_BACKGROUND_LOAD );
    view->insertSeparator( -1 );
    view->insertItem( locale->translate("&Statusbar"),   ID_VIEW_STATUSBAR );

    QString about;
    about.sprintf(locale->translate("Mahjongg %s\n\nby Mathias Mueller (in5y158@public.uni-hamburg.de)"), KMAHJONGG_VERSION);
    QPopupMenu *help = kapp->getHelpMenu(true, about);
 
    pMenuBar = new KMenuBar( this );

    pMenuBar->insertItem( locale->translate("&Game"), game );
    pMenuBar->insertItem( locale->translate("&View"), view );
    pMenuBar->insertSeparator();
    pMenuBar->insertItem( locale->translate("&Help"), help );

    // initialize menu accelerators
    pMenuBar->setAccel( CTRL+Key_Q, ID_GAME_QUIT );
    pMenuBar->show();

    connect( pMenuBar,  SIGNAL(activated(int) ), SLOT( menuCallback(int) ) );
    setMenu( pMenuBar );
}

// ---------------------------------------------------------
void KMahjonggWidget::menuCallback( int item )
{
    // Menu IDs from ID_GAME_NEW_START ... ID_GAME_NEW_END
    // are reserved for new game index numbers
    if( item < ID_GAME_NEW_END && item >= ID_GAME_NEW_START )
    {
        startNewGame( item-ID_GAME_NEW_START );
    }
    else  switch( item )
    {
        case ID_GAME_QUIT:
	{
	    // write configuration
	    KConfig* conf = kapp->getConfig();
	    conf->setGroup("");
	    conf->writeEntry(CONFIG_STATUSBAR_VIS, bShowStatusbar );

            kapp->quit();
            break;
	}
        case ID_GAME_HELP:
            bw->helpMove();
            break;

        case ID_GAME_UNDO:
            bw->undoMove();
            break;

        case ID_GAME_DEMO:
            if( bDemoModeActive )
                bw->stopDemoMode();
            else
                bw->startDemoMode();
            break;

        case ID_GAME_SHOW:
        {
            bShowMatchingTiles = ! bShowMatchingTiles;
            bw->setShowMatch( bShowMatchingTiles );
            pMenuBar->setItemChecked( ID_GAME_SHOW, bShowMatchingTiles );
            break;
        }

        case ID_VIEW_BACKGROUND_LOAD:
	{
            QString strFile = QFileDialog::getOpenFileName( NULL, NULL, this );
            if( ! strFile.isEmpty() )
	    {
                if( bw->loadBackground( strFile ) )
                    bw->drawBoard();
            }
            break;
        }

        case ID_VIEW_STATUSBAR:
            bShowStatusbar = !bShowStatusbar;
            updateStatusbar( bShowStatusbar );
            break;
    }
}

// ---------------------------------------------------------
void KMahjonggWidget::startNewGame( int item )
{
    if( ! bDemoModeActive )
        bw->calculateNewGame( item );
}

// ---------------------------------------------------------
void KMahjonggWidget::closeEvent( QCloseEvent* e )
{
    kapp->quit();
    e->accept();
}

// ---------------------------------------------------------
void KMahjonggWidget::showStatusText( const char* msg )
{
    pStatusBar->changeItem( locale->translate(msg), ID_STATUS_MESSAGE );
}

// ---------------------------------------------------------
void KMahjonggWidget::showMessage( const char* msg )
{
    QMessageBox::message( NULL, locale->translate(msg), NULL, this );
}

// ---------------------------------------------------------
void KMahjonggWidget::showTileNumber( int iMaximum, int iCurrent )
{
    char szBuffer[16];
    sprintf( szBuffer, "%d/%d", iCurrent, iMaximum );
    pStatusBar->changeItem( szBuffer, ID_STATUS_TILENUMBER );
    // update undo menu item, if demomove is inactive 
    if( ! bDemoModeActive )
    {
        pMenuBar->setItemEnabled( ID_GAME_UNDO, iMaximum != iCurrent );
    }
}

// ---------------------------------------------------------
void KMahjonggWidget::demoModeChanged( bool bActive )
{
    bDemoModeActive = bActive;

    pMenuBar->setItemChecked( ID_GAME_DEMO, bActive );
    pMenuBar->setItemEnabled( ID_GAME_NEW,  !bActive );
    pMenuBar->setItemEnabled( ID_GAME_HELP, !bActive );
    pMenuBar->setItemEnabled( ID_GAME_UNDO, !bActive );
    pMenuBar->setItemEnabled( ID_GAME_SHOW, !bActive );
}



// ---------------------------------------------------------
/**
    Constructor.
    Loads tileset and background bitmaps.
*/
BoardWidget::BoardWidget( QWidget* parent )
  : QWidget( parent )
{
    setBackgroundColor( QColor( 0,0,0 ) );

    TimerState = Stop;
    iTimerStep = 0;
    iCurrentBoard = 0;
    matchCount = 0;
    showMatch = false;
    MouseClickPos1.e = EMax;     // mark tile position as invalid 
    MouseClickPos2.e = EMax;

    // Load tileset
    if( ! loadTileset( DEFAULTTILESET ) )
    {
        QString str;
        str.sprintf( "%s/kmahjongg/pics/%s", 
		     kapp->kde_datadir().data(), DEFAULTTILESET );
        if( ! loadTileset( str ) )
	{
            showMessage( i18n("Unable to open bitmap file !") );
	    fatal( "Unable to open bitmap file !" );
	}
    }

    // Load background
    Bgnd = NULL;
    if( ! loadBackground( DEFAULTBACKGROUND, false ) )
    {
        QString str;
	str.sprintf( "%s/kmahjongg/pics/%s", 
                     kapp->kde_datadir().data(), DEFAULTBACKGROUND );
        if( ! loadBackground( str ) )
	{
            showMessage( i18n("Unable to open bitmap file !") );
	    fatal( "Unable to open bitmap file !" );
	}
    }

    setFixedSize( SX, SY-SYOFFSET );

    show();
}

// ---------------------------------------------------------
void BoardWidget::paintEvent( QPaintEvent* pa )
{
    QPixmap  pm;

    int x = pa->rect().left();
    int height = pa->rect().height();
    int width  = pa->rect().width();

    QImage   qi( width, height, 8, 256 );
    for( int c=0; c<256; c++ )
    {  
        qi.setColor( c, qiScreen.color( c ) );
    }

    for( int y=0; y<height; y++ )
    {
        int yScreenScanLine = pa->rect().top() + y + SYOFFSET;
        // if scanline of screen is out of range, skip
        if( yScreenScanLine >= SY )  break;

        memcpy( qi.scanLine( y ), 
                qiScreen.scanLine( yScreenScanLine ) + x,
                width );
    }
    if( pm.convertFromImage( qi ) )
    {
        bitBlt( this, x, pa->rect().top(),
                &pm, 0,0, width, height, CopyROP );
    }       
}

// ---------------------------------------------------------
void BoardWidget::undoMove()
{
    cancelUserSelectedTiles();

    if( Game.TileNum < Game.MaxTileNum )
    {
        putTile( Game.MoveList[Game.TileNum] );
        Game.TileNum++;
        putTile( Game.MoveList[Game.TileNum] );
        Game.TileNum++;
        drawTileNumber();
        setStatusText( i18n("Undo operation done successfully.") );
    }
    else
        setStatusText( i18n("What do you want to undo? You have done nothing!") );
}

// ---------------------------------------------------------
void BoardWidget::helpMove()
{
    cancelUserSelectedTiles();
    stopMatchAnimation();

    if( findMove( TimerPos1, TimerPos2 ) )
    {
        iTimerStep = 1;
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
        hilightTile( TimerPos1 );
        hilightTile( TimerPos2 );
    }
    else
    {
        drawTile( TimerPos1 );
        drawTile( TimerPos2 );
    }
    // restart timer
    if( iTimerStep++ < 8 )
        QTimer::singleShot( ANIMSPEED, this, SLOT( helpMoveTimeout() ) );
}

// ---------------------------------------------------------
void BoardWidget::startDemoMode()
{
    cancelUserSelectedTiles();
    stopMatchAnimation();

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
    setStatusText( i18n("Now it's you again.") );
    emit demoModeChanged( false );
}
// ---------------------------------------------------------
void BoardWidget::demoMoveTimeout()
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
                        putTile( Game.MoveList[Game.TileNum] );
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
            if( TimerState == Demo ) 
	    {
                hilightTile( TimerPos1 );
                hilightTile( TimerPos2 );
            }
            break;
	
        case 2:
        case 4:
            drawTile( TimerPos1 );
            drawTile( TimerPos2 );              
            break;
	// remove matching tiles from game board
        case 5:
            removeTile( TimerPos1 );
            removeTile( TimerPos2 );              
            drawTileNumber();
            break;
    }
    // restart timer if demo mode is still active
    if( TimerState == Demo )
        QTimer::singleShot( ANIMSPEED, this, SLOT( demoMoveTimeout() ) );
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
            hilightTile(PosTable[Pos]);
        }
    }
    else
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {
            drawTile(PosTable[Pos]);
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
        drawTile(PosTable[Pos]);
    }
    TimerState = Stop;
    matchCount = 0;
}

// ---------------------------------------------------------
void BoardWidget::animateMoveList()
{
    setStatusText( i18n("Congratulations. You have won!") );

    while( Game.TileNum < Game.MaxTileNum )
    {
        // put back all tiles
        putTile(Game.MoveList[Game.TileNum]);
        Game.TileNum++;
        putTile(Game.MoveList[Game.TileNum]);
        Game.TileNum++;
        drawTileNumber();
    }
    while( Game.TileNum > 0 )
    {
        // remove all tiles
        removeTile(Game.MoveList[Game.TileNum-1]);
        removeTile(Game.MoveList[Game.TileNum-1]);
        drawTileNumber();
    }

    calculateNewGame();
}

// ---------------------------------------------------------
void BoardWidget::calculateNewGame( int iBoard )
{
    cancelUserSelectedTiles();
    setStatusText( i18n("Calculating new game...") );

    if( iBoard>=0 )  iCurrentBoard = iBoard;

    if( !loadBoard( BoardInfo[iCurrentBoard].pszGameMask ) )
    {
        setStatusText( i18n("Error converting board information!") );
        return;
    }

    srandom( time(NULL) );

    // try max. 64 times
    for( short nr=0; nr<64; nr++ )
    {
        if( generateStartPosition() )
        {
            drawBoard();
            setStatusText( i18n("Ready. Now it's your turn.") );
            return;
        }
    }

    drawBoard();
    setStatusText( i18n("Error generating new game!") );
}

// ---------------------------------------------------------
void BoardWidget::findFreePositions( POSITION& Pos )
{
    static const short PosOffsets[] = {
        0,-1,-2,  0,0,-2,  0,1,-2,  0,-1,2,  0,0,2,  0,1,2,
        0,-2,-1,  0,-2,0,  0,-2,1,  0,2,-1,  0,2,0,  0,2,1,
        1,-1,-1,  1,0,-1,  1,1,-1,  1,-1,0,  1,0,0,  1,1,0,
        1,-1, 1,  1,0, 1,  1,1, 1,  EMax };

    for( const short* PosOffset = PosOffsets; PosOffset[0] < EMax; PosOffset += 3 )
    {
        short E = Pos.e + PosOffset[0];
        short Y = Pos.y + PosOffset[1];
        short X = Pos.x + PosOffset[2];

        if( Y<YBorder || Y>=YMax-YBorder || X<XBorder || X>=XMax-XBorder ) continue;
        if( Game.Board[E][Y][X] || Game.Mask[E][Y][X] != (UCHAR)'1' ) continue;

        if (E > 0)
        {
            if( Game.Board[E-1][Y][X]     <= 1
             || Game.Board[E-1][Y+1][X]   <= 1
             || Game.Board[E-1][Y+1][X+1] <= 1
             || Game.Board[E-1][Y][X+1]   <= 1 ) continue;
        }
        // clip right border
        if( X < XMax-XBorder-2 )
        {
            short X2 = X+2;
            if (!((Game.Board[E][Y][X2]>1 || Game.Mask[E][Y][X2]==0)
               && (Game.Board[E][Y+1][X2]>1 || Game.Mask[E][Y+1][X2]==0)))
            {
                while (X2<XMax-XBorder)
                {
                    if( Game.Board[E][Y][X2] || Game.Board[E][Y+1][X2] )
                        X2 = 2*XMax;  // illegal
                    else X2 += 2;
                }
                if( X2==2*XMax )
                    continue;  // illegal
            }
        }
        if( X >= XBorder+2 )
        {
            // ist nicht am Rand, also testen
            short X2 = X-2;

            if (!((Game.Board[E][Y][X2]>1 || Game.Mask[E][Y][X2]==0) &&
                  (Game.Board[E][Y+1][X2]>1 || Game.Mask[E][Y+1][X2]==0)))
            {
                while (X2>=XBorder)
                {
                    if( Game.Board[E][Y][X2] || Game.Board[E][Y+1][X2] )
                        X2 =- XMax;  // illegal
                    else
                        X2 -= 2;
                }
                if( X2==-XMax )
                    continue;  // illegal
            }
        }
        PosTable[iPosCount].e = E;
        PosTable[iPosCount].y = Y;
        PosTable[iPosCount].x = X;
        iPosCount++;
        // mark position in gameboard
        Game.putTile( E, Y, X, 1 );
    }
}


// ---------------------------------------------------------
/**
    Generate new game by random.
    To make sure, that each puzzle is solveable, matching pairs
    of tiles are put on positions, which are currently free.  
    It is possible, that a dead lock situation is reached.

    @return Success indicator
*/
bool BoardWidget::generateStartPosition()
{
    UCHAR TilePair[FMax+144];
    short Pos1, Pos2;
    short nr;

    // insert all tiles in table;
    for( int i=0; i<FMax; )
    {
        for( nr=0; nr<9*4; nr++)    TilePair[i++] = TILE_SERIENINE1+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)    TilePair[i++] = TILE_SERIENINE2+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)    TilePair[i++] = TILE_SERIENINE3+(nr/4); // 4*9 Tiles
        for( nr=0; nr<4;   nr++)    TilePair[i++] = TILE_FLOWER+nr;         // 4 Tiles
        for( nr=0; nr<4;   nr++)    TilePair[i++] = TILE_SEASON+nr;         // 4 Tiles
        for( nr=0; nr<4*4; nr++)    TilePair[i++] = TILE_DIRECTION+(nr/4);  // 4*4 Tiles
        for( nr=0; nr<3*4; nr++)    TilePair[i++] = TILE_DRAGON+(nr/4);     // 3*4 Tiles
    }

    Game.TileNum = Game.MaxTileNum;

    // clear board
    memset( Game.Board, 0, sizeof(Game.Board) );

    // find first two position
    iPosCount=0;
    for (short Y=YBorder; Y<YMax-YBorder; Y++)
    {
        for (short X=XBorder; X<XMax-XBorder; X++)
        {
            if (Game.Mask[0][Y][X] == (UCHAR) '1')
            {
                PosTable[iPosCount].e = 0;
                PosTable[iPosCount].y = Y;
                PosTable[iPosCount++].x = X;
            }
        }
    }
    if( iPosCount == 0 )
        return( false );  // Error: no start position.

    // try max. 200.
    for( nr=200; nr>0; nr-- )
    {
        Pos1 = random() % iPosCount;
        Pos2 = random() % iPosCount;
        // distance must be min. 4
        if( abs(PosTable[Pos1].y - PosTable[Pos2].y) >= 4 )
            break;
    }
    if( nr==0 )
        return( false );    // Error: no start position.

    // Pos1 *has to be* smaller than Pos2 .
    if( Pos1 > Pos2 )
    {
        short PosBB = Pos1;
        Pos1 = Pos2; 
        Pos2 = PosBB;
    }
    PosTable[0] = PosTable[Pos1];     
    PosTable[1] = PosTable[Pos2];

    iPosCount=2;

    // put tiles on board, while tiles are left
    while( Game.TileNum>0 )
    {  
        if( iPosCount<2 )
            return( false ); // Error: 

        short Figur = (random() % Game.TileNum) & -2;   // tile number
        do
        {
            Pos1 = random() % iPosCount;
            Pos2 = random() % iPosCount;
        } while( Pos1 == Pos2 );

        PosTable[Pos1].f = TilePair[Figur];
        Game.putTile( PosTable[Pos1] );

        PosTable[Pos2].f = TilePair[Figur+1];
        Game.putTile( PosTable[Pos2] );

        Game.TileNum -= 2;
        TilePair[Figur]   = TilePair[Game.TileNum];
        TilePair[Figur+1] = TilePair[Game.TileNum+1];

        findFreePositions( PosTable[Pos1] );
        findFreePositions( PosTable[Pos2] );

        // remove position from pos table
        // Pos1 *before* Pos2.
        if( Pos1 < Pos2 )
        {
            short PosBB = Pos1;
            Pos1 = Pos2;
            Pos2 = PosBB;
        }
        PosTable[Pos1] = PosTable[--iPosCount];
        PosTable[Pos2] = PosTable[--iPosCount];
    }

    // game generated successfully
    Game.TileNum = Game.MaxTileNum;
    return( true );     
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

    for( short E=0; E<EMax; E++ )
    {
        for( short Y=1; Y<YMax-1; Y++ )
        {
            for( short X=1; X<XMax-1; X++ )
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

    PosTable[0].e = EMax;  // 1. Paar noch nicht gefunden
    iPosCount = 0;  // Hier Anzahl der gefunden Paare merken

    while( Pos_Ende < Game.MaxTileNum-1 )
    {
        for( short Pos = Pos_Ende+1; Pos < Game.MaxTileNum; Pos++)
        {
            if( isMatchingTile(PosTable[Pos], PosTable[Pos_Ende]) )
            {
                PosTable[iPosCount++] = PosTable[Pos_Ende];
                PosTable[iPosCount++] = PosTable[Pos];
            }
        }
        Pos_Ende++;
    }

    if( iPosCount>=2 )
    {
        srandom( time(NULL) );
        short Pos = ( random() % iPosCount ) & -2;  // Gerader Wert
        posA = PosTable[Pos];
        posB = PosTable[Pos+1];
        return( true );
    }
    else
        return( false );
}

// ---------------------------------------------------------
short BoardWidget::findAllMatchingTiles( POSITION& posA )
{
    short Pos = 0;

    for( short E=0; E<EMax; E++ )
    {
        for( short Y=1; Y<YMax-1; Y++ )
        {
            for( short X=1; X<XMax-1; X++ )
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
/**
    Draw contents of TileBuffer into screen and refresh window.
    TileBuffer must be filled with bitmap data before calling
    this function.

    @param  E   Level of tile. ( -1 <= E < EMAX )
    @param  Y   Y position ( 0 <= Y < YMAX )
    @param  X   X position ( 0 <= X < XMAX )
*/
void BoardWidget::drawBuffer( short E, short Y, short X )
{
    short tsox = 0;
    short tsoy = 0; // Shadow offset
    short E2;

    // iterate levels from bottom to top to remove overlapped regions
    for( E2=E; E2<EMax; E2++, tsox+=TSOX, tsoy+=TSOY )
    {
        if( E2<0 ) continue;    // ground

        for( short y=-1; y<3; y++)
        {
            short Y2=Y+y;
            // skip border where no tiles can be located
            if( Y2<YBorder || Y2>=YMax-YBorder ) continue;

            for( short x=-2; x<2; x++)
            {
                // skip original position
                if( x==0 && y==0 && E2==E )  continue;

                short X2=X+x;
                // skip border where no tiles can be located
                if( X2<XBorder || X2>=XMax-XBorder ) continue;

                if( Game.Board[E2][Y2][X2]  && Game.Mask[E2][Y2][X2]==(UCHAR)'1')
                {
                    BYTE* mask = TileMask;
                    BYTE* dest = TileBuffer;
                    short XP   = TX4*x+tsox; // X Position relative to tile pos
                    short YP   = TY4*y-tsoy; // Y Position relative to tile pos
                    short dex  = TX;         // Delta X == Tile Width
                    short dey  = TY;         // Delta Y == Tile Height

                    if( YP<0 )
                    {
                        mask -= YP*TX;     // skip top region of mask
                        dey += YP;         // YP is negative
                    }
                    else
                    {
                        dest += YP*TX;     // 
                        dey -= YP;         // 
                    }

                    if( XP<0 )
                    {
		        mask -= XP;        // skip left region of mask
                        dex  += XP;        // 
                    }
                    else
                    {
                        dest += XP;
                        dex  -= XP;        // rechter Rand
                    }

                    while( dey-- > 0 )
                    {
                        for( short j=dex; j>0; j--)
                            *dest++ &= *mask++;

                        mask += TX-dex;
                        dest += TX-dex;
                    }
                }
            }
        }
    }
    // iterate over levels above tile to draw shadows
    tsox = tsoy = 0;
    for( E2=E+1; E2<EMax; E2++, tsox+=TSOX, tsoy+=TSOY)
    {
        for( short y=-2; y<2; y++)
        {
            short Y2=Y+y;
            // skip border where no tiles can be located
            if( Y2<YBorder || Y2>=YMax-YBorder ) continue;

            for( short x=-1; x<3; x++)
            {
                short X2 = X+x;
                // skip border where no tiles can be located
                if( X2<XBorder || X2>=XMax-XBorder ) continue;

                if( Game.Board[E2][Y2][X2] 
                 && Game.Mask[E2][Y2][X2]==(UCHAR)'1' )
                {
                    BYTE* dest = TileBuffer;
                    BYTE* mask = TileShadow;
                    short XP   = TX4*x-tsox;
                    short YP   = TY4*y+tsoy;
                    short dex  = TX;
                    short dey  = TY;

                    if( YP<0 )
                    {
                        mask -= YP*TX;       // skip top border
                        dey  += YP;
                        if( dey > TY-TSOY ) dey = TY-TSOY;
                    }
                    else
                    {
                        dest += YP*TX;       // skip bottom
                        dey  -= YP+TSOY;
                    }

                    if( XP<TSOX )
                    {
		        XP -= TSOX;
                        dest += TSOX;
                        mask -= XP;          // skip left border
                        dex  += XP;
                    }
                    else
                    {
                        dest += XP;          // skip right border
                        dex  -= XP;
                    }
                    if( dex > TX-TSOX ) dex=TX-TSOX;

                    while( dey-->0 )
                    {
                        // set bit 5 for shadow
                        for( short j=dex; j>0; j--, dest++, mask++ )
                            if (*dest) *dest |= *mask;
 
                        mask += TX-dex;
                        dest += TX-dex;
                    }
                }
            }
        }
    }

    BYTE* src = TileBuffer;
    short XS = X*TX4 + E*TSOX;  // X Position at screen
    short YS = Y*TY4 - E*TSOY;  // Y Position at screen

    Y = TY;               // Height of bitmap
    if( YS > SY-TY )
        Y = SY-YS;          // Y == count of lines

    if( YS<0 )
    {
        Y += YS;            // subtract
        src -= YS*TX;    // subtrahieren (YS ist < 0)
        YS=0;
    }

    X = TX;               // Width of Bitmap
    if( XS > SX-TX )
        X = SX-XS;          // Anzahl der zu zeichnenden Spalten bestimmen

    if( XS<0 )
    {
        X += XS;            // subtrahieren
        src -= XS;          // addieren (XS ist < 0)
        XS = 0;
    }

    // width and height of update region must positiv
    if( X>0 && Y>0 )
    {
      // draw tile buffer in screen bitmap
      for( short i=0; i<Y; i++ )
      {
          BYTE* dest = qiScreen.scanLine( YS+i ) + XS;

          for( short j=0; j<X; j++ )
          {
              BYTE Q = *src++;
              if (Q) *dest = Q;
              dest++;
          }
          src  += TX-X;
      }
      // force window update at this region
      //this->update( XS, YS-SYOFFSET, X, Y );
      this->repaint( XS, YS-SYOFFSET, X, Y, FALSE );
    }
}

// ---------------------------------------------------------
void BoardWidget::hilightTile( POSITION& Pos )
{
    short E = Pos.e;
    short Y = Pos.y;
    short X = Pos.x;
    BYTE  F = Game.Board[E][Y][X] - TILE_OFFSET;
    BYTE* src =  Tiles + F*TX*TY + TSOX;
    BYTE* dest = TileBuffer + TSOX;

    memset( TileBuffer, 0, TX*TY );

    for( short YP=0; YP < TY-TSOY; YP++)
    {
        // switch color 21 to 20
        for( short x=0; x < TX-TSOX; x++ )
	{
            if( *src == 21 ) *dest = 20;
            else             *dest = *src;
            src++;
            dest++;
	}   

        src  += TSOX;
        dest += TSOX;
    }
    drawBuffer( E, Y, X );
}

// ---------------------------------------------------------
void BoardWidget::drawTile( short E, short Y, short X )
{
    // draw background
    if( E==-1 )
    {
        // if background is hidden by tiles, skip
        if( Game.Board[0][Y][X] && Game.Board[0][Y+1][X] &&
            Game.Board[0][Y+1][X+1] && Game.Board[0][Y][X+1])
            return;

	BYTE* src = Bgnd + ((X*TX4)%iBgndWidth)
	                 + ((Y*TY4)%iBgndHeight)*iBgndBytesPerLine;
        // copy data into destination buffer and draw tile into screen
        BYTE* dest = TileBuffer;
        memset( dest, 0x00, TX*TY );
        for( short y=0; y<TY-TSOY; y++)
	{
            memcpy( dest+TSOX, src+TSOX, TX-TSOX );
            dest += TX;
            src  += iBgndBytesPerLine;
        }
    }
    // draw tile
    else
    {
        // if no tile is on this position, skip
        if( Game.Mask[E][Y][X]!='1' || Game.Board[E][Y][X]==0 )
            return;
        // if tile is hidden by others tiles, skip
        if( E<EMax-1 &&
            Game.Board[E+1][Y][X]     && Game.Board[E+1][Y+1][X] &&
            Game.Board[E+1][Y+1][X+1] && Game.Board[E+1][Y][X+1] &&
            Game.Board[E][Y][X-1]     && Game.Board[E][Y+1][X-1] &&
            Game.Board[E][Y+2][X-1]   && Game.Board[E][Y+2][X]   &&
            Game.Board[E][Y+2][X+1] )
            return;

        BYTE F = Game.Board[E][Y][X];
        if( F >= TILE_FLOWER+4 )
            return;

        BYTE* src = Tiles + (F-TILE_OFFSET)*TX*TY;
        // copy data into destination buffer
        memcpy( TileBuffer, src, TX*TY );
    }
    // draw contents of TileBuffer into screen
    drawBuffer( E, Y, X );
}

// ---------------------------------------------------------
void BoardWidget::drawBoard()
{
    for( short Y=0; Y<YMax; Y+=2 )
    {
        for( short X=0; X<XMax; X+=2 )
            drawTile( -1, Y, X );

	//        drawTile( -1, Y, XMax-1 );
    }
    for( short E=0; E<EMax; E++ )
    {
        for( short Y=YBorder; Y<YMax-YBorder; Y++ )
	{
            for( short X=XBorder; X<XMax-XBorder; X++ )
                drawTile( E, Y, X );
        }
    }
    drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::putTile( POSITION& Pos )
{
    BYTE RepairBuffer[6][6];

    short E=Pos.e;
    short Y=Pos.y;
    short X=Pos.x;

    Game.putTile( E, Y, X, Pos.f );

    memset( RepairBuffer, 0 , sizeof(RepairBuffer) );
    drawTile( E,Y,X );

    for( short dy=-1; dy<3; dy++)
    {
        short Y2 = Y+dy;
        if( Y2>=0 && Y2<YMax )
        {
            for( short dx=-2; dx<2; dx++)
            {
                short X2 = X+dx;
                if( X2>=0 && X2<XMax )
                {
                    short E2 = E;
                    while( E2-- > 0 )
                    {
                        if( Game.Board[E2][Y2][X2]
                         && Game.Mask[E2][Y2][X2]== (UCHAR)'1')
                        {
                            // draw tile under new tile to update shadow
                            drawTile(E2,Y2,X2);
                            break;   
                        }
                    }
                    // if ground is reached
                    if( E2==-1 )
                    {
                        if (! RepairBuffer[dx+2][dy+2])
                        {
                            drawTile(-1,Y2,X2);
                            // mark repaired positions to avoid
                            // positions to be updated twice
                            RepairBuffer[dx+2][dy+2] = 1;
                            RepairBuffer[dx+3][dy+2] = 1;
                            RepairBuffer[dx+3][dy+3] = 1;
                            RepairBuffer[dx+2][dy+3] = 1;
                        }
                    }
                }
            }
        }
    }
}


// ---------------------------------------------------------
void BoardWidget::removeTile( POSITION& Pos )
{
    BYTE RepairBuffer[6][6];

    short E = Pos.e;
    short Y = Pos.y;
    short X = Pos.x;

    Game.TileNum--;                    // Eine Figur weniger
    Game.MoveList[Game.TileNum] = Pos; // Position ins Protokoll eintragen

    memset(RepairBuffer, 0 , sizeof(RepairBuffer));

    // remove tile from game board
    Game.putTile( E, Y, X, 0 );

    for ( short dy=-2; dy<3; dy++)
    {
        short Y2 = Y+dy;
        if( Y2>=0 && Y2<YMax)
        {
            for ( short dx=-2; dx<3; dx++)
            {
                short X2 = X+dx;
                if( X2>=0 && X2<XMax )
                {
                    short E2 = E;
                    while (E2>=0)
                    {
                        if( Game.Board[E2][Y2][X2]
                         && Game.Mask[E2][Y2][X2]==(UCHAR)'1')
                        {
                            drawTile(E2,Y2,X2);
                        }
                        E2--;
                    }
                    // if floor is reached
                    if (E2==-1)
                    {
                        if (! RepairBuffer[dx+2][dy+2])
                        {
                            drawTile(-1,Y2,X2);
                            // to avoid multiple drawing operations,
                            // remember updated positions in
                            RepairBuffer[dx+2][dy+2]=1;
                            RepairBuffer[dx+3][dy+2]=1;
                            RepairBuffer[dx+3][dy+3]=1;
                            RepairBuffer[dx+2][dy+3]=1;
                        }
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------
void BoardWidget::mousePressEvent ( QMouseEvent* event )
{
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

        if( MouseClickPos1.e == EMax )       // first tile
	{
            transformPointToPosition( event->pos(), MouseClickPos1 );
            if( MouseClickPos1.e != EMax && showMatch )
            {
                matchCount = findAllMatchingTiles( MouseClickPos1 );
                TimerState = Match;
                iTimerStep = 1;
                matchAnimationTimeout();
            }
        }
        else                                // second tile
        {
            transformPointToPosition( event->pos(), MouseClickPos2 );
            if( MouseClickPos2.e == EMax )
            {
                cancelUserSelectedTiles();
            }
            else
            {
                if( isMatchingTile( MouseClickPos1, MouseClickPos2 ) )
                {
                    removeTile(MouseClickPos1);
                    removeTile(MouseClickPos2);
                    drawTileNumber();
                    // if no tiles are left, show animation
                    if( Game.TileNum == 0 )
                    {
                        animateMoveList();
                    }
                    // else if no more moves are possible, notify player
                    else if( ! findMove( TimerPos1, TimerPos2 ) )
                    {
                        showMessage( i18n("Sorry, you have lost the game.") );
                    }
                }
                else
                {
                    // redraw tiles in normal state 
                    drawTile( MouseClickPos1 );
                    drawTile( MouseClickPos2 );
                }
                MouseClickPos1.e = EMax;     // mark tile position as invalid 
                MouseClickPos2.e = EMax;
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
    for( E=EMax-1; E>=0; E-- )
    {
        // calculate mouse coordiantes --> position in game board
        X = (point.x() - (E+1)*TSOX) / TX4;
        Y = (point.y() + SYOFFSET + E*TSOY) / TY4;

        // skip when position is illegal
        if (X<0 || X>=XMax || Y<0 || Y>=YMax) continue;

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
                Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                continue;
        }

        if ( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
             (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
            continue;

        // here, position is legal
        MouseClickPos.e = E;
        MouseClickPos.y = Y;
        MouseClickPos.x = X;
        MouseClickPos.f = Game.Board[E][Y][X];
        // give visible feedback
        hilightTile( MouseClickPos ); 
	/*
        {
            char szText[128];
            sprintf( szText, "MousePos: %d/%d - BoardPos: %d/%d/%d", 
                             point.x(), point.y(), X, Y, E );
            setStatusText( szText );
        }
	*/
        break;
    }
}

// ---------------------------------------------------------
bool BoardWidget::loadBoard( const char* pszMask )
{
    short E=0;
    short X=XBorder;          // Rand lassen.
    short Y=YBorder;
    GAMEDATA newGame;

    memset( &newGame, 0, sizeof( newGame ) );

    // loop will be left by break or return
    while( TRUE )
    {
        BYTE c = *pszMask++;

        switch( c )
        {
            case (UCHAR)'1': newGame.MaxTileNum++;
            case (UCHAR)'3':
            case (UCHAR)'2':
            case (UCHAR)'4': newGame.Mask[E][Y][X] = c;
                             break;

            default:         break;
        }
        if( ++X == XMax-XBorder)
        {
            X=XBorder;
            if( ++Y == YMax-YBorder )
            {
                Y=YBorder;
                if( ++E == EMax)
                {
                    // number of tiles have to be even
                    if( newGame.MaxTileNum & 1 ) break;
                    Game = newGame;
                    return( true );
                }
            }
        }
    }
    return( false );
}

// ---------------------------------------------------------
void BoardWidget::setStatusText( const char* pszText )
{
    emit statusTextChanged( pszText );
}

// ---------------------------------------------------------
bool BoardWidget::loadTileset( const char* pszFileName )
{
    short c;
    short y;
    BYTE* src;
    BYTE* dest;
    QImage qiTiles;

    if( ! qiTiles.load( pszFileName ) )
        return( false );

    iTilesNumColors = qiTiles.numColors();    
    if( iTilesNumColors > 128 )
        return( false );

    // allocate memory for bitmaps
    TileBuffer  = (BYTE*) malloc( TX*TY );
    TileMask    = (BYTE*) malloc( TX*TY );
    TileShadow  = (BYTE*) malloc( TX*TY );
    Tiles       = (BYTE*) malloc( 9*5*TX*TY );

    // transform 9*5 tiles from bitmap to own buffer
    for( short t=0; t<9*5; t++ )
    {
        dest = Tiles + t*TX*TY;
        src  = qiTiles.scanLine( (t/9)*TY ) + (t%9)*TX;
        // copy data of one tile into destination buffer
        for( y=0; y < TY; y++ )
        {
            memcpy( dest, src, TX );
            dest += TX;
            src  += qiTiles.bytesPerLine();
        }    
    }

    // create screen image and color table
    qiScreen.create( SX, SY, 8, 256 );

    // create color table for screen
    for( c=0; c<iTilesNumColors; c++ )
    {  
        qiScreen.setColor( c, qiTiles.color( c ) );
    }

    for( c=0; c<128; c++ )
    {
        qiScreen.setColor( c+128, 
                           QColor( qiScreen.color( c ) ).dark(133 ).rgb() );
    }

    // create tile mask, which is used by bitwise AND operation
    src  = Tiles;
    dest = TileMask;
    for( y=0; y<TY; y++ )
    {
        for( short x=0; x<TX; x++)
            *dest++ = (*src++) ?  0 : 255;  
    }

    // create shadow mask, which is used to draw shadows by bitwise OR
    src  = TileMask;
    dest = TileShadow;
    for( y=0; y<TY; y++)
    {
        for( short x=0; x<TX; x++)
            *dest++ = (*src++) ? 0 : 128;
    }

    return( true );
}

// ---------------------------------------------------------
bool BoardWidget::loadBackground(
        const char* pszFileName,
        bool        bShowError
    )
{
    short c;
    short y;
    BYTE* src;
    BYTE* dest;
    QImage qiBgnd;

    if( ! qiBgnd.load( pszFileName ) )
    {
        if( bShowError )
	{
            QString strMsg;
            strMsg.sprintf( i18n("Failed to load image:\n%s"), pszFileName );
            showMessage( strMsg );
	}
        return( false );
    }

    if( iTilesNumColors + qiBgnd.numColors() > 128 )
    {
        if( bShowError )
	{
            QString strMsg;
            strMsg.sprintf( i18n( "Sorry, this image uses %d colors.\n"
                                  "Maximum allowed is %d"),
                            qiBgnd.numColors(), 128 - iTilesNumColors );
            showMessage( strMsg );
        }
        return( false );
    }

    iBgndWidth  = qiBgnd.width();
    iBgndHeight = qiBgnd.height();
    iBgndBytesPerLine = iBgndWidth+TX;

    // free/allocate memory for bitmap
    if( Bgnd ) free( Bgnd );
    Bgnd = (BYTE*) malloc( iBgndBytesPerLine*(iBgndHeight+TY) );

    // create bitmap data for background
    for( y = 0; y<iBgndHeight; y++ )
    {
        dest = Bgnd + y*iBgndBytesPerLine;
        src  = qiBgnd.scanLine( y );
        for( short x=0; x<iBgndWidth; x++ )
            *dest++ = (*src++) + iTilesNumColors;

        memcpy( Bgnd + y*iBgndBytesPerLine + iBgndWidth,
                Bgnd + y*iBgndBytesPerLine,
                TX );
    }
    memcpy( Bgnd+iBgndHeight*iBgndBytesPerLine, Bgnd, TY*iBgndBytesPerLine );

    // update color table for background
    for( c=0; c<qiBgnd.numColors(); c++ )
    {  
        qiScreen.setColor( c+iTilesNumColors, qiBgnd.color( c ) );
    }

    for( c=0; c<128; c++ )
    {
        qiScreen.setColor( c+128, 
                           QColor( qiScreen.color( c ) ).dark(133 ).rgb() );
    }
    return( true );
}

// ---------------------------------------------------------
void BoardWidget::drawTileNumber()
{
    emit tileNumberChanged( Game.MaxTileNum, Game.TileNum );
}

// ---------------------------------------------------------
void BoardWidget::cancelUserSelectedTiles()
{
    if( MouseClickPos1.e != EMax )
    {
        drawTile( MouseClickPos1 ); // redraw tile
        MouseClickPos1.e = EMax;    // mark tile invalid
    }
}

// ---------------------------------------------------------
void BoardWidget::showMessage( const char* pszText )
{
    emit message( pszText );
}


