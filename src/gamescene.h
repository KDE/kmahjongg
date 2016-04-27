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

// Qt
#include <QGraphicsScene>

// KMahjongg
#include "kmtypes.h"

#define BOARD_WIDTH 36
#define BOARD_HEIGHT 16
#define BOARD_DEPTH 5


// Forward declarations...
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
    explicit GameScene(QObject *parent = 0);
    ~GameScene();

    /**
     * Return the GameItem on the given grid position.
     *
     * @param x The x position of the item.
     * @param y The y position of the item.
     * @param z The z position of the item.
     *
     * @return The GameItem object or null if no one was found. */
    GameItem * getItemOnGridPos(int x, int y, int z);

    /**
     * Return the GameItem on the given grid position.
     *
     * @param stItemPos The position struct.
     *
     * @return The GameItem object or null if no one was found. */
    GameItem * getItemOnGridPos(POSITION &stItemPos);

    /**
     * Test whether a item exist on the given position or not.
     *
     * @param x The x position of the item.
     * @param y The y position of the item.
     * @param z The z position of the item.
     *
     * @return True if GameItem object was found, else false. */
    bool isItemOnGridPos(int x, int y, int z) const;

    /**
     * Override from QGraphicsScene. */
    void clear();

    /**
     * Override from QGraphicsScene. */
    void addItem(GameItem *gameItem);

    /**
     * Override from QGraphicsScene. */
    void removeItem(GameItem *gameItem);

    /**
     * Override from QGraphicsScene with POSITION parameter.
     *
     * @param stItemPos The item position. */
    void removeItem(POSITION const &stItemPos);

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> selectedItems() const;

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> items() const;

    /**
     * Override from QGraphicsScene. */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /**
     * Override from QGraphicsScene. */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /**
     * Override from QGraphicsScene.
     * Mouse wheel rotates view. */
    void wheelEvent(QGraphicsSceneWheelEvent *mouseEvent);

    /**
     * Test if the item is selectable or not.
     *
     * @param pameItem The game item to test.
     * @return True if selectable else false. */
    bool isSelectable(const GameItem * const pameItem) const;

signals:
    void rotateCW();
    void rotateCCW();
    void clearSelectedTile();

private:
    /**
     * Initialize the m_pGameItemsArray. */
    void initializeGameItemsArray();

    /**
     * Adds an item to the positions array.
     *
     * @param pameItem THe game item to add to array. */
    void addItemToPositionArray(GameItem * const pameItem);

    GameItem *m_pGameItemsArray[BOARD_WIDTH][BOARD_HEIGHT][BOARD_DEPTH];
    GameItem *m_pFirstSelectedItem;
    GameItem *m_pSecondSelectedItem;
};

#endif // GAMESCENE_H
