#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <qwidget.h>
#include <qevent.h>
#include <krandomsequence.h>


#include "KmTypes.h"
#include "Tileset.h"
#include "Background.h"
#include "BoardLayout.h"

typedef struct gamedata {
    int      allow_undo;
    int      allow_redo;
    UCHAR    Board[BoardLayout::depth][BoardLayout::height][BoardLayout::width];
    USHORT   TileNum;
    USHORT   MaxTileNum;
    UCHAR    Mask[BoardLayout::depth][BoardLayout::height][BoardLayout::width];
    UCHAR    hilighted[BoardLayout::depth][BoardLayout::height][BoardLayout::width];
    POSITION MoveList[BoardLayout::maxTiles];
    void putTile( short e, short y, short x, UCHAR f )
    {


        Board[e][y][x] = Board[e][y+1][x] =
		   Board[e][y+1][x+1] = Board[e][y][x+1] = f;
    }
    void putTile( POSITION& pos )
    {
        putTile( pos.e, pos.y, pos.x, pos.f );
    }


    bool tilePresent(int z, int y, int x) {
	return(Board[z][y][x]!=0 && Mask[z][y][x] == '1');
    }

    bool partTile(int z, int y, int x) {
	return (Board[z][y][x] != 0);
    }

    int shadowHeight(int z, int y, int x) {


    if ((z>=BoardLayout::depth||y>=BoardLayout::height||x>=BoardLayout::width))
	return 0;


	if ((y < 0) || (x < 0))
		return 0;

	int h=0;
	for (int e=z; e<BoardLayout::depth; e++) {
		if (Board[e][y][x] && Mask[e][y][x]) {


			h++;
		} else {
			return h;
		}

	}
	return h;
    }


} GAMEDATA;

#define ANIMSPEED    200

// tiles symbol names:
#define TILE_OFFSET      2

#define TILE_CHARACTER  ( 0 + TILE_OFFSET)
#define TILE_BAMBOO     ( 9 + TILE_OFFSET)
#define TILE_ROD        (18 + TILE_OFFSET)
#define TILE_SEASON     (27 + TILE_OFFSET)
#define TILE_WIND       (31 + TILE_OFFSET)
#define TILE_DRAGON     (36 + TILE_OFFSET)
#define TILE_FLOWER     (39 + TILE_OFFSET)

#define ID_GAME_TIMER 		999

/**
 *  @author Mathias Mueller
 */
class BoardWidget : public QWidget
{
    Q_OBJECT

    public:
  	BoardWidget( QWidget* parent = 0, const char *name = 0 );
	~BoardWidget();

        void calculateNewGame(int num = -1 );
        int  undoMove();
	void redoMove();
        void startDemoMode();
        void stopDemoMode();

	void pause();
	void gameLoaded();

        void animateMoveList();
        void setShowMatch( bool );
	void tileSizeChanged();
	long getGameNum() {return gameGenerationNum;}
	QString &getBoardName(){return theBoardLayout.getFilename();}
	QString &getLayoutName() {return theBoardLayout.getFilename();}


    public slots:
	void loadSettings();
        void saveSettings();

	void shuffle();
        void helpMove();
        void helpMoveTimeout();
	void helpMoveStop();
        void demoMoveTimeout();
        void matchAnimationTimeout();
	void setDisplayedWidth();
        bool loadTileset    ( const QString & );
        bool loadBoardLayout( const QString& );
        bool loadBoard      ( );
        void updateScaleMode ();
        void drawBoard(bool deferUpdate = true);
        bool loadBackground ( const QString&, bool bShowError = true );
    signals:
        void statusTextChanged ( const QString&, long );
        void tileNumberChanged ( int iMaximum, int iCurrent, int iLeft );
        void demoModeChanged   ( bool bActive );

	void gameCalculated();

		void gameOver(unsigned short removed, unsigned short cheats);
    protected:
	void getFileOrDefault(QString filename, QString type, QString &res);
	void shadowArea(int z, int y, int x, int sx, int sy, int rx, int ry, QPixmap *src);
	void shadowTopLeft(int depth, int sx, int sy, int rx, int ry,QPixmap *src, bool flag);
	void shadowBotRight(int depth, int sx, int sy, int rx, int ry,QPixmap *src, bool flag);
        void paintEvent      ( QPaintEvent* );
        void mousePressEvent ( QMouseEvent* );

        void setStatusText ( const QString& );
        void cancelUserSelectedTiles();
        void drawTileNumber();

        void hilightTile ( POSITION&, bool on=true, bool refresh=true );
        void putTile     ( POSITION& , bool refresh = true);
        void removeTile  ( POSITION& , bool refresh = true);
	void setRemovedTilePair(POSITION &a, POSITION &b);
	void clearRemovedTilePair(POSITION &a, POSITION &b);
        void transformPointToPosition( const QPoint&, POSITION& );

        bool isMatchingTile( POSITION&, POSITION& );
        bool generateStartPosition2();
        bool findMove( POSITION&, POSITION& );
        int  moveCount( );
        short findAllMatchingTiles( POSITION& );
        void stopMatchAnimation();
	void stackTiles(unsigned char t, unsigned short h, unsigned short x,unsigned  short y);
	void initialiseRemovedTiles();

	int requiredWidth();
	int requiredHeight();

	void calcShadow(int e, int y, int x, int &left, int &right, int &corn);


	// new bits for game generation
	void randomiseFaces();
	int tilesAllocated;
	int tilesUsed;
	void getFaces(POSITION &a, POSITION &b);
	UCHAR tilePair[144];

	KRandomSequence random;

	Tileset  theTiles;
	Background theBackground;

	BoardLayout theBoardLayout;


        int iPosCount;             // count of valid positions in PosTable
        POSITION PosTable[BoardLayout::maxTiles];   // Table of all possible positions
        POSITION MouseClickPos1, MouseClickPos2;
        POSITION TimerPos1, TimerPos2;

        enum STATES { Stop, Demo, Help, Animation, Match } TimerState;
        int iTimerStep;

        short matchCount;
        bool  showMatch;
        bool  showHelp;

        QTimer *timer;

	// offscreen draw area.
	QPixmap backBuffer;		// pixmap to render to



	bool    updateBackBuffer;	// does board need redrawing. Not if it is just a repaint

	bool gamePaused;

	// storage for hiscore claculation
	unsigned short cheatsUsed;

	// seed for the random number generator used for this game
	long  gameGenerationNum;

	// storage to keep track of removed tiles
	unsigned char removedCharacter[9];
	unsigned char removedBamboo[9];
	unsigned char removedRod[9];
	unsigned char removedDragon[3];
	unsigned char removedWind[9];
	unsigned char removedFlower[4];
	unsigned char removedSeason[4];

	// new bits for new game generation, with solvability
	int numTiles;
	POSITION tilePositions[BoardLayout::maxTiles];
	DEPENDENCY positionDepends[BoardLayout::maxTiles];
	void generateTilePositions();
	void generatePositionDepends();
	int tileAt(int x, int y, int z);
	bool generateSolvableGame();
	bool onlyFreeInLine(int position);
	int selectPosition(int lastPosition);
	void placeTile(int position, int tile);
	void updateDepend(int position);

public:
  GAMEDATA Game;
};

#endif // BOARDWIDGET_H

