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

#include "GameWidget.h"
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


GameWidget::GameWidget(GameScene *pGameScene, QWidget *pParent)
    : QGraphicsView(pGameScene, pParent),
    m_pGameData(0)
{
    m_angle = (TileViewAngle) Prefs::angle();

    // Create tiles...
    m_pTiles = new KMahjonggTileset();

    // Load background
    m_pBackground = new KMahjonggBackground();

    //~ updateWidget(true);

    // Try to get the GameData object from the scene.
    m_pGameData = pGameScene->getGameData();

    // Connect to the scene...
    connect(pGameScene, SIGNAL(itemAdded(GameItem *)), this, SLOT(itemAddedToScene(GameItem *)));
    connect(pGameScene, SIGNAL(itemsAddedFromBoardLayout()), this, SLOT(itemsAddedToScene()));
}

GameWidget::~GameWidget()
{
    delete m_pBackground;
    delete m_pTiles;
}

void GameWidget::itemsAddedToScene()
{
    QList<QGraphicsItem *> tmpItems = items();

    for (int iI = 0; iI < tmpItems.size(); iI++) {
        GameItem *pGameItem = dynamic_cast<GameItem *>(tmpItems.at(iI));

        int iX = pGameItem->getXPosition() - 1;
        int iY = pGameItem->getYPosition() - 1;

        int iTileWidth = m_pTiles->qWidth();
        int iTileHeight = m_pTiles->qHeight();

        int iTilesWidth = iTileWidth * (m_pGameData->m_width / 2);
        int iTilesHeight = iTileHeight * (m_pGameData->m_height / 2);

        int iXFrame = (width() / 2 - iTilesWidth) / 2;
        int iYFrame = (height() / 2 - iTilesHeight) / 2;

        kDebug() << "iXFrame: " << iXFrame;
        kDebug() << "width: " << iTileWidth;
        kDebug() << "rest: " << ((iTileWidth * 2) * m_pGameData->m_width);

        pGameItem->setPos(iTileWidth / 2 * iX + iXFrame, iTileHeight / 2 * iY + iYFrame);
    }
}

void GameWidget::itemAddedToScene(GameItem *pGameItem)
{
    kDebug() << "Added to scene";

    QPixmap selPix;
    QPixmap unselPix;

    selPix = m_pTiles->selectedTile(SW);
    unselPix = m_pTiles->unselectedTile(SW);

    // Set the background pictures to the item.
    pGameItem->setAngle(SW, &selPix, &unselPix);
}

bool GameWidget::setTilesetPath(QString const &rTilesetPath)
{
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

bool GameWidget::setBackgroundPath(QString const &rBackgroundPath)
{
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

void GameWidget::resizeEvent(QResizeEvent *pEvent)
{
    if (pEvent->spontaneous() || m_pGameData == 0) {
        return;
    }

    resizeTileset(pEvent->size());
    m_pBackground->sizeChanged(m_pGameData->m_width / 2, m_pGameData->m_height / 2);

    setSceneRect(0, 0, width(), height());
}

void GameWidget::resizeTileset(QSize const &rSize)
{
    if (m_pGameData == 0) {
        return;
    }

    QSize newtiles = m_pTiles->preferredTileSize(rSize, m_pGameData->m_width / 2,
        m_pGameData->m_height / 2);

    m_pTiles->reloadTileset(newtiles);

    updateItemImages();
}

void GameWidget::updateItemImages()
{
    kDebug() << "Update";

    QList<QGraphicsItem *> tmpItems = items();

    for (int iI = 0; iI < tmpItems.size(); iI++) {
        GameItem *pGameItem = dynamic_cast<GameItem *>(tmpItems.at(iI));

        QPixmap selPix;
        QPixmap unselPix;

        selPix = m_pTiles->selectedTile(SW);
        unselPix = m_pTiles->unselectedTile(SW);

        // Set the background pictures to the item.
        pGameItem->setAngle(SW, &selPix, &unselPix);
    }

    itemsAddedToScene();
}

void GameWidget::setStatusText(QString const &rText)
{
    emit statusTextChanged(rText, m_lGameNumber);
}

void GameWidget::updateBackground()
{
    QPalette palette;
    palette.setBrush(backgroundRole(), m_pBackground->getBackground());
    setPalette(palette);
    setAutoFillBackground(true);
}

void GameWidget::setGameData(GameData *pGameData)
{
    m_pGameData = pGameData;
}

GameData * GameWidget::getGameData()
{
    return m_pGameData;
}
