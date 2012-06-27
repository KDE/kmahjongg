/* Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
 * Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H


#include "KmTypes.h"
#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "kmahjongglayout.h"
#include "GameData.h"
#include "TileCoord.h"
#include "TileSprite.h"

#include <QHash>
#include <QPoint>

#include <qevent.h>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgamecanvas.h>

#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfiledialog.h>


#define ANIMSPEED 200
#define ID_GAME_TIMER 999


class GameItem;
class QGraphicsItem;

/**
 * @short Where all the funn happens
 *
 * @see BoardLayout
 * @author Mathias Mueller */
class BoardWidget : public KGameCanvasWidget
{
    Q_OBJECT

public:
    /**
     * Class Constructor
     *
     * @param *parent blah blah */
    explicit BoardWidget(QWidget *parent = 0);

    /**
     * Class Destructor */
    ~BoardWidget();

    /**
     * Method Description */
    void calculateNewGame(int num = -1);

    /**
     * Method Description
     *
     * @return int blah blah
     * @ref redoMove() */
    int undoMove();

    /**
     * Method Description */
    void redoMove();

    /**
     * Method Description */
    void startDemoMode();

    /**
     * Method Description */
    void stopDemoMode();

    /**
     * Method Description */
    void pause();

    /**
     * Method Description */
    void gameLoaded();

    /**
     * Method Description */
    void animateMoveList();

    /**
     * Method Description
     *
     * @param show */
    void setShowMatch(bool show);

    /**
     * Method Description
     *
     * @return long gameGenerationNum
     * @see gameGenerationNum */
    long getGameNum() {return gameGenerationNum;}

    /**
     * Method Description
     *
     * @return QString filename */
    QString getBoardName() {return theBoardLayout.board()->getFilename();}

    /**
     * Method Description
     *
     * @return QString filename */
    QString getLayoutName();

    TileViewAngle m_angle;
    GameData *Game;
    KMahjonggTileset theTiles;
    KMahjonggBackground theBackground;
    KMahjonggLayout theBoardLayout;

//    QHash<TileCoord, TileSprite *> spriteMap;
    QHash<TileCoord, GameItem *> spriteMap;

    void raiseItem(QGraphicsItem *item);
    void lowerItem(QGraphicsItem *item);

public slots:
    /**
    * Slot Description */
    void loadSettings();

    /**
    * Slot Description */
    void saveSettings();

    /**
    * Slot Description */
    void resizeTileset(const QSize &wsize);

    /**
    * Slot Description */
    void animatingMoveListForward();

    /**
    * Slot Description */
    void animatingMoveListBackwards();

    /**
    * Slot Description */
    void shuffle();

    /**
    * Slot Description */
    void angleSwitchCW();

    /**
    * Slot Description */
    void angleSwitchCCW();

    /**
    * Slot Description */
    void helpMove();

    /**
    * Slot Description */
    void helpMoveTimeout();

    /**
    * Slot Description */
    void helpMoveStop();

    /**
    * Slot Description */
    void demoMoveTimeout();

    /**
    * Slot Description */
    void matchAnimationTimeout();

    /**
    * Slot Description */
    void setDisplayedWidth();

    /**
    * Slot Description
    *
    * @return @c true if ...
    * @return @c false if ... */
    bool loadTileset    ( const QString & );

    /**
    * Slot Description
    *
    * @return @c true if ...
    * @return @c false if ... */
    bool loadBoardLayout( const QString& );

    /**
     * Slot Description
     *
     * @return @c true if ...
     * @return @c false if ... */
    bool loadBoard();

    /**
     * Slot Description */
    void drawBoard(bool showTiles = true);

    /**
     * Slot Description */
    void updateSpriteMap();

    /**
     * Slot Description */
    void populateSpriteMap();

    /**
     * Slot Description
     *
     * @return @c true if ...
     * @return @c false if ... */
    bool loadBackground(const QString&, bool bShowError = true);

signals:
    /**
     * Signal Description */
    void statusTextChanged(const QString&, long);

    /**
     * Signal Description */
    void tileNumberChanged(int iMaximum, int iCurrent, int iLeft);

    /**
     * Signal Description
     *
     * @param bActive */
    void demoModeChanged(bool bActive);

    /**
     * Signal Description */
    void gameCalculated();

    /**
     * Signal Description */
    void gameOver(unsigned short removed, unsigned short cheats);

protected:
    /**
     * Overridden virtual method for wheel events. */
    virtual void wheelEvent(QWheelEvent *event);

    /**
     * Override the itemAt method of the KGameCanvasWidget. We need this cause of corrections in the
     * 3D simulated view with the stones, as the normal KGameCanvasWidget only supports 2D objects,
     * and the thrown shadows should not be a part of the click event.
     *
     * @param point the point we click on the KGameCanvasWidget. */
    KGameCanvasItem* itemAt(const QPoint &point) const;

    /**
     * Virtual Method Description */
    virtual void resizeEvent(QResizeEvent *event);

    /**
     * Method Description */
    void getFileOrDefault(const QString &filename, const QString &type, QString &res);

    /**
     * Method Description */
    void mousePressEvent(QMouseEvent*);

    /**
     * Method Description */
    void setStatusText(const QString&);

    /**
     * Method Description */
    void cancelUserSelectedTiles();

    /**
     * Method Description */
    void drawTileNumber();

    /**
     * Method Description */
    void hilightTile(POSITION&, bool on = true, bool refresh = true);

    /**
     * Method Description */
    void putTileInBoard(POSITION&, bool refresh = true);

    /**
     * Method Description */
    void removeTile(POSITION&, bool refresh = true);

    /**
     * Transform window point to board position.
     *
     * @param  point          Input: Point in window coordinates
     * @param  MouseClickPos  Output: Position in game board */
    void transformPointToPosition(const QPoint &point, POSITION &MouseClickPos);

    /**
     * Method Description */
    void stopEndAnimation();

    /**
     * Method Description */
    void stopMatchAnimation();

    /**
     * Method Description
     *
     * @return int blah blah */
    int requiredWidth();

    /**
     * Method Description
     *
     * @return int blah blah */
    int requiredHeight();

    /**
     * Method Description
     *
     * @return int blah blah */
    int requiredHorizontalCells();

    /**
     * Method Description
     *
     * @return int blah blah */
    int requiredVerticalCells();

    /**
     * This method test if any valid moves are available.
     *
     * @return False if no valid move available, else true. */
    bool validMovesAvailable();

    int iTimerStep;

    short matchCount;
    bool showMatch;
    bool showHelp;
    bool gamePaused;

    unsigned short cheatsUsed;
    long gameGenerationNum;

    POSITION MouseClickPos1;
    POSITION MouseClickPos2;
    POSITION TimerPos1;
    POSITION TimerPos2;

    /**
    * @short Describe the enum */
    enum STATES {
        Stop,
        Demo,
        Help,
        Animation,
        Match
    } TimerState;

    QTimer *timer;
    QTimer *animateForwardTimer;
    QTimer *animateBackwardsTimer;

private:
    QString getRandomLayoutName() const;
};


#endif // BOARDWIDGET_H
