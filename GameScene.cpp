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

#include <QList>


GameScene::GameScene(GameData *pGameData, QObject *pParent)
    : QGraphicsScene(pParent)
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

void GameScene::addItem(GameItem *pGameItem)
{
    QGraphicsScene::addItem(pGameItem);

    // Build a connection to recognize wheather the position of the tile changes.
//     connect(pGameItem, SINGAL(positionChanged(GameItem*)), this,
//         SLOT(addItemToPositionArray(GameItem*)));

    // Call the slot function for the first time.
    addItemToPositionArray(pGameItem);
}

void GameScene::addItemToPositionArray(GameItem *pGameItem)
{
    // Take a look, if the place is already taken.
    if (m_pGameItemsArray[pGameItem->getXPosition()][pGameItem->getYPosition()]
        [pGameItem->getZPosition()] == NULL) {
        m_pGameItemsArray[pGameItem->getXPosition()][pGameItem->getYPosition()]
            [pGameItem->getZPosition()] = pGameItem;
    }
}

GameItem * GameScene::getItemOnPosition(int &iX, int &iY, int &iZ)
{
    return m_pGameItemsArray[iX][iY][iZ];
}
