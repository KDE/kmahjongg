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

void GameScene::addItem(GameItem *pGameItem)
{
    QGraphicsScene::addItem(pGameItem);

    // Build a connection to recognize wheather the position of the tile changes.
//     connect(pGameItem, SINGAL(positionChanged(GameItem*)), this,
//         SLOT(addItemToPositionArray(GameItem*)));

    // Call the slot function for the first time.
    addItemToPositionArray(pGameItem);
}

void GameScene::removeItem(GameItem *pGameItem)
{
    m_pGameItemsArray[pGameItem->getXPosition()][pGameItem->getYPosition()]
        [pGameItem->getZPosition()] = NULL;

    QGraphicsScene::removeItem(pGameItem);
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

QList<GameItem *> GameScene::selectedItems() const
{
    QList<QGraphicsItem *> originalList = QGraphicsScene::selectedItems();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); i++) {
        tmpList.append(dynamic_cast<GameItem *>(originalList.at(i)));
    }

    return tmpList;
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent)
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

    pMouseEvent->accept();
    pGameItem->setSelected(true);
}
