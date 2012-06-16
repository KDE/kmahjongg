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

    int iSelectedItems = selectedItems().size();

    pMouseEvent->accept();
    pGameItem->setSelected(true);

    // Update the number of selected items.
    iSelectedItems = selectedItems().size();

    // If got a 2 selected items after selection, emit a signal.
    if (iSelectedItems == 2) {
        // TODO: Emit a signal, that we have two selected items.
        emit pairSelected(m_pFirstSelectedItem, m_pSecondSelectedItem);

        // Now it could be, that the selection of the items changes between 0 and 1 selection. So if
        // any selection exist, the item should be the new m_pFirstSelectedItem.
        iSelectedItems = selectedItems().size();

        if (iSelectedItems == 1) {
            m_pFirstSelectedItem = selectedItems().at(0);
        }

        // If something went wrong and more than one item is selected, clear all selections.
        if (iSelectedItems > 1) {
            // clearSelection();
        }
    } else {
        m_pFirstSelectedItem = pGameItem;
    }
}
