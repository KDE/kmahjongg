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

#include "KmTypes.h"

#define BOARD_WIDTH 32
#define BOARD_HEIGHT 16
#define BOARD_DEPH 5


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
    /**
     * Constructor
     *
     * @param pParent The parent object. */
    GameScene(GameData *pGameData = 0, QObject *pParent = 0);

    /**
     * Destructor */
    ~GameScene();

    /**
     * Return the GameItem on the given grid position.
     *
     * @param iX The x position of the item.
     * @param iY The y position of the item.
     * @param iZ The z position of the item.
     *
     * @return The GameItem object or null if no one was found. */
    GameItem * getItemOnGridPos(int iX, int iY, int iZ);

    /**
     * Override from QGraphicsScene. */
    void clear();

    /**
     * Override from QGraphicsScene. */
    void addItem(GameItem * pGameItem);

    /**
     * Override from QGraphicsScene. */
    void removeItem(GameItem * pGameItem);

    /**
     * Override from QGraphicsScene with POSITION parameter.
     *
     * @param stItemPos The item position. */
    void removeItem(POSITION & stItemPos);

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> selectedItems() const;

    /**
     * Override from QGraphicsScene. */
    QList<GameItem *> items() const;

    /**
     * Override from QGraphicsScene. */
    void mousePressEvent(QGraphicsSceneMouseEvent * pMouseEvent);

    /**
     * Override from QGraphicsScene. */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * pMouseEvent);

    /**
     * Test if the item is selectable or not.
     *
     * @param pGameItem The game item to test.
     * @return True if selectable else false. */
    bool isSelectable(GameItem * pGameItem);

private:
    /**
     * Initialize the m_pGameItemsArray. */
    void initializeGameItemsArray();

    /**
     * Adds an item to the positions array.
     *
     * @param pGameItem THe game item to add to array. */
    void addItemToPositionArray(GameItem *pGameItem);

    GameItem *m_pGameItemsArray[BOARD_WIDTH][BOARD_HEIGHT][BOARD_DEPH];
    GameItem *m_pFirstSelectedItem;
    GameItem *m_pSecondSelectedItem;
};


#endif // GAMESCENE_H
