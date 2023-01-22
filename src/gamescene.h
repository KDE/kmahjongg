/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMESCENE_H
#define GAMESCENE_H

// Qt
#include <QGraphicsScene>

// KMahjongg
#include "kmtypes.h"

constexpr int BOARD_WIDTH = 36;
constexpr int BOARD_HEIGHT = 16;
constexpr int BOARD_DEPTH = 5;


// Forward declarations...
class GameItem;
class GameBackground;
class GameRemovedTiles;

/**
 * Holds and manages all GameItems.
 *
 * @author Christian Krippendorf */
class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GameScene(QObject * parent = nullptr);
    ~GameScene() override;

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
    GameItem * getItemOnGridPos(POSITION & stItemPos);

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
     * Clear only the GameItem objects
     */
    void clearGameItems();

    /**
     * Override from QGraphicsScene. */
    void addItem(GameItem * gameItem);

    /**
     * Override from QGraphicsScene. */
    void removeItem(GameItem * gameItem);

    /**
     * Override from QGraphicsScene with POSITION parameter.
     *
     * @param stItemPos The item position. */
    void removeItem(POSITION const & stItemPos);

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> selectedItems() const;

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> items() const;

    /**
     * Do not clear selection on background click.
     *
     * @param clearSelectionBackground the value is true if selection should be cleared */
    void setClearSelectionOnBackgroundClick(bool clearSelectionBackground);

    /**
     *
     * @return true if selection should be cleared, else false */
    bool getClearSelectionOnBackgroundClick() const;

    /**
     * Override from QGraphicsScene. */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent) override;

    /**
     * Override from QGraphicsScene. */
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) override;

    /**
     * Override from QGraphicsScene.
     * Mouse wheel rotates view. */
    void wheelEvent(QGraphicsSceneWheelEvent * mouseEvent) override;

    /**
     * Test if the item is selectable or not.
     *
     * @param pameItem The game item to test.
     * @return True if selectable else false. */
    bool isSelectable(const GameItem * const pameItem) const;

    /**
     * Set a background
     * @param gameBackground The background object
     */
    void setBackgroundItem(GameBackground * gameBackground);

    /**
     * Set a removedtiles item
     * @param gameRemovedTiles The removedtiles object
     */
    void setRemovedTilesItem(GameRemovedTiles * gameRemovedTiles);

Q_SIGNALS:
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

    GameItem * m_pGameItemsArray[BOARD_WIDTH][BOARD_HEIGHT][BOARD_DEPTH];
    GameItem * m_pFirstSelectedItem;
    GameItem * m_pSecondSelectedItem;

    GameBackground * m_gameBackground;
    GameRemovedTiles * m_gameRemovedTiles;

    bool m_clearSelectionBackground;
};

#endif // GAMESCENE_H
