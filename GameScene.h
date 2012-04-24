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

#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>


class GameItem;
class GameData;
class GameWidget;
class KMahjonggLayout;

/**
 * Holds and manages all GameItems.
 *
 * @author Christian Krippendorf */
class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param pParent The parent object. */
    GameScene(GameData *pGameData = 0, QObject *pParent = 0);

    /**
     * Destructor */
    ~GameScene();

    /**
     * Set the game data object. This function does not delete the old GameData object.
     *
     * @param pGameData Set up the new GameData object.
     * @return The old GameData object. */
    GameData * setGameData(GameData *pGameData);

    /**
     * Create a new GameScene, so it recreates a game with the given game number and recreates all
     * items.
     *
     * @param iGameNumber The game number which is unique for every item position on a layout. */
    void createNewGameScene(int iGameNumber = -1);

    /**
     * Set the BoardLayout file and therefore load the new one.
     *
     * @param rBoardLayoutFile A reference to the BoardLayout file name.
     * @return True if loading success else false. */
    bool setBoardLayoutPath(QString const &rBoardLayoutPath);
    QString getBoardLayoutPath() const;

    /**
     * Removes all items from the scene and deletes them. */
    void removeItems();

    /**
     * Get the tileset path setup for the GameScene object.
     *
     * @return The tileset path. */
    QString getTilesetPath() const;

    /**
     * Get the background path setup for the GameScene object.
     *
     * @return The background path. */
    QString getBackgroundPath() const;

    /**
     * Override the function from QGraphicsScene. */
    void addItem(GameItem *pGameItem);

    /**
     * Get the actual GameData object. */
    GameData * getGameData();

public slots:
    /**
     * Sets the tileset path for all GameWidgets, that are connected to this GameScene object and
     * tries to load it.
     *
     * @param rTilesetPath The path to the tileset.
     * @return True if set for all GameWidgets success, else false. */
    bool setTilesetPath(QString const &rTilesetPath);

    /**
     * Sets the background path for all GameWidgets, that are connected to this GameScene object and
     * tries to load it.
     *
     * @param rBackgroundPath The path to the background.
     * @return True if set for all GameWidgets success, else false. */
    bool setBackgroundPath(QString const &rBackgroundPath);

signals:
    /**
     * The background of the scene changed. Every view should connect to it, in order to change the
     * background on the view.
     *
     * @param rBackgroundPath The background path to set up. */
    void backgroundChanged(QString const &rBackgroundPath);

    /**
     * The tileset of the scene changed. Every view should connect to ot, in order to change the
     * tileset on the view.
     *
     * @param rTilesetPath The tileset path to set up. */
    void tilesetChanged(QString const &rTilesetPath);

    /**
     * A new game scene was created */
    void newGameSceneCreated();

    /**
     * New item was added to the scene.
     *
     * @param pItem The graphics added, that was added to the scene. */
    void itemAdded(GameItem *pGameItem);

    /**
     * All items are added to the board layout. */
    void itemsAddedFromBoardLayout();

private:
    /**
     * Updates the game scene object by deleting all items and recreate them related to the
     * layout. */
    void updateGameScene();

    /**
     * Loads the board layout from the given path.
     *
     * @return True if loading success, else false. */
    bool loadBoardLayoutFromPath();

    /**
     * Creates the items from the board layout and add them. */
    void addItemsFromBoardLayout();

    long m_lGameNumber;
    int m_iCheatsUsed;

    GameData *m_pGameData;
    KMahjonggLayout *m_pBoardLayout;

    QString *m_pBoardLayoutPath;
    QString *m_pBackgroundPath;
    QString *m_pTilesetPath;
};


#endif // GAMESCENE_H
