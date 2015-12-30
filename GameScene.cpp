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

#include "GameScene.h"
#include "GameData.h"
#include "GameView.h"
#include "GameItem.h"
#include "kmahjongglayout.h"

#include <QGraphicsSceneMouseEvent>
#include <QList>


GameScene::GameScene(QObject *pParent)
    : QGraphicsScene(pParent),
    m_pFirstSelectedItem(nullptr),
    m_pSecondSelectedItem(nullptr)
{
    initializeGameItemsArray();
}

GameScene::~GameScene()
{
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
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            for (int k = 0; k < BOARD_DEPTH; k++) {
                m_pGameItemsArray[i][j][k] = nullptr;
            }
        }
    }
}

void GameScene::addItem(GameItem * pGameItem)
{
    QGraphicsScene::addItem(pGameItem);

    // Add the item to the position array.
    addItemToPositionArray(pGameItem);
}

void GameScene::removeItem(GameItem * pGameItem)
{
    USHORT iX = pGameItem->getGridPosX();
    USHORT iY = pGameItem->getGridPosY();
    USHORT iZ = pGameItem->getGridPosZ();
    m_pGameItemsArray[iX][iY][iZ] = nullptr;

    QGraphicsScene::removeItem(pGameItem);
}

void GameScene::removeItem(POSITION const &stItemPos)
{
    GameItem *pGameItem = m_pGameItemsArray[stItemPos.x][stItemPos.y][stItemPos.e];

    if (pGameItem != nullptr) {
        removeItem(pGameItem);
    }
}

void GameScene::addItemToPositionArray(GameItem * const pGameItem)
{
    // Take a look, if the place is already taken.
    USHORT iX = pGameItem->getGridPosX();
    USHORT iY = pGameItem->getGridPosY();
    USHORT iZ = pGameItem->getGridPosZ();
    if (m_pGameItemsArray[iX][iY][iZ] == nullptr) {
        m_pGameItemsArray[iX][iY][iZ] = pGameItem;
    }
}

GameItem * GameScene::getItemOnGridPos(int iX, int iY, int iZ)
{
    // Test for range
    if ((iX < 0 || iX > BOARD_WIDTH - 1) ||
        (iY < 0 || iY > BOARD_HEIGHT - 1) ||
        (iZ < 0 || iZ > BOARD_DEPTH - 1)) {
        return nullptr;
    }

    return m_pGameItemsArray[iX][iY][iZ];
}

GameItem * GameScene::getItemOnGridPos(POSITION &stItemPos)
{
    return getItemOnGridPos(stItemPos.x, stItemPos.y, stItemPos.e);
}

bool GameScene::isItemOnGridPos(int iX, int iY, int iZ) const
{
        // Test for range
    if ((iX < 0 || iX > BOARD_WIDTH - 1) ||
        (iY < 0 || iY > BOARD_HEIGHT - 1) ||
        (iZ < 0 || iZ > BOARD_DEPTH - 1)) {
        return false;
    }

    return !(m_pGameItemsArray[iX][iY][iZ] == nullptr);
}

QList<GameItem *> GameScene::selectedItems() const
{
    QList<QGraphicsItem *> originalList = QGraphicsScene::selectedItems();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); i++) {
        tmpList.append(dynamic_cast<GameItem *>(originalList.at(i)));
    }

    return tmpList;
}

QList<GameItem *> GameScene::items() const
{
    QList<QGraphicsItem *> originalList = QGraphicsScene::items();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); i++) {
        tmpList.append(dynamic_cast<GameItem *>(originalList.at(i)));
    }

    return tmpList;
}

bool GameScene::isSelectable(const GameItem * const pGameItem) const
{
    USHORT iX = pGameItem->getGridPosX();
    USHORT iY = pGameItem->getGridPosY();
    USHORT iZ = pGameItem->getGridPosZ();

    // Test for items above...

    // We need one layer above.
    iZ++;

    for (int i = iX - 1; i <= iX + 1; i++) {
        for (int j = iY - 1; j <= iY + 1; j++) {
            // If there is a stone on the position, the item is not selectable.
            if (isItemOnGridPos(i, j, iZ)) {
                return false;
            }
        }
    }

    // Test for items beside...

    // Go back to the layer of the item.
    iZ--;

    bool bSideFree = true;
    for (int i = iX - 2; i <= iX + 2; i += 4) {
        for (int j = iY - 1; j <= iY + 1; j++) {
            // If there is one item on the side, it is no longer free.
            if (isItemOnGridPos(i, j, iZ)) {
                bSideFree = false;
            }
        }

        // If a side is free the item is selectable.
        if (bSideFree == true) {
            return true;
        } else {
            bSideFree = true;
        }
    }

    return false;
}

void GameScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
    // Swallow this event.  A double-click causes a mousePressEvent() AND a
    // mouseDoubleClickEvent().  The second event could cause a tile that is
    // NOT removable to become selected (by default), allowing it to be removed
    // illegally (ie. it has a tile on each side or is half under another tile).
    pMouseEvent->accept(); // Double-click ==> single-click in KMahjongg.
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
    // N.B. Event occurs when there is a click OR double-click with ANY button.
    GameItem * pGameItem = dynamic_cast <GameItem *>(itemAt(pMouseEvent->scenePos().x(),
        pMouseEvent->scenePos().y(), QTransform()));

    // An item was clicked.
    if (pGameItem != nullptr) {
        // If we click on a shadow of the actual item, we have to correct the clicking position, in
        // order to simulate a transparent shadow.
        if (pGameItem->isShadow(pMouseEvent->scenePos() - pGameItem->pos())) {
            pGameItem = dynamic_cast <GameItem *>(itemAt(pMouseEvent->scenePos().x() +
                pGameItem->getShadowDeltaX(), pMouseEvent->scenePos().y() +
                pGameItem->getShadowDeltaY(), QTransform()));
        }
    }

    // No item was clicked.
    if (pGameItem == nullptr) {
        emit clearSelectedTile();
        pMouseEvent->ignore();
        return;
    }

    // If the item is selectable go on with selection.
    if (isSelectable(pGameItem)) {
        clearSelection();
        pGameItem->setSelected(true);
        pMouseEvent->accept();
    } else {
        emit clearSelectedTile();
    }
}

void GameScene::wheelEvent(QGraphicsSceneWheelEvent* pMouseEvent)
{
    if (pMouseEvent->delta() < 0) {
        emit rotateCW();
    } else {
        emit rotateCCW();
    }
}
