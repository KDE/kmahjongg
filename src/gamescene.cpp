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

// own
#include "gamescene.h"

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QList>

// KMahjongg
#include "gamedata.h"
#include "gameitem.h"
#include "gameview.h"
#include "kmahjongglayout.h"
#include "gamebackground.h"
#include "gameremovedtiles.h"


GameScene::GameScene(QObject * parent)
    : QGraphicsScene(parent)
    , m_pFirstSelectedItem(nullptr)
    , m_pSecondSelectedItem(nullptr)
    , m_gameBackground(nullptr)
    , m_gameRemovedTiles(nullptr)
{
    initializeGameItemsArray();
}

GameScene::~GameScene()
{
}

void GameScene::clearGameItems()
{
    // Remove all GameItem objects and DON'T remove GameBackground object.
    QList<QGraphicsItem*> items = QGraphicsScene::items();
    for (int i = 0; i < items.size(); i++) {
        GameItem *gameItem = dynamic_cast<GameItem*>(items.at(i));
        if (gameItem != nullptr) {
            QGraphicsScene::removeItem(gameItem);
        }
    }
    initializeGameItemsArray();

    m_pFirstSelectedItem = nullptr;
    m_pSecondSelectedItem = nullptr;
}

void GameScene::clear()
{
    QGraphicsScene::clear();
    initializeGameItemsArray();

    m_pFirstSelectedItem = nullptr;
    m_pSecondSelectedItem = nullptr;
}

void GameScene::initializeGameItemsArray()
{
    // initialize all array pointers with nullptr.
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        for (int j = 0; j < BOARD_HEIGHT; ++j) {
            for (int k = 0; k < BOARD_DEPTH; ++k) {
                m_pGameItemsArray[i][j][k] = nullptr;
            }
        }
    }
}

void GameScene::setRemovedTilesItem(GameRemovedTiles * gameRemovedTiles)
{
    // If a removedtiles object already exist, delete it from scene
    if (m_gameRemovedTiles != nullptr) {
        QGraphicsScene::removeItem(m_gameRemovedTiles);
    }

    m_gameRemovedTiles = gameRemovedTiles;
    QGraphicsScene::addItem(gameRemovedTiles);
}

void GameScene::setBackgroundItem(GameBackground * gameBackground)
{
    // If a background exist, delete it from scene
    if (m_gameBackground != nullptr) {
        QGraphicsScene::removeItem(m_gameBackground);
    }

    m_gameBackground = gameBackground;
    QGraphicsScene::addItem(gameBackground);
}

void GameScene::addItem(GameItem * gameItem)
{
    QGraphicsScene::addItem(gameItem);

    // Add the item to the position array.
    addItemToPositionArray(gameItem);
}

void GameScene::removeItem(GameItem * gameItem)
{
    USHORT x = gameItem->getGridPosX();
    USHORT y = gameItem->getGridPosY();
    USHORT z = gameItem->getGridPosZ();
    m_pGameItemsArray[x][y][z] = nullptr;

    QGraphicsScene::removeItem(gameItem);
}

void GameScene::removeItem(POSITION const & stItemPos)
{
    GameItem * gameItem = m_pGameItemsArray[stItemPos.x][stItemPos.y][stItemPos.z];

    if (gameItem != nullptr) {
        removeItem(gameItem);
    }
}

void GameScene::addItemToPositionArray(GameItem * const gameItem)
{
    // Take a look, if the place is already taken.
    USHORT x = gameItem->getGridPosX();
    USHORT y = gameItem->getGridPosY();
    USHORT z = gameItem->getGridPosZ();
    if (m_pGameItemsArray[x][y][z] == nullptr) {
        m_pGameItemsArray[x][y][z] = gameItem;
    }
}

GameItem * GameScene::getItemOnGridPos(int x, int y, int z)
{
    // Test for range
    if ((x < 0 || x > BOARD_WIDTH - 1) || (y < 0 || y > BOARD_HEIGHT - 1) || (z < 0 || z > BOARD_DEPTH - 1)) {
        return nullptr;
    }

    return m_pGameItemsArray[x][y][z];
}

GameItem * GameScene::getItemOnGridPos(POSITION & stItemPos)
{
    return getItemOnGridPos(stItemPos.x, stItemPos.y, stItemPos.z);
}

bool GameScene::isItemOnGridPos(int x, int y, int z) const
{
    // Test for range
    if ((x < 0 || x > BOARD_WIDTH - 1) || (y < 0 || y > BOARD_HEIGHT - 1) || (z < 0 || z > BOARD_DEPTH - 1)) {
        return false;
    }

    return !(m_pGameItemsArray[x][y][z] == nullptr);
}

QList<GameItem *> GameScene::selectedItems() const
{
    QList<QGraphicsItem *> originalList = QGraphicsScene::selectedItems();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); ++i) {
        tmpList.append(dynamic_cast<GameItem *>(originalList.at(i)));
    }

    return tmpList;
}

QList<GameItem *> GameScene::items() const
{
    QList<QGraphicsItem *> originalList = QGraphicsScene::items();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); ++i) {
        GameItem * gameItem = dynamic_cast<GameItem *>(originalList.at(i));
        if (gameItem != nullptr) {
            tmpList.append(gameItem);
        }
    }

    return tmpList;
}

bool GameScene::isSelectable(const GameItem * const gameItem) const
{
    USHORT x = gameItem->getGridPosX();
    USHORT y = gameItem->getGridPosY();
    USHORT z = gameItem->getGridPosZ();

    // Test for items above...

    // We need one layer above.
    ++z;

    for (int i = x - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            // If there is a stone on the position, the item is not selectable.
            if (isItemOnGridPos(i, j, z)) {
                return false;
            }
        }
    }

    // Test for items beside...

    // Go back to the layer of the item.
    --z;

    bool sideFree = true;
    for (int i = x - 2; i <= x + 2; i += 4) {
        for (int j = y - 1; j <= y + 1; ++j) {
            // If there is one item on the side, it is no longer free.
            if (isItemOnGridPos(i, j, z)) {
                sideFree = false;
            }
        }

        // If a side is free the item is selectable.
        if (sideFree == true) {
            return true;
        } else {
            sideFree = true;
        }
    }

    return false;
}

void GameScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // Swallow this event.  A double-click causes a mousePressEvent() AND a
    // mouseDoubleClickEvent().  The second event could cause a tile that is
    // NOT removable to become selected (by default), allowing it to be removed
    // illegally (ie. it has a tile on each side or is half under another tile).
    mouseEvent->accept(); // Double-click ==> single-click in KMahjongg.
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // N.B. Event occurs when there is a click OR double-click with ANY button.
    GameItem * gameItem = dynamic_cast<GameItem *>(
        itemAt(mouseEvent->scenePos().x(),
        mouseEvent->scenePos().y(), QTransform())
    );

    // An item was clicked.
    if (gameItem != nullptr) {
        // If we click on a shadow of the actual item, we have to correct the 
        // clicking position, in order to simulate a transparent shadow.
        if (gameItem->isShadow(mouseEvent->scenePos() - gameItem->pos())) {
            gameItem = dynamic_cast<GameItem *>(
                itemAt(mouseEvent->scenePos().x() + gameItem->getShadowDeltaX(),
                mouseEvent->scenePos().y() + gameItem->getShadowDeltaY(), 
                QTransform())
            );
        }
    }

    // No item was clicked.
    if (gameItem == nullptr) {
        Q_EMIT clearSelectedTile();
        mouseEvent->ignore();
        return;
    }

    // If the item is selectable go on with selection.
    if (isSelectable(gameItem)) {
        clearSelection();
        gameItem->setSelected(true);
        mouseEvent->accept();
    } else {
        Q_EMIT clearSelectedTile();
    }
}

void GameScene::wheelEvent(QGraphicsSceneWheelEvent * mouseEvent)
{
    if (mouseEvent->delta() < 0) {
        Q_EMIT rotateCW();
    } else {
        Q_EMIT rotateCCW();
    }
}
