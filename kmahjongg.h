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

#ifndef _KMAHJONGG_H
#define _KMAHJONGG_H

#include <qapp.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qimage.h>
#include <qfiledlg.h>

#include <kapp.h>
#include <kurl.h>
#include <kmsgbox.h> 
#include <ktopwidget.h>


//----------------------------------------------------------
// DEFINES
//----------------------------------------------------------
#define XMax    36      // Size of game board
#define YMax    18
#define EMax    5
#define XBorder 2       // Border size of the gameboard
#define YBorder 1

// maximum of tiles on gameboard
#define FMax   (EMax*(XMax-2*XBorder)*(YMax-2*YBorder))/4  


//----------------------------------------------------------
// TYPEDEFS
//----------------------------------------------------------
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;


typedef struct pos {
    USHORT e,y,x,f;
} POSITION;


typedef struct gamedata {
    UCHAR    Board[EMax][YMax][XMax]; 
    USHORT   TileNum;          
    USHORT   MaxTileNum;       
    UCHAR    Mask[EMax][YMax][XMax];
    POSITION MoveList[FMax];

    void putTile( short E, short Y, short X, UCHAR f )
    {
        Board[E][Y][X] =
	Board[E][Y+1][X] =
        Board[E][Y+1][X+1] =
        Board[E][Y][X+1] = f;
    }
    void putTile( POSITION& pos )
    {
        putTile( pos.e, pos.y, pos.x, pos.f );
    }
} GAMEDATA;


typedef struct boardinfo {
    char*    pszName;
    char*    pszGameMask;
} BOARDINFO;




/**
    ...

    @author Mathias Mueller
*/
class BoardWidget : public QWidget
{
    Q_OBJECT

    public:
        BoardWidget( QWidget* parent );

        void calculateNewGame( int iBoard = -1 );
        void helpMove();
        void undoMove();
        void startDemoMode();
        void stopDemoMode();
        void animateMoveList();
        void setShowMatch( bool );

        void drawBoard();

        bool loadBoard      ( const char* );
        bool loadTileset    ( const char* );
        bool loadBackground ( const char*, bool bShowError = true );

        static BOARDINFO BoardInfo[];

    public slots:
        void helpMoveTimeout();
        void demoMoveTimeout();
        void matchAnimationTimeout();

    signals:
        void statusTextChanged ( const char* );
        void message           ( const char* );
        void tileNumberChanged ( int iMaximum, int iCurrent );
        void demoModeChanged   ( bool bActive );

    protected:
        void paintEvent      ( QPaintEvent* );
        void mousePressEvent ( QMouseEvent* );

        void setStatusText ( const char* );
        void showMessage   ( const char* );
        void cancelUserSelectedTiles();
        void drawTileNumber();

        void drawBuffer  ( short, short, short );
        void drawTile    ( short, short, short );
        void hilightTile ( POSITION& );
        void drawTile    ( POSITION& Pos ) { drawTile( Pos.e, Pos.y, Pos.x ); }
        void putTile     ( POSITION& );
        void removeTile  ( POSITION& );

        void transformPointToPosition( const QPoint&, POSITION& );

        void findFreePositions( POSITION& );
        bool isMatchingTile( POSITION&, POSITION& );
        bool generateStartPosition();
        bool findMove( POSITION&, POSITION& );
        short findAllMatchingTiles( POSITION& );
        void stopMatchAnimation();

        GAMEDATA Game;         

        int iPosCount;             // count of valid positions in PosTable
        POSITION PosTable[FMax];   // Table of all possible positions
        POSITION MouseClickPos1, MouseClickPos2;
        POSITION TimerPos1, TimerPos2;

        enum STATES { Stop, Demo, Help, Animation, Match } TimerState;
        int iTimerStep;

        short matchCount;
        bool  showMatch;

        QImage qiScreen;

        BYTE* Bgnd;
        BYTE* Tiles;
        BYTE* TileBuffer;
        BYTE* TileMask;
        BYTE* TileShadow;

        // number of colors used by tileset ( always < 128 ) 
        int   iTilesNumColors;
        // Background bitmap width
        int   iBgndWidth;
        // number of bytes per Background bitmap scanline
        int   iBgndBytesPerLine;
        // Background tile height
        int   iBgndHeight;
        // array index of current game board data
        int   iCurrentBoard;
};



/**
    ...
    @author Mathias 
*/
class KMahjonggWidget : public KTopLevelWidget
{
    Q_OBJECT

    public:
        KMahjonggWidget();
        ~KMahjonggWidget();

    public slots:
        void menuCallback( int );
        void startNewGame( int );
        void showStatusText ( const char* );
        void showMessage    ( const char* );
        void showTileNumber( int iMaximum, int iCurrent );
        void demoModeChanged( bool bActive );
  
    protected:
        void closeEvent ( QCloseEvent* );

        void setupStatusBar();
        void setupMenuBar();
        void updateStatusbar( bool );

    private:
        BoardWidget* bw;
        KStatusBar*  pStatusBar;
        KMenuBar*    pMenuBar;
        bool         bShowStatusbar;
        bool         bShowMatchingTiles;
        bool         bDemoModeActive;
};


#endif





