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

#include "GameView.h"
#include "GameData.h"
#include "GameScene.h"
#include "GameItem.h"
#include "kmahjongglayout.h"
#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "prefs.h"

#include <KLocale>
#include <KRandom>
#include <KDebug>

#include <QResizeEvent>


GameView::GameView(GameScene *pGameScene, QWidget *pParent)
    : QGraphicsView(pGameScene, pParent),
    m_pGameData(0),
    m_pBoardLayoutPath(new QString()),
    m_pBackgroundPath(new QString()),
    m_pTilesetPath(new QString()),
    m_pBoardLayout(new KMahjonggLayout()),
    m_pBackground(new KMahjonggBackground()),
    m_pTiles(new KMahjonggTileset())
{
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet( "QGraphicsView { border-style: none; }" );

    m_angle = (TileViewAngle) Prefs::angle();
}

GameView::~GameView()
{
    delete m_pBackground;
    delete m_pTiles;
    delete m_pGameData;
}

void GameView::createNewGame(int iGameNumber)
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
            addItemsFromBoardLayout();

            return;
        }
    }
}

void GameView::addItemsFromBoardLayout()
{
    GameScene *pGameScene = dynamic_cast<GameScene *>(scene());

    // Remove all existing items.
    pGameScene->clear();

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
                pGameScene->addItem(item);
                updateItemPictures(item);

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

    updateItemsOrder();
}

void GameView::itemsAddedToScene()
{
    QList<QGraphicsItem *> tmpItems = items();


    int iAngleXFactor = (m_angle == NE || m_angle == SE) ? -1 : 1;
    int iAngleYFactor = (m_angle == NW || m_angle == NE) ? -1 : 1;

    for (int iI = 0; iI < tmpItems.size(); iI++) {
        GameItem *pGameItem = dynamic_cast<GameItem *>(tmpItems.at(iI));

        int iX = pGameItem->getXPosition() - 1;
        int iY = pGameItem->getYPosition() - 1;
        int iZ = pGameItem->getZPosition();

        int iTileWidth = m_pTiles->qWidth();
        int iTileHeight = m_pTiles->qHeight();

        int iTilesWidth = iTileWidth * (m_pGameData->m_width / 2);
        int iTilesHeight = iTileHeight * (m_pGameData->m_height / 2);

        int iXFrame = (width() / 2 - iTilesWidth) / 2;
        int iYFrame = (height() / 2 - iTilesHeight) / 2;

        pGameItem->setPos(iTileWidth / 2 * iX + iXFrame + iZ * iAngleXFactor *
            (m_pTiles->levelOffsetX() / 2), iTileHeight / 2 * iY + iYFrame + iZ * iAngleYFactor *
            (m_pTiles->levelOffsetY() / 2));
    }
}

void GameView::updateItemPictures(GameItem *pGameItem)
{
    QPixmap selPix;
    QPixmap unselPix;
    QPixmap facePix;

    facePix = m_pTiles->tileface(m_pGameData->BoardData(pGameItem->getZPosition(),
        pGameItem->getYPosition(), pGameItem->getXPosition()) - TILE_OFFSET);
    selPix = m_pTiles->selectedTile(SW);
    unselPix = m_pTiles->unselectedTile(SW);

    // Set the background pictures to the item.
    pGameItem->setAngle(SW, &selPix, &unselPix);
    pGameItem->setFace(&facePix);
}

void GameView::updateItemsOrder()
{
    int iZCount = 0;
    int iXStart;
    int iXEnd;
    int iXCounter;
    int iYStart;
    int iYEnd;
    int iYCounter;

    switch (m_angle) {
    case NW:
        kDebug() << "NW";
        iXStart = m_pGameData->m_width - 1;
        iXEnd = 0;
        iXCounter = -1;

        iYStart = 0;
        iYEnd = m_pGameData->m_height;
        iYCounter = 1;
        break;
    case NE:
        kDebug() << "NE";
        iXStart = 0;
        iXEnd = m_pGameData->m_width;
        iXCounter = 1;

        iYStart = 0;
        iYEnd = m_pGameData->m_height;
        iYCounter = 1;
        break;
    case SE:
        kDebug() << "SE";
        iXStart = 0;
        iXEnd = m_pGameData->m_width;
        iXCounter = 1;

        iYStart = m_pGameData->m_height - 1;
        iYEnd = -1;
        iYCounter = -1;
        break;
    case SW:
        kDebug() << "SW";
        iXStart = m_pGameData->m_width - 1;
        iXEnd = -1;
        iXCounter = -1;

        iYStart = m_pGameData->m_height - 1;
        iYEnd = -1;
        iYCounter = -1;
        break;
    }

    GameScene *pGameScene = dynamic_cast<GameScene *>(scene());

    for (int iZ = 0; iZ < m_pGameData->m_depth; iZ++) {
        for (int iY = iYStart; iY != iYEnd; iY = iY + iYCounter) {
            for (int iX = iXStart; iX != iXEnd; iX = iX + iXCounter) {
                GameItem *pGameItem = pGameScene->getItemOnPosition(iX, iY, iZ);

                if (pGameItem == NULL) {
                    continue;
                }

                pGameItem->setZValue(iZCount);
                iZCount++;
            }
        }
    }

    itemsAddedToScene();
}

bool GameView::loadBoardLayoutFromPath()
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

bool GameView::setBoardLayoutPath(QString const &rBoardLayoutPath)
{
    *m_pBoardLayoutPath = rBoardLayoutPath;

    kDebug() << *m_pBoardLayoutPath;

    // Load the new set board layout.
    loadBoardLayoutFromPath();

    // We need to create a new GameData object.
    GameData *pOldGameData = m_pGameData;
    m_pGameData = new GameData(m_pBoardLayout->board());

    // New game data object so set, so delete the old one.
    delete pOldGameData;

    return true;
}

bool GameView::setTilesetPath(QString const &rTilesetPath)
{
    *m_pTilesetPath = rTilesetPath;

    if (m_pTiles->loadTileset(rTilesetPath)) {
        if (m_pTiles->loadGraphics()) {
            resizeTileset(size());

            return true;
        }
    }

    //Tileset or graphics could not be loaded, try default
    if (m_pTiles->loadDefault()) {
        if (m_pTiles->loadGraphics()) {
            resizeTileset(size());
        }
    }

    return false;
}

bool GameView::setBackgroundPath(QString const &rBackgroundPath)
{
    *m_pBackgroundPath = rBackgroundPath;

    if (m_pBackground->load(rBackgroundPath, width(), height())) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();

            return true;
        }
    }

    // Try default
    if (m_pBackground->loadDefault()) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();
        }
    }

    return false;
}

void GameView::setAngle(TileViewAngle angle)
{
    m_angle = angle;
    updateItemsImages();
    updateItemsOrder();
}

TileViewAngle GameView::getAngle() const
{
    return m_angle;
}

void GameView::angleSwitchCCW()
{
    switch (m_angle) {
    case SW:
        m_angle = NW;
        break;
    case NW:
        m_angle = NE;
        break;
    case NE:
        m_angle = SE;
        break;
    case SE:
        m_angle = SW;
        break;
    }

    updateItemsImages();
    updateItemsOrder();
}

void GameView::angleSwitchCW()
{
    switch (m_angle) {
    case SW:
        m_angle = SE;
        break;
    case SE:
        m_angle = NE;
        break;
    case NE:
        m_angle = NW;
        break;
    case NW:
        m_angle = SW;
        break;
    }

    updateItemsImages();
    updateItemsOrder();
}

void GameView::resizeEvent(QResizeEvent *pEvent)
{
    if (pEvent->spontaneous() || m_pGameData == 0) {
        return;
    }

    resizeTileset(pEvent->size());
    m_pBackground->sizeChanged(m_pGameData->m_width / 2, m_pGameData->m_height / 2);

    setSceneRect(0, 0, width(), height());
}

void GameView::resizeTileset(QSize const &rSize)
{
    if (m_pGameData == 0) {
        return;
    }

    QSize newtiles = m_pTiles->preferredTileSize(rSize, m_pGameData->m_width / 2,
        m_pGameData->m_height / 2);

    m_pTiles->reloadTileset(newtiles);

    updateItemsImages();
}

void GameView::updateItemsImages()
{
    QList<QGraphicsItem *> tmpItems = items();

    for (int iI = 0; iI < tmpItems.size(); iI++) {
        GameItem *pGameItem = dynamic_cast<GameItem *>(tmpItems.at(iI));

        QPixmap selPix;
        QPixmap unselPix;
        QPixmap facePix;

        facePix = m_pTiles->tileface(m_pGameData->BoardData(pGameItem->getZPosition(),
            pGameItem->getYPosition(), pGameItem->getXPosition()) - TILE_OFFSET);
        selPix = m_pTiles->selectedTile(m_angle);
        unselPix = m_pTiles->unselectedTile(m_angle);

        // Set the background pictures to the item.
        pGameItem->setAngle(m_angle, &selPix, &unselPix);
        pGameItem->setFace(&facePix);
    }

    itemsAddedToScene();

    // Repaint the view.
    update();
}

void GameView::setStatusText(QString const &rText)
{
    emit statusTextChanged(rText, m_lGameNumber);
}

void GameView::updateBackground()
{
    QPalette palette;
    palette.setBrush(backgroundRole(), m_pBackground->getBackground());
    setPalette(palette);
    setAutoFillBackground(true);
}

void GameView::setGameData(GameData *pGameData)
{
    m_pGameData = pGameData;
}

GameData * GameView::getGameData()
{
    return m_pGameData;
}

QString GameView::getTilesetPath()
{
    return *m_pTilesetPath;
}

QString GameView::getBackgroundPath()
{
    return *m_pBackgroundPath;
}

QString GameView::getBoardLayoutPath()
{
    return *m_pBoardLayoutPath;
}
