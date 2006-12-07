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

#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <qevent.h>
#include <kgamecanvas.h>
#include <QHash>
#include <QPoint>

#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfiledialog.h>

#include "KmTypes.h"
#include "kmahjonggtileset.h"
#include "Background.h"
#include "BoardLayout.h"
#include "GameData.h"
#include "TileCoord.h"
#include "TileSprite.h"

#define ANIMSPEED    200

#define ID_GAME_TIMER 		999

/**
 *  @author Mathias Mueller
 */
class BoardWidget : public KGameCanvasWidget
{
    Q_OBJECT

    public:
  	BoardWidget( QWidget* parent = 0 );
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
	long getGameNum() {return gameGenerationNum;};
	QString &getBoardName(){return theBoardLayout.getFilename();};
	QString &getLayoutName() {return theBoardLayout.getFilename();};

	KGameCanvasPixmap * backsprite;
        QHash<TileCoord, TileSprite *> spriteMap;

	TileViewAngle m_angle;

    public slots:
	void loadSettings();
        void saveSettings();
        void resizeTileset ( const QSize & wsize );

	void animatingMoveListForward();
	void animatingMoveListBackwards();
	void shuffle();
	void angleSwitchCW();
	void angleSwitchCCW();
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
	void updateSpriteMap();
	void populateSpriteMap();
        bool loadBackground ( const QString&, bool bShowError = true );
    signals:
        void statusTextChanged ( const QString&, long );
        void tileNumberChanged ( int iMaximum, int iCurrent, int iLeft );
        void demoModeChanged   ( bool bActive );

	void gameCalculated();
	void gameOver(unsigned short removed, unsigned short cheats);
    protected:
	virtual void resizeEvent ( QResizeEvent * event );
	void getFileOrDefault(const QString &filename, const QString &type, QString &res);
        //void paintEvent      ( QPaintEvent* );
        void mousePressEvent ( QMouseEvent* );

        void setStatusText ( const QString& );
        void cancelUserSelectedTiles();
        void drawTileNumber();

        void hilightTile ( POSITION&, bool on=true, bool refresh=true );
        void putTileInBoard     ( POSITION& , bool refresh = true);
        void removeTile  ( POSITION& , bool refresh = true);
        void transformPointToPosition( const QPoint&, POSITION& );

        void stopMatchAnimation();
	void stackTiles(QPainter* p, unsigned char t, unsigned short h, unsigned short x,unsigned  short y);

	int requiredWidth();
	int requiredHeight();
	int requiredHorizontalCells();
	int requiredVerticalCells();

	KMahjonggTileset  theTiles;
	Background theBackground;
	BoardLayout theBoardLayout;

        POSITION MouseClickPos1, MouseClickPos2;
        POSITION TimerPos1, TimerPos2;

        enum STATES { Stop, Demo, Help, Animation, Match } TimerState;
        int iTimerStep;

        short matchCount;
        bool  showMatch;
        bool  showHelp;

        QTimer *timer;

	bool gamePaused;

	// storage for hiscore claculation
	unsigned short cheatsUsed;

	// seed for the random number generator used for this game
	long  gameGenerationNum;

public:
  GameData * Game;
};

#endif // BOARDWIDGET_H

