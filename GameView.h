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

#include <QGraphicsView>

#include "KmTypes.h"

#define ANIMATION_SPEED 200


class GameScene;
class GameData;
class GameItem;
class SelectionAnimation;
class DemoAnimation;
class KMahjonggLayout;
class KMahjonggTileset;
class KMahjonggBackground;
class QMouseEvent;

/**
 * The Mahjongg board where the tiles will be placed.
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
    GameView(GameScene *pGameScene, QWidget *pParent = 0);

    /**
     * Destructor */
    ~GameView();

    /**
     * Items where added to the scene and should now be layouted. */
    void updateItemsPosition();

    /**
     * Get the POSITION struct copy of the GameItem positions.
     *
     * @param pGameItem The GameItem to get the position struct from. */
    POSITION getPositionFromItem(GameItem * pGameItem);

    /**
     * Get the GameItem of a POSITION.
     *
     * @param stGamePos The position of the item.
     * @return The GameItem object or NULL if no one exist with the given position. */
    GameItem * getItemFromPosition(POSITION stGamePos);

    /**
     * Sets the status text.
     *
     * @param rText The new status text. */
    void setStatusText(QString const &rText);

    /**
     * Updates the whole widget.
     *
     * @param bShowTiles True if the tiles should be displayed, else false. */
    void updateWidget(bool bShowTiles);

    /**
     * Override from QGraphicsView. */
    virtual void resizeEvent(QResizeEvent *pEvent);

    /**
     * Override from QGraphicsView. */
    virtual void mousePressEvent(QMouseEvent * pMouseEvent);

    /**
     * Override from QGraphicsView. */
    GameScene * scene() const;

    /**
     * Updates the images of the items. */
    void updateItemsImages();

    /**
     * Updates the order of the items. */
    void updateItemsOrder();

    /**
     * Set and get the GameData object. */
    void setGameData(GameData *pGameData);
    GameData * getGameData();

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
     * Set the match variable. If set to true, the matching items to the selected will be animated.
     *
     * @param bMatch The match value to set up. */
    void setMatch(bool bMatch);

    /**
     * Get the match value.
     *
     * @return True when matching items of the eselected one will be displayed, else false. */
    bool getMatch() const;

public slots:
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
     * Sets the tileset path and tries to load it.
     *
     * @param rTilesetPath The path to the tileset.
     * @return True if setting and therfore loading success, else false. */
    bool setTilesetPath(QString const &rTilesetPath);
    QString getTilesetPath();

    /**
     * Sets the background path and tries to load it.
     *
     * @param rBackgroundPath The path to the background.
     * @return True if setting and therfore loading success, else false. */
    bool setBackgroundPath(QString const &rBackgroundPath);
    QString getBackgroundPath();

    /**
     * Set the board layout path and tries to load it.
     *
     * @param rBoardLayoutPath THe path to the board layout.
     * @return True if loading success else false. */
    bool setBoardLayoutPath(QString const &rBoardLayoutPath);
    QString getBoardLayoutPath();

    /**
     * Create a new game.
     *
     * @param iGameNumber The game number to create or -1 for a random number. */
    void createNewGame(int iGameNumber = -1);

    /**
     * Populates the number of the items, by emit a signal: itemNumberChanged(...). */
    void populateItemNumber();

    /**
     * Search for a valid move silently or with an information text.
     *
     * @param bSilent False if a message should appears when no legal moves exist, else true.
     *        Default ist false!
     * @return True if a legal move exist, else false. */
    bool validMovesAvailable(bool bSilent = false);

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
    void helpMatch(GameItem * pGameItem);

signals:
    /**
     * Emits when a new game was calculated. */
    void newGameCalculated();

    /**
     * Emits when the status text changed.
     *
     * @param rText The new status text.
     * @param lGameNumber The actual game number. */
    void statusTextChanged(const QString &rText, long lGameNumber);

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

private slots:
    /**
     * Change the selected state of the given item.
     *
     * @param stItemPos The position of the item.
     * @param bSelected The selection state to set. */
    void changeItemSelectedState(POSITION & stItemPos, bool bSelected);

    /**
     * Gets called when a pair was selected. */
    void selectionChanged();

private:
    /**
     * Reloads the board and therefore create a new GameData object. */
    void loadBoard();

    /**
     * Resize the tileset to the given size.
     *
     * @param rSize The new size of the tileset. */
    void resizeTileset(QSize const &rSize);

    /**
     * Updates the background by creating a new QPalette object. */
    void updateBackground();

    /**
     * Add all the items from the board layout to the scene object. */
    void addItemsFromBoardLayout();

    /**
     * Loads the board layout from the given path.
     *
     * @return True if loading success, else false. */
    bool loadBoardLayoutFromPath();

    /**
     * Order the line starting by the item.
     *
     * @param pStartItem The item where the line starts.
     * @param iXStart The x position of the item.
     * @param iY The y position of the item.
     * @param iZ The z position of the item.
     * @param iZCount The z count variable for ther order. */
    void orderLine(GameItem * pStartItem, int iXStart, int iXEnd, int iXCounter, int iY,
        int iYCounter, int iZ, int &iZCount);

    unsigned short m_usCheatsUsed;
    long m_lGameNumber;
    bool m_bGamePaused;
    bool m_bMatch;

    GameData * m_pGameData;
    GameItem * m_pSelectedItem;

    QString * m_pBoardLayoutPath;
    QString * m_pTilesetPath;
    QString * m_pBackgroundPath;

    SelectionAnimation * m_pHelpAnimation;
    DemoAnimation * m_pDemoAnimation;

    KMahjonggLayout * m_pBoardLayout;
    KMahjonggTileset * m_pTiles;
    KMahjonggBackground * m_pBackground;

    TileViewAngle m_angle;
};


#endif // GAMEVIEW_H
