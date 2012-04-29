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
#include "GameItem.h"
#include "GameWidget.h"
#include "kmahjongglayout.h"

#include <KRandom>
#include <KDebug>

#include <QList>


GameScene::GameScene(GameData *pGameData, QObject *pParent)
    : QGraphicsScene(pParent),
    m_pBoardLayoutPath(new QString()),
    m_pBackgroundPath(new QString()),
    m_pTilesetPath(new QString()),
    m_pBoardLayout(new KMahjonggLayout()),
    m_pGameData(0)
{
}

GameScene::~GameScene()
{
    delete m_pBoardLayoutPath;
    delete m_pBackgroundPath;
    delete m_pTilesetPath;
    delete m_pBoardLayout;
}

bool GameScene::setBoardLayoutPath(QString const &rBoardLayoutPath)
{
    *m_pBoardLayoutPath = rBoardLayoutPath;

    loadBoardLayoutFromPath();

    // We need to create a new GameData object.
    delete m_pGameData;
    m_pGameData = new GameData(m_pBoardLayout->board());

    // Give the new GameData object to every widget.
    QList<QGraphicsView *> tmpViews = views();

    for (int iI = 0; iI < tmpViews.size(); iI++) {
        GameWidget *pGameWidget = dynamic_cast<GameWidget *>(tmpViews.at(iI));
        pGameWidget->setGameData(m_pGameData);
    }

    return true;
}

QString GameScene::getBoardLayoutPath() const
{
    return *m_pBoardLayoutPath;
}

bool GameScene::setTilesetPath(QString const &rTilesetPath)
{
    QList<QGraphicsView *> tmpViews = views();

    for (int iI = 0; iI < tmpViews.size(); iI++) {
        GameWidget *pGameWidget = dynamic_cast<GameWidget *>(tmpViews.at(iI));

        if (!pGameWidget->setTilesetPath(rTilesetPath)) {
            return false;
        }
    }

    return true;
}

bool GameScene::setBackgroundPath(QString const &rBackgroundPath)
{
    QList<QGraphicsView *> tmpViews = views();

    for (int iI = 0; iI < tmpViews.size(); iI++) {
        GameWidget *pGameWidget = dynamic_cast<GameWidget *>(tmpViews.at(iI));

        if (!pGameWidget->setBackgroundPath(rBackgroundPath)) {
            return false;
        }
    }

    return true;
}


bool GameScene::loadBoardLayoutFromPath()
{
    if (m_pBoardLayout->load(*m_pBoardLayoutPath)) {
        return true;
    } else {
        if (m_pBoardLayout->loadDefault()) {
            return false;
        } else {
            return false;
        }
    }
}

void GameScene::removeItems()
{
    // Delete all items in GameScene.
    QList<QGraphicsItem *> tmpItems = items();

    while (!tmpItems.isEmpty()) {
        QGraphicsItem *pItem = tmpItems.takeFirst();
        removeItem(pItem);
        delete pItem;
    }
}

GameData * GameScene::setGameData(GameData *pGameData)
{
    GameData *pTmpGameData = m_pGameData;
    m_pGameData = pGameData;

    return pTmpGameData;
}

void GameScene::createNewGameScene(int iGameNumber)
{
    kDebug() << "Create new game with game number: " << iGameNumber;

    // Create a random game number, if no one was given.
    if (iGameNumber == -1) {
        m_lGameNumber = KRandom::random();
    } else {
        m_lGameNumber = iGameNumber;
    }

    m_pGameData->random.setSeed(m_lGameNumber);

    // Translate m_pGameData->Map to an array of POSITION data.  We only need to
    // do this once for each new game.
    m_pGameData->generateTilePositions();

    // Now use the tile position data to generate tile dependency data.
    // We only need to do this once for each new game.
    m_pGameData->generatePositionDepends();

    // TODO: This is really bad... the generatedStartPosition2-function should never fail!!!!
    // Now try to position tiles on the board, 64 tries max.
    for (short sNr = 0; sNr < 64; sNr++) {
        if (m_pGameData->generateStartPosition2()) {
            // No cheats are used until now.
            m_iCheatsUsed = 0;

            // Throw a signal, that a new game was calculated.
            emit newGameSceneCreated();

            addItemsFromBoardLayout();

            return;
        }
    }
}

void GameScene::addItemsFromBoardLayout()
{
    // Remove all existing items.
    removeItems();

    // Create the items and add them to the scene.
    for (int iZ = 0; iZ < m_pGameData->m_depth; iZ++) {
        for (int iY = m_pGameData->m_height - 1; iY >= 0; iY--) {
            for (int iX = m_pGameData->m_width - 1; iX >= 0; iX--) {

                // Skip if no tile should be displayed on this position.
                if (!m_pGameData->tilePresent(iZ, iY, iX)) {
                    continue;
                }

                bool bSelected = false;

                if (m_pGameData->HighlightData(iZ, iY, iX)) {
                    bSelected = true;
                }

                GameItem *item = new GameItem(bSelected);
                item->setPosition(iX, iY, iZ);
                addItem(item);

                // We need to decide whether the item is selectable or not.

                // If another item overlay this item, continue.
                if (m_pGameData->tilePresent(iZ + 1, iY - 1, iX - 1) ||
                    m_pGameData->tilePresent(iZ + 1, iY - 1, iX) ||
                    m_pGameData->tilePresent(iZ + 1, iY - 1, iX + 1) ||
                    m_pGameData->tilePresent(iZ + 1, iY, iX - 1) ||
                    m_pGameData->tilePresent(iZ + 1, iY, iX) ||
                    m_pGameData->tilePresent(iZ + 1, iY, iX + 1) ||
                    m_pGameData->tilePresent(iZ + 1, iY + 1, iX - 1) ||
                    m_pGameData->tilePresent(iZ + 1, iY + 1, iX) ||
                    m_pGameData->tilePresent(iZ + 1, iY + 1, iX + 1)) {
                    continue;
                }

                // Find items beside this one.
                if ((m_pGameData->tilePresent(iZ, iY, iX - 2) ||
                     m_pGameData->tilePresent(iZ, iY - 1, iX - 2) ||
                     m_pGameData->tilePresent(iZ, iY + 1, iX - 2)) &&
                    (m_pGameData->tilePresent(iZ, iY, iX + 2) ||
                     m_pGameData->tilePresent(iZ, iY - 1, iX + 2) ||
                     m_pGameData->tilePresent(iZ, iY + 1, iX + 2))) {
                    continue;
                }

                item->setFlag(QGraphicsItem::ItemIsSelectable);
            }
        }
    }

    emit itemsAddedFromBoardLayout();
}

void GameScene::addItem(GameItem *pGameItem)
{
    QGraphicsScene::addItem(pGameItem);
    emit itemAdded(pGameItem);
}

QString GameScene::getTilesetPath() const
{
    return *m_pTilesetPath;
}

QString GameScene::getBackgroundPath() const
{
    return *m_pBackgroundPath;
}

GameData * GameScene::getGameData()
{
    return m_pGameData;
}
