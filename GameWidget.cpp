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


GameWidget::GameWidget(QWidget *pParent)
    : QGraphicsView(pParent)
{
    m_bGamePaused = false;
    m_lGameNumber = 0;

    m_angle = (TileViewAngle) Prefs::angle();

    // Set the board layout.
    m_pBoardLayout = new KMahjonggLayout();
    setBoardLayoutFile(Prefs::layout());

    // Init the game structure.
    m_pGameData = new GameData(m_pBoardLayout->board());

    // Init the game scene object for the GameItems.
    m_pGameScene = new GameScene();
    setScene(m_pGameScene);

    // Create tiles...
    m_pTiles = new KMahjonggTileset();

    if (!setTilesetFile(Prefs::tileSet())) {
        kDebug() << "An error occurred when loading the tileset" << Prefs::tileSet() << "KMahjongg "
            "will continue with the default tileset.";
    }

    // Load background
    m_pBackground = new KMahjonggBackground();

    if (!setBackgroundFile(Prefs::background())) {
        kDebug() << "An error occurred when loading the background" << Prefs::background() << "KMah"
            "jongg will continue with the default background.";
    }

    updateWidget(true);
}

GameWidget::~GameWidget()
{
    delete m_pGameData;
    delete m_pGameScene;
    delete m_pBackground;
    delete m_pTiles;
}

bool GameWidget::setTilesetFile(QString const &rTilesetFile)
{
    if (m_pTiles->loadTileset(rTilesetFile)) {
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

bool GameWidget::setBackgroundFile(QString const &rBackgroundFile)
{
    kDebug() << "Loading background file: " + rBackgroundFile;

    if (m_pBackground->load(rBackgroundFile, width(), height())) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();

            return true;
        }
    }

    // Try default
    if (m_pBackground->loadDefault()) {
        if (m_pBackground->loadGraphics()) {
        }
    }

    // Update the new background.
    updateBackground();

    return false;
}

void GameWidget::resizeTileset(QSize const &rSize)
{
    QSize newtiles = m_pTiles->preferredTileSize(rSize, m_pGameData->m_width / 2,
        m_pGameData->m_height / 2);

    m_pTiles->reloadTileset(newtiles);
}

bool GameWidget::setBoardLayoutFile(QString const &rBoardLayoutFile)
{
    if (m_pBoardLayout->load(rBoardLayoutFile)) {
        return true;
    } else {
        if (m_pBoardLayout->loadDefault()) {
            return false;
        } else {
            return false;
        }
    }
}

void GameWidget::setStatusText(QString const &rText)
{
    emit statusTextChanged(rText, m_lGameNumber);
}

void GameWidget::loadBoard()
{
    delete m_pGameData;
    m_pGameData = new GameData(m_pBoardLayout->board());
}

void GameWidget::calculateNewGame(int iGameNumber)
{
//    deselectItems();
//    stopAnimations();

    m_pGameData->initialiseRemovedTiles();

    // If random layout is true, we will create a new random layout from the existing layouts.
//    if (Prefs::randomLayout()) {
//        setBoardLayoutFile(getRandomLayoutName());
//    }

    setStatusText(i18n("Calculating new game..."));

    // Create a random game number, if no one was given.
    if (iGameNumber == -1) {
        m_lGameNumber = KRandom::random();
    } else {
        m_lGameNumber = iGameNumber;
    }

    m_pGameData->random.setSeed(m_lGameNumber);

    // Try to load the board.
    loadBoard();
//    setStatusText(i18n("Error converting board information!"));
//    return;

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
            // Make the board visible.
            updateWidget(true);
            setStatusText(i18n("Ready. Now it is your turn."));

            // No cheats are used until now.
            cheatsUsed = 0;

            // Throw a signal, that a new game was calculated.
            emit newGameCalculated();

            return;
        }
    }

    // Hide the board cause something went wrong.
    updateWidget(false);
    setStatusText(i18n("Error generating new game!"));
}

void GameWidget::updateWidget(bool bShowTiles)
{
    kDebug() << "Update widget";

    if (m_bGamePaused) {
        bShowTiles = false;
    }

    if (bShowTiles) {
        updateGameScene();
//        drawTileNumber();
    } else {
        // Delete all items in GameScene.
        QList<QGraphicsItem *> items = m_pGameScene->items();
        while (!items.isEmpty()) {
            QGraphicsItem *item = items.takeFirst();
            m_pGameScene->removeItem(item);
            delete item;
        }

        //Recreate our background
//        QPalette palette;
//        palette.setBrush(backgroundRole(), m_pBackground->getBackground());
//        setPalette(palette);
//        setAutoFillBackground(true);
    }
}

void GameWidget::updateGameScene()
{
    kDebug() << "Update game scene";

    // Delete all items in GameScene.
    QList<QGraphicsItem *> items = m_pGameScene->items();
    while (!items.isEmpty()) {
        QGraphicsItem *item = items.takeFirst();
        m_pGameScene->removeItem(item);
        delete item;
    }

    // Recreate the background
//    QPalette palette;
//    palette.setBrush(backgroundRole(), m_pBackground->getBackground());
//    setPalette(palette);
//    setAutoFillBackground(true);

    // Create the items and add them to the scene.
    for (int iZ = 0; iZ < m_pGameData->m_depth; iZ++) {
        for (int iY = 0; iY < m_pGameData->m_height; iY++) {
            for (int iX = m_pGameData->m_width - 1; iX >= 0; iX--) {

                // Skip if no tile should be displayed on this position.
                if (!m_pGameData->tilePresent(iZ, iY, iX)) {
                    continue;
                }

                bool bSelected = false;

                QPixmap selPix;
                QPixmap unselPix;
                QPixmap facePix;

                selPix = m_pTiles->selectedTile(m_angle);
                unselPix = m_pTiles->unselectedTile(m_angle);
                facePix = m_pTiles->tileface(m_pGameData->BoardData(iZ, iY, iX) - TILE_OFFSET);

                if (m_pGameData->HighlightData(iZ, iY, iX)) {
                    bSelected = true;
                }

                GameItem *item = new GameItem(&unselPix, &selPix, &facePix, m_angle, bSelected);
                m_pGameScene->addItem(item);

//                spriteMap.insert(TileCoord(x, y, z), thissprite);
            }
        }
    }
//    updateSpriteMap();
}

void GameWidget::updateBackground()
{
    QPalette palette;
    palette.setBrush(backgroundRole(), m_pBackground->getBackground());
    setPalette(palette);
    setAutoFillBackground(true);
}
