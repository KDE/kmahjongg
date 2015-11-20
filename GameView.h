/* Copyright (C) 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA. */

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "KmTypes.h"

#include <QGraphicsView>

static const int ANIMATION_SPEED = 200;

// Forward declaration...
class GameScene;
class GameData;
class GameItem;
class SelectionAnimation;
class MoveListAnimation;
class DemoAnimation;
class KMahjonggTileset;
class KMahjonggBackground;
class QMouseEvent;

/**
 * The Mahjongg board where the tiles (GameItems) will be painted.
 *
 * @author Christian Krippendorf */
class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param pParent The parent widget.
     * @param pGameScene The related GameScene object. */
    GameView(GameScene *pGameScene, GameData *pGameData, QWidget *pParent = 0);
    ~GameView();

    /**
     * Items where added to the scene and should now be layouted.
     *
     * @param pGameItem The items of which the positions should be updated. */
    void updateItemsPosition(QList<GameItem *> gameItems);

    /**
     * Updates the whole widget.
     *
     * @param bShowTiles True if the tiles should be displayed, else false. */
    void updateWidget(bool bShowTiles);

    /**
     * Override from QGraphcisView. */
    virtual QList<GameItem *> items() const;

    /**
     * Override from QGraphicsView. */
    GameScene * scene() const;

    /**
     * Set the GameData object.
     *
     * @param pGameData The game data object. */
    void setGameData(GameData *pGameData);

    /**
     * Get the GameData object that is actually set.
     *
     * @return The actual GameData object. */
    GameData * getGameData() const;

    /**
     * Set the angle of the view.
     *
     * @param angle The angle of to set up. */
    void setAngle(TileViewAngle angle);

    /**
     * Get the angle of the view.
     *
     * @return The angle of the view. */
    TileViewAngle getAngle() const;

    /**
     * Test for active help animation and maybe close.
     *
     * @param bStop Stop the help animation if running.
     * @return Return true if the help animation was running else false. */
    bool checkHelpAnimationActive(bool bStop = false);

    /**
     * Test for active demo animation and maybe close.
     *
     * @param bStop Stop the demo animation if running.
     * @return Return true if the demo animation was running else false. */
    bool checkDemoAnimationActive(bool bStop = false);

    /**
     * Test for active move list animation and maybe close.
     *
     * @param bStop Stop the move list animation if running.
     * @return Return true if the move list animation was running else false. */
    bool checkMoveListAnimationActive(bool bStop = false);

    /**
     * Set the match variable. If set to true, the matching items to the selected will be animated.
     *
     * @param bMatch The match value to set up. */
    void setMatch(bool bMatch);

    /**
     * Get the match value.
     *
     * @return True when matching items of the eselected one will be displayed, else false. */
    bool getMatch() const;

    /**
     * Gets the tilesetpath that is actually set.
     *
     * @return The tilesetpath as a string. */
    QString getTilesetPath() const;

    /**
     * Gets the background path that is actually set.
     *
     * @return The background path as a string. */
    QString getBackgroundPath() const;

    /**
     * Sets the tileset path and tries to load it.
     *
     * @param rTilesetPath The path to the tileset.
     * @return True if setting and therfore loading success, else false. */
    bool setTilesetPath(QString const &rTilesetPath);

    /**
     * Sets the background path and tries to load it.
     *
     * @param rBackgroundPath The path to the background.
     * @return True if setting and therfore loading success, else false. */
    bool setBackgroundPath(QString const &rBackgroundPath);

    /**
     * Undo the last move.
     *
     * @return True if successfull, else false. */
    bool undo();

    /**
     * Redo the last undo.
     *
     * @return True if successfull, else false. */
    bool redo();

    /**
     * Test if undo is allowed.
     *
     * @return True if allowed, else false. */
    bool checkUndoAllowed();

    /**
     * Test if redo is allowed.
     *
     * @return True if allowed, else false. */
    bool checkRedoAllowed();

    /**
     * Get the game number.
     *
     * @return The game number or -1 if no game number set. */
    long getGameNumber() const;

    /**
     * Set the game number.
     *
     * @param gameNumber Game number. */
    void setGameNumber(long gameNumber);

    /**
     * Search for a valid move silently or with an information text.
     *
     * @param bSilent False if a message should appears when no legal moves exist, else true.
     *        Default ist false!
     * @return True if a legal move exist, else false. */
    bool validMovesAvailable(bool bSilent = false);

    /**
     * Hide/show tiles when game is paused/unpaused. */
    void pause(bool isPaused);

    /**
     * Get whether a game was generated.
     *
     * @return True if game was generated, or false. */
    bool gameGenerated();

public slots:
    /**
     * Add a new item with teh given position and update imgages, position and order.
     *
     * @param pGameItem The new game item object. */
    void addItemAndUpdate(POSITION &stItemPos);

    /**
     * Remove the given item.
     *
     * @param stItemPos The item position. */
    void removeItem(POSITION &stItemPos);

    /**
     * Starts the demo animation. */
    void startDemo();

    /**
     * Switch the view angle to the next right around. */
    void angleSwitchCCW();

    /**
     * Switch the view angle to the next left around. */
    void angleSwitchCW();

    /**
     * Create a new game.
     *
     * @param iGameNumber The game number to create or -1 for a random number. */
    void createNewGame(long iGameNumber = -1);

    /**
     * Shuffle the position of items. */
    void shuffle();

    /**
     * Give a hint for a valid move. */
    void helpMove();

    /**
     * Give a hint to the matching tiles.
     *
     * @param pGameItem The item we search matching tiles for. */
    void helpMatch(const GameItem * const pGameItem);

    /**
     * Start the move list animation. */
    void startMoveListAnimation();

    /**
     * Clear the selection. */
    void clearSelectedTile();

protected:
    /**
     * Override from QGraphicsView. */
    virtual void resizeEvent(QResizeEvent *pEvent);

    /**
     * Override from QGraphicsView. */
    virtual void mousePressEvent(QMouseEvent * pMouseEvent);

signals:
    /**
     * Emits when a new game was calculated. */
    void newGameCalculated();

    /**
     * Emits when the status text changed.
     *
     * @param rText The new status text.
     * @param lGameNumber The actual game number. */
    void statusTextChanged(const QString &text, long lGameNumber);

    /**
     * Emits when the number of the items changed or could change.
     *
     * @param iMaxItemNum The max tile number.
     * @param iItemNum The item number that are still there.
     * @param iMoveCount Number of moves. */
    void itemNumberChanged(int iMaxItemNum, int iItemNum, int iMoveCount);

    /**
     * Emits when the game is over.
     *
     * @param usRemovedItems The number of the removed items.
     * @param usCheatsUsed The number of the cheats that are used. */
    void gameOver(unsigned short usRemovedItems, unsigned short usCheatsUsed);

    /**
     * Emits when demo is played out and lost or stopped by a mouse click, or
     * the MoveListAnimation is stopped by a mouse click.
     *
     * @param bDemoGameLost True if demo game is played out and lost. */
    void demoOrMoveListAnimationOver(bool bDemoGameLost);

    /**
     * Emits when no more moves are available. */
    void noMovesAvailable();

private slots:
    /**
     * Add a new item with the given position.
     *
     * @param stItemPos The position for the new item.
     * @param bUpdateImage True for updating the images else false.
     * @param bUpdateOrder True for updating the order else false.
     * @param bUpdatePosition True for updating the position else false. */
    void addItem(POSITION &stItemPos, bool bUpdateImage = false, bool bUpdateOrder = false,
        bool bUpdatePosition = false);

    /**
     * Add a new item.
     *
     * @param pGameItem The new game item object.
     * @param bUpdateImage True for updating the images else false.
     * @param bUpdateOrder True for updating the order else false.
     * @param bUpdatePosition True for updating the position else false. */
    void addItem(GameItem *pGameItem, bool bUpdateImage = false, bool bUpdateOrder = false,
        bool bUpdatePosition = false);

    /**
     * When the game is over by the demo mode.
     *
     * @param bWon True if the computer won in demo mode, else false. */
    void demoGameOver(bool bWon);

    /**
     * Change the selected state of the given item.
     *
     * @param stItemPos The position of the item.
     * @param bSelected The selection state to set. */
    void changeItemSelectedState(POSITION &stItemPos, bool bSelected);

    /**
     * Gets called when a pair was selected. */
    void selectionChanged();

private:
    /**
     * Updates the images of the items.
     *
     * @param pGameItem The items of which the images should be updated. */
    void updateItemsImages(QList<GameItem *> gameItems);

    /**
     * Updates the order of the items. */
    void updateItemsOrder();

    /**
     * Populates the number of the items, by emit a signal: itemNumberChanged(...). */
    void populateItemNumber();

    /**
     * Sets the status text.
     *
     * @param rText The new status text. */
    void setStatusText(const QString &text);

    /**
     * Resize the tileset to the given size.
     *
     * @param rSize The new size of the tileset. */
    void resizeTileset(const QSize &size);

    /**
     * Updates the background by creating a new QPalette object. */
    void updateBackground();

    /**
     * Add all the items from the board layout to the scene object. */
    void addItemsFromBoardLayout();

    /**
     * Order the line starting by the item.
     *
     * @param pStartItem The item where the line starts.
     * @param iXStart The x position of the item.
     * @param iY The y position of the item.
     * @param iZ The z position of the item.
     * @param iZCount The z count variable for ther order. */
    void orderLine(GameItem *pStartItem, int iXStart, int iXEnd, int iXCounter, int iY,
        int iYCounter, int iZ, int &iZCount);

    unsigned short m_usCheatsUsed;
    long m_lGameNumber;
    bool m_bGamePaused;
    bool m_bMatch;
    bool m_bGameGenerated;

    GameData *m_pGameData;
    GameItem *m_pSelectedItem;

    QString *m_pTilesetPath;
    QString *m_pBackgroundPath;

    SelectionAnimation *m_pHelpAnimation;
    MoveListAnimation *m_pMoveListAnimation;
    DemoAnimation *m_pDemoAnimation;

    KMahjonggTileset *m_pTiles;
    KMahjonggBackground *m_pBackground;

    TileViewAngle m_angle;

    // Needed for disconnecting connection
    QMetaObject::Connection m_selectionChangedConnect;
};

#endif // GAMEVIEW_H
