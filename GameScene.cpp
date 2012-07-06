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

#include <KRandom>
#include <KDebug>

#include <QGraphicsSceneMouseEvent>
#include <QList>


GameScene::GameScene(GameData *pGameData, QObject *pParent)
    : QGraphicsScene(pParent),
    m_pFirstSelectedItem(NULL),
    m_pSecondSelectedItem(NULL)
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

    m_pFirstSelectedItem = NULL;
    m_pSecondSelectedItem = NULL;
}

void GameScene::initializeGameItemsArray()
{
    // initialize all array pointers with NULL.
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            for (int k = 0; k < BOARD_DEPH; k++) {
                m_pGameItemsArray[i][j][k] = NULL;
            }
        }
    }
}

void GameScene::addItem(GameItem * pGameItem)
{
    QGraphicsScene::addItem(pGameItem);

    // Build a connection to recognize wheather the position of the tile changes.
//     connect(pGameItem, SINGAL(positionChanged(GameItem*)), this,
//         SLOT(addItemToPositionArray(GameItem*)));

    // Call the slot function for the first time.
    addItemToPositionArray(pGameItem);
}

void GameScene::removeItem(GameItem * pGameItem)
{
    m_pGameItemsArray[pGameItem->getGridPosX()][pGameItem->getGridPosY()]
        [pGameItem->getGridPosZ()] = NULL;

    QGraphicsScene::removeItem(pGameItem);
}

void GameScene::removeItem(POSITION & stItemPos)
{
    GameItem * pGameItem = m_pGameItemsArray[stItemPos.x][stItemPos.y][stItemPos.e];

    if (pGameItem != NULL) {
        removeItem(pGameItem);
    }
}

void GameScene::addItemToPositionArray(GameItem * pGameItem)
{
    // Take a look, if the place is already taken.
    if (m_pGameItemsArray[pGameItem->getGridPosX()][pGameItem->getGridPosY()]
        [pGameItem->getGridPosZ()] == NULL) {
        m_pGameItemsArray[pGameItem->getGridPosX()][pGameItem->getGridPosY()]
        [pGameItem->getGridPosZ()] = pGameItem;
    }
}

GameItem * GameScene::getItemOnGridPos(int iX, int iY, int iZ)
{
    // Test for range
    if ((iX < 0 || iX > BOARD_WIDTH - 1) ||
        (iY < 0 || iY > BOARD_HEIGHT - 1) ||
        (iZ < 0 || iZ > BOARD_DEPH - 1)) {
        return NULL;
    }

    return m_pGameItemsArray[iX][iY][iZ];
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

bool GameScene::isSelectable(GameItem * pGameItem)
{
    int iX = pGameItem->getGridPosX();
    int iY = pGameItem->getGridPosY();
    int iZ = pGameItem->getGridPosZ();

    // Test for items above...

    // We need one layer above.
    iZ++;

    for (int i = iX - 1; i <= iX + 1; i++) {
        for (int j = iY - 1; j <= iY + 1; j++) {
            // If there is a stone on the position, the item is not selectable.
            if (getItemOnGridPos(i, j, iZ) != NULL) {
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
            if (getItemOnGridPos(i, j, iZ) != NULL) {
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

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
    GameItem * pGameItem = dynamic_cast <GameItem *>(itemAt(pMouseEvent->scenePos().x(),
        pMouseEvent->scenePos().y()));

    // No item was clicked.
    if (pGameItem != NULL) {
        // If we click on a shadow of the actual item, we have to correct the clicking position, in
        // order to simulate a transparent shadow.
        if (pGameItem->isShadow(pMouseEvent->scenePos() - pGameItem->pos())) {
            pGameItem = dynamic_cast <GameItem *>(itemAt(pMouseEvent->scenePos().x() +
                pGameItem->getShadowDeltaX(), pMouseEvent->scenePos().y() +
                pGameItem->getShadowDeltaY()));
        }
    }

    // No item was clicked.
    if (pGameItem == NULL) {
        pMouseEvent->ignore();
        return;
    }

    // If the item is selectable go on with selection.
    if (isSelectable(pGameItem)) {
        clearSelection();
        pGameItem->setSelected(true);
        pMouseEvent->accept();
    }
}

void GameScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
    pMouseEvent->ignore();
}
