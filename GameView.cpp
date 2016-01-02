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
#include "SelectionAnimation.h"
#include "MoveListAnimation.h"
#include "DemoAnimation.h"
#include "kmahjongglayout.h"
#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "kmahjongg_debug.h"
#include "prefs.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KRandom>

#include <QMouseEvent>
#include <QResizeEvent>


GameView::GameView(GameScene *pGameScene, GameData *pGameData, QWidget *pParent)
    : QGraphicsView(pGameScene, pParent),
    m_bMatch(false),
    m_bGameGenerated(false),
    m_pGameData(pGameData),
    m_pSelectedItem(nullptr),
    m_pTilesetPath(new QString()),
    m_pBackgroundPath(new QString()),
    m_pHelpAnimation(new SelectionAnimation(this)),
    m_pMoveListAnimation(new MoveListAnimation(this)),
    m_pDemoAnimation(new DemoAnimation(this)),
    m_pTiles(new KMahjonggTileset()),
    m_pBackground(new KMahjonggBackground())
{
    // Some settings to the QGraphicsView.
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QGraphicsView { border-style: none; }");
    setAutoFillBackground(true);

    // Read in some settings.
    m_angle = (TileViewAngle) Prefs::angle();

    // Init HelpAnimation
    m_pHelpAnimation->setAnimationSpeed(ANIMATION_SPEED);
    m_pHelpAnimation->setRepetitions(3);

    // Init DemoAnimation
    m_pDemoAnimation->setAnimationSpeed(ANIMATION_SPEED);

    // Init MoveListAnimation
    m_pMoveListAnimation->setAnimationSpeed(ANIMATION_SPEED);

    m_selectionChangedConnect = connect(scene(), &GameScene::selectionChanged, this, &GameView::selectionChanged);

    connect(m_pDemoAnimation, &DemoAnimation::changeItemSelectedState, this, &GameView::changeItemSelectedState);
    connect(m_pDemoAnimation, &DemoAnimation::removeItem, this, &GameView::removeItem);
    connect(m_pDemoAnimation, &DemoAnimation::gameOver, this, &GameView::demoGameOver);

    connect(m_pMoveListAnimation, &MoveListAnimation::removeItem, this, &GameView::removeItem);
    connect(m_pMoveListAnimation, &MoveListAnimation::addItem, this, &GameView::addItemAndUpdate);
    connect(scene(), &GameScene::clearSelectedTile, this, &GameView::clearSelectedTile);
}

GameView::~GameView()
{
    delete m_pHelpAnimation;
    delete m_pDemoAnimation;
    delete m_pMoveListAnimation;
    delete m_pBackground;
    delete m_pTiles;
}

GameScene * GameView::scene() const
{
    return dynamic_cast<GameScene *>(QGraphicsView::scene());
}

bool GameView::checkUndoAllowed()
{
    return (m_pGameData->allow_undo && !checkDemoAnimationActive() && !checkMoveListAnimationActive());
}

bool GameView::checkRedoAllowed()
{
    return (m_pGameData->allow_redo && !checkDemoAnimationActive() && !checkMoveListAnimationActive());
}

long GameView::getGameNumber() const
{
    return m_lGameNumber;
}

void GameView::setGameNumber(long lGameNumber)
{
    m_lGameNumber = lGameNumber;
    setStatusText(i18n("Ready. Now it is your turn."));
}

bool GameView::undo()
{
    // Clear user selections.
    scene()->clearSelection();

    if (m_pGameData->TileNum < m_pGameData->MaxTileNum) {
        m_pGameData->clearRemovedTilePair(m_pGameData->MoveListData(m_pGameData->TileNum + 1),
            m_pGameData->MoveListData(m_pGameData->TileNum + 2));

        ++m_pGameData->TileNum;
        addItemAndUpdate(m_pGameData->MoveListData(m_pGameData->TileNum));
        ++m_pGameData->TileNum;
        addItemAndUpdate(m_pGameData->MoveListData(m_pGameData->TileNum));

        ++m_pGameData->allow_redo;

        setStatusText(i18n("Undo operation done successfully."));

        return true;
    }

    setStatusText(i18n("What do you want to undo? You have done nothing!"));

    return false;
}

bool GameView::redo()
{
    if (m_pGameData->allow_redo > 0) {
        m_pGameData->setRemovedTilePair(m_pGameData->MoveListData(m_pGameData->TileNum),
            m_pGameData->MoveListData(m_pGameData->TileNum - 1));

        removeItem(m_pGameData->MoveListData(m_pGameData->TileNum));
        removeItem(m_pGameData->MoveListData(m_pGameData->TileNum));

        --m_pGameData->allow_redo;

        return true;
    }
    return false;
}

void GameView::demoGameOver(bool bWon)
{
    if (bWon) {
        startMoveListAnimation();
    } else {
        setStatusText(i18n("Your computer has lost the game."));
        emit demoOrMoveListAnimationOver(true);
    }
}

void GameView::createNewGame(long lGameNumber)
{
    setStatusText(i18n("Calculating new game..."));

    // Check any animations are running and stop them.
    checkHelpAnimationActive(true);
    checkDemoAnimationActive(true);
    checkMoveListAnimationActive(true);

    // Create a random game number, if no one was given.
    if (lGameNumber == -1) {
        m_lGameNumber = KRandom::random();
    } else {
        m_lGameNumber = lGameNumber;
    }

    m_pGameData->allow_undo = 0;
    m_pGameData->allow_redo = 0;
    m_pGameData->random.setSeed(m_lGameNumber);

    // Translate m_pGameData->Map to an array of POSITION data.  We only need to
    // do this once for each new game.
    m_pGameData->generateTilePositions();

    // Now use the tile position data to generate tile dependency data.
    // We only need to do this once for each new game.
    m_pGameData->generatePositionDepends();

    // TODO: This is really bad... the generatedStartPosition2() function should never fail!!!!
    // Now try to position tiles on the board, 64 tries max.
    for (short sNr = 0; sNr < 64; ++sNr) {
        if (m_pGameData->generateStartPosition2()) {

            m_bGameGenerated = true;

            // No cheats are used until now.
            m_usCheatsUsed = 0;
            addItemsFromBoardLayout();
            populateItemNumber();

            setStatusText(i18n("Ready. Now it is your turn."));

            return;
        }
    }

    // Couldn't generate the game.
    m_bGameGenerated = false;

    // Hide all generated tiles.
    foreach (GameItem *item, items()) {
        item->hide();
    }

    setStatusText(i18n("Error generating new game!"));
}

void GameView::selectionChanged()
{
    QList<GameItem *> selectedGameItems = scene()->selectedItems();

    // When no item is selected or help animation is running, there is nothing to do.
    if (selectedGameItems.size() < 1 || checkHelpAnimationActive() || checkDemoAnimationActive()) {
        return;
    }

    // If no item was already selected...
    if (m_pSelectedItem == nullptr) {
        // ...set the selected item.
        m_pSelectedItem = selectedGameItems.at(0);

        // Display the matching ones if wanted.
        if (m_bMatch) {
            helpMatch(m_pSelectedItem);
        }
    } else {
        // The selected item is already there, so this is the second selected item.

        // If the same item was clicked, clear the selection and return.
        if (m_pSelectedItem == selectedGameItems.at(0)) {
            clearSelectedTile();
            return;
        }

        // Get both items and their positions.
        POSITION stFirstPos = m_pSelectedItem->getGridPos();
        POSITION stSecondPos = selectedGameItems.at(0)->getGridPos();

        // Test if the items are the same...
        if (m_pGameData->isMatchingTile(stFirstPos, stSecondPos)) {
            // Update the removed tiles in GameData.
            m_pGameData->setRemovedTilePair(stFirstPos, stSecondPos);

            // One tile pair is removed, so we are not allowed to redo anymore.
            m_pGameData->allow_redo = 0;

            // Remove the items.
            removeItem(stFirstPos);
            removeItem(stSecondPos);

            // Reset the selected item variable.
            m_pSelectedItem = nullptr;

            // Test whether the game is over or not.
            if (m_pGameData->TileNum == 0) {
                emit gameOver(m_pGameData->MaxTileNum, m_usCheatsUsed);
            } else {
                // The game is not over, so test if there are any valid moves.
                validMovesAvailable();
            }
        } else {
            // The second tile keeps selected and becomes the first one.
            m_pSelectedItem = selectedGameItems.at(0);

            // Display the matching ones if wanted.
            if (m_bMatch) {
                helpMatch(m_pSelectedItem);
            }
        }
    }
}

void GameView::removeItem(POSITION & stItemPos)
{
    // Adding the data to the protocoll.
    m_pGameData->setMoveListData(m_pGameData->TileNum, stItemPos);

    // Put an empty item in the data object. (data part)
    m_pGameData->putTile(stItemPos.e, stItemPos.y, stItemPos.x, 0);

    // Remove the item from the scene object. (graphic part)
    scene()->removeItem(stItemPos);

    // Decrement the tilenum variable from GameData.
    m_pGameData->TileNum = m_pGameData->TileNum - 1;

    // If TileNum is % 2 then update the number in the status bar.
    if (!(m_pGameData->TileNum % 2)) {
        // The item numbers changed, so we need to populate the new informations.
        populateItemNumber();
    }
}

void GameView::startDemo()
{
    qCDebug(KMAHJONGG_LOG) << "Starting demo mode";

    // Create a new game with the actual game number.
    createNewGame(m_lGameNumber);

    if (m_bGameGenerated) {
        // Start the demo mode.
        m_pDemoAnimation->start(m_pGameData);

        // Set the status text.
        setStatusText(i18n("Demo mode. Click mousebutton to stop."));
    }
}

void GameView::startMoveListAnimation()
{
    qCDebug(KMAHJONGG_LOG) << "Starting move list animation";

    // Stop any helping animation.
    checkHelpAnimationActive(true);

    // Stop demo animation, if anyone is running.
    checkDemoAnimationActive(true);

    m_pMoveListAnimation->start(m_pGameData);
}

void GameView::clearSelectedTile()
{
    scene()->clearSelection();
    m_pSelectedItem = nullptr;
}

void GameView::changeItemSelectedState(POSITION &stItemPos, bool bSelected)
{
    GameItem *pGameItem = scene()->getItemOnGridPos(stItemPos);

    if (pGameItem != nullptr) {
        pGameItem->setSelected(bSelected);
    }
}

void GameView::helpMove()
{
    POSITION stItem1;
    POSITION stItem2;

    // Stop a running help animation.
    checkHelpAnimationActive(true);

    if (m_pGameData->findMove(stItem1, stItem2)) {
        clearSelectedTile();
        m_pHelpAnimation->addGameItem(scene()->getItemOnGridPos(stItem1));
        m_pHelpAnimation->addGameItem(scene()->getItemOnGridPos(stItem2));

        // Increase the cheat variable.
        ++m_usCheatsUsed;

        m_pHelpAnimation->start();
    }
}

void GameView::helpMatch(GameItem const * const pGameItem)
{
    int iMatchCount = 0;
    POSITION stGameItemPos = pGameItem->getGridPos();

    // Stop a running help animation.
    checkHelpAnimationActive(true);

    // Find matching items...
    if ((iMatchCount = m_pGameData->findAllMatchingTiles(stGameItemPos))) {

        // ...add them to the animation object...
        for (int i = 0; i < iMatchCount; ++i) {
            if (scene()->getItemOnGridPos(m_pGameData->getFromPosTable(i)) != pGameItem) {
                m_pHelpAnimation->addGameItem(scene()->getItemOnGridPos(
                    m_pGameData->getFromPosTable(i)));
            }
        }

        // Increase the cheat variable.
        ++m_usCheatsUsed;

        // ...and start the animation.
        m_pHelpAnimation->start();
    }
}

bool GameView::checkHelpAnimationActive(bool bStop)
{
    bool bActive = m_pHelpAnimation->isActive();

    // If animation is running and it should be closed, do so.
    if (bActive && bStop) {
        m_pHelpAnimation->stop();
    }

    return bActive;
}

bool GameView::checkMoveListAnimationActive(bool bStop)
{
    bool bActive = m_pMoveListAnimation->isActive();

    // If animation is running and it should be closed, do so.
    if (bActive && bStop) {
        m_pMoveListAnimation->stop();
    }

    return bActive;
}

bool GameView::checkDemoAnimationActive(bool bStop)
{
    bool bActive = m_pDemoAnimation->isActive();

    // If animation is running and it should be closed, do so.
    if (bActive && bStop) {
        m_pDemoAnimation->stop();
    }

    return bActive;
}

bool GameView::validMovesAvailable(bool bSilent)
{
    POSITION stItem1;
    POSITION stItem2;

    if (!m_pGameData->findMove(stItem1, stItem2)) {
        if (!bSilent) {
            emit noMovesAvailable();
        }
        return false;
    }
    return true;
}

void GameView::pause(bool isPaused)
{
    if (isPaused) {
        foreach (GameItem *item, items()) {
            item->hide();
        }
    } else {
        foreach (GameItem *item, items()) {
            item->show();
        }
    }
}

bool GameView::gameGenerated()
{
    return m_bGameGenerated;
}

void GameView::shuffle()
{
    if (!gameGenerated()) {
        return;
    }

    if (checkDemoAnimationActive() || checkMoveListAnimationActive()) {
        return;
    }

    // Fix bug 156022 comment 5: Redo after shuffle can cause invalid states.
    m_pGameData->allow_redo = 0;

    m_pGameData->shuffle();

    // Update the item images.
    updateItemsImages(items());

    // Cause of using the shuffle function... increase the cheat used variable.
    m_usCheatsUsed += 15;

    // Populate the new item numbers.
    populateItemNumber();

    // Test if any moves are available
    validMovesAvailable();
}

void GameView::populateItemNumber()
{
    // Update the allow_undo variable, cause the item number changes.
    m_pGameData->allow_undo = (m_pGameData->MaxTileNum != m_pGameData->TileNum);

    emit itemNumberChanged(m_pGameData->MaxTileNum, m_pGameData->TileNum, m_pGameData->moveCount());
}

void GameView::addItemsFromBoardLayout()
{
    // The QGraphicsScene::selectionChanged() signal can be emitted when deleting or removing
    // items, so disconnect from this signal to prevent our selectionChanged() slot being
    // triggered and trying to access those items when we clear the scene.
    // The signal is reconnected at the end of the function.
    disconnect(m_selectionChangedConnect);

    // Remove all existing items.
    scene()->clear();

    // Create the items and add them to the scene.
    for (int iZ = 0; iZ < m_pGameData->m_depth; ++iZ) {
        for (int iY = m_pGameData->m_height - 1; iY >= 0; --iY) {
            for (int iX = m_pGameData->m_width - 1; iX >= 0; --iX) {

                // Skip if no tile should be displayed on this position.
                if (!m_pGameData->tilePresent(iZ, iY, iX)) {
                    continue;
                }

                POSITION stItemPos;
                stItemPos.x = iX;
                stItemPos.y = iY;
                stItemPos.e = iZ;
                stItemPos.f = (m_pGameData->BoardData(iZ, iY, iX) - TILE_OFFSET);

                addItem(stItemPos, false, false, false);
            }
        }
    }

    updateItemsImages(items());
    updateItemsOrder();

    // Reconnect our selectionChanged() slot.
    m_selectionChangedConnect = connect(scene(), &GameScene::selectionChanged, this, &GameView::selectionChanged);
}

void GameView::addItem(GameItem * pGameItem, bool bUpdateImage, bool bUpdateOrder, bool bUpdatePosition)
{
    // Add the item to the scene.
    scene()->addItem(pGameItem);

    // If TileNum is % 2 then update the number in the status bar.
    if (!(m_pGameData->TileNum % 2)) {
        // The item numbers changed, so we need to populate the new information.
        populateItemNumber();
    }

    QList<GameItem *> gameItems;
    gameItems.append(pGameItem);

    if (bUpdateImage) {
        updateItemsImages(gameItems);
    }

    if (bUpdatePosition) {
        // When updating the order... the position will automatically be updated after.
        if (bUpdateOrder) {
            updateItemsOrder();
        } else {
            updateItemsPosition(gameItems);
        }
    }
}

void GameView::addItem(POSITION & stItemPos, bool bUpdateImage, bool bUpdateOrder, bool bUpdatePosition)
{
    GameItem * pGameItem = new GameItem(m_pGameData->HighlightData(stItemPos.e, stItemPos.y, stItemPos.x));
    pGameItem->setGridPos(stItemPos);
    pGameItem->setFlag(QGraphicsItem::ItemIsSelectable);

    m_pGameData->putTile(stItemPos.e, stItemPos.y, stItemPos.x, stItemPos.f + TILE_OFFSET);
    addItem(pGameItem, bUpdateImage, bUpdateOrder, bUpdatePosition);
}

void GameView::addItemAndUpdate(POSITION & stItemPos)
{
    addItem(stItemPos, true, true, true);
}

void GameView::updateItemsPosition(QList<GameItem *> gameItems)
{
    // These factor are needed for the different angles. So we simply can
    // calculate to move the items to the left or right and up or down.
    int iAngleXFactor = (m_angle == NE || m_angle == SE) ? -1 : 1;
    int iAngleYFactor = (m_angle == NW || m_angle == NE) ? -1 : 1;

    // Get half width and height of tile faces: minimum spacing = 1 pixel.
    qreal iTileWidth  = m_pTiles->qWidth() + 0.5;
    qreal iTileHeight = m_pTiles->qHeight() + 0.5;

    // Get half height and width of tile-layout: ((n - 1) faces + full tile)/2.
    qreal iTilesWidth  = iTileWidth * (m_pGameData->m_width - 2) / 2
                             + m_pTiles->width() / 2;
    qreal iTilesHeight = iTileHeight * (m_pGameData->m_height - 2) / 2
                             + m_pTiles->height() / 2;

    // Get the top-left offset required to center the items in the view.
    qreal iXFrame = (width() / 2  - iTilesWidth) / 2;
    qreal iYFrame = (height() / 2 - iTilesHeight) / 2;

    // TODO - The last /2 makes it HALF what it should be, but it gets doubled
    //        somehow before the view is painted. Why? Apparently it is because
    //        the background is painted independently by the VIEW, rather than
    //        being an item in the scene and filling the scene completely. So
    //        the whole scene is just the rectangle that contains the tiles.
    // NOTE - scene()->itemsBoundingRect() returns the correct doubled offset.

    for (int iI = 0; iI < gameItems.size(); ++iI) {
        GameItem *pGameItem = gameItems.at(iI);

        // Get rasterized positions of the item.
        int iX = pGameItem->getGridPosX();
        int iY = pGameItem->getGridPosY();
        int iZ = pGameItem->getGridPosZ();


        // Set the position of the item on the scene.
        pGameItem->setPos(
                iXFrame + iTileWidth * iX / 2
                    + iZ * iAngleXFactor * (m_pTiles->levelOffsetX() / 2),
                iYFrame + iTileHeight * iY / 2
                    + iZ * iAngleYFactor * (m_pTiles->levelOffsetY() / 2));
    }
}

void GameView::updateItemsOrder()
{
    int iZCount = 0;
    int iXStart = 0;
    int iXEnd = 0;
    int iXCounter = 0;
    int iYStart = 0;
    int iYEnd = 0;
    int iYCounter = 0;

    switch (m_angle) {
    case NW:
        iXStart = m_pGameData->m_width - 1;
        iXEnd = -1;
        iXCounter = -1;

        iYStart = 0;
        iYEnd = m_pGameData->m_height;
        iYCounter = 1;
        break;
    case NE:
        iXStart = 0;
        iXEnd = m_pGameData->m_width;
        iXCounter = 1;

        iYStart = 0;
        iYEnd = m_pGameData->m_height;
        iYCounter = 1;
        break;
    case SE:
        iXStart = 0;
        iXEnd = m_pGameData->m_width;
        iXCounter = 1;

        iYStart = m_pGameData->m_height - 1;
        iYEnd = -1;
        iYCounter = -1;
        break;
    case SW:
        iXStart = m_pGameData->m_width - 1;
        iXEnd = -1;
        iXCounter = -1;

        iYStart = m_pGameData->m_height - 1;
        iYEnd = -1;
        iYCounter = -1;
        break;
    }

    GameScene *pGameScene = scene();

    for (int iZ = 0; iZ < m_pGameData->m_depth; ++iZ) {
        for (int iY = iYStart; iY != iYEnd; iY = iY + iYCounter) {
            orderLine(pGameScene->getItemOnGridPos(iXStart, iY, iZ), iXStart, iXEnd, iXCounter, iY, iYCounter, iZ, iZCount);
        }
    }

    updateItemsPosition(items());
}

void GameView::orderLine(GameItem * pStartItem, int iXStart, int iXEnd, int iXCounter, int iY, int iYCounter, int iZ, int &iZCount)
{
    GameScene * pGameScene = scene();
    GameItem * pGameItem = pStartItem;

    for (int i = iXStart; i != iXEnd; i = i + iXCounter) {
        if (pGameItem == nullptr) {
            if ((pGameItem = pGameScene->getItemOnGridPos(i, iY, iZ)) == nullptr) {
                continue;
            }
        }

        pGameItem->setZValue(iZCount);
        ++iZCount;

        pGameItem = pGameScene->getItemOnGridPos(i + 2 * iXCounter, iY - 1 * iYCounter, iZ);
        if (pGameItem != nullptr) {
            orderLine(pGameItem, i + 2 * iXCounter, iXEnd, iXCounter, iY - 1 * iYCounter, iYCounter, iZ, iZCount);
            pGameItem = nullptr;
        }
    }
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

    // Tileset or graphics could not be loaded, try default
    if (m_pTiles->loadDefault()) {
        if (m_pTiles->loadGraphics()) {
            resizeTileset(size());

            *m_pTilesetPath = m_pTiles->path();
        }
    }

    return false;
}

bool GameView::setBackgroundPath(QString const &rBackgroundPath)
{
    qCDebug(KMAHJONGG_LOG) << "Set a new Background: " << rBackgroundPath;

    *m_pBackgroundPath = rBackgroundPath;

    if (m_pBackground->load(rBackgroundPath, width(), height())) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();

            return true;
        }
    }

    qCDebug(KMAHJONGG_LOG) << "Loading the background failed. Try to load the default background.";

    // Try default
    if (m_pBackground->loadDefault()) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();

            *m_pBackgroundPath = m_pBackground->path();
        }
    }

    return false;
}

void GameView::setAngle(TileViewAngle angle)
{
    m_angle = angle;
    updateItemsImages(items());
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

    updateItemsImages(items());
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

    updateItemsImages(items());
    updateItemsOrder();
}

QList<GameItem *> GameView::items() const
{
    QList<QGraphicsItem *> originalList = QGraphicsView::items();
    QList<GameItem *> tmpList;

    for (int i = 0; i < originalList.size(); ++i) {
        tmpList.append(dynamic_cast<GameItem *>(originalList.at(i)));
    }

    return tmpList;
}

void GameView::mousePressEvent(QMouseEvent * pMouseEvent)
{
    // If a move list animation is running start a new game.
    if (checkMoveListAnimationActive(true)) {
        emit demoOrMoveListAnimationOver(false);
        return;
    }

    // No mouse events when the demo mode is active.
    if (checkDemoAnimationActive(true)) {
        emit demoOrMoveListAnimationOver(false);
        return;
    }

    // If any help mode is active, ... stop it.
    checkHelpAnimationActive(true);

    // Then go on with the press event.
    QGraphicsView::mousePressEvent(pMouseEvent);
}

void GameView::resizeEvent(QResizeEvent *pEvent)
{
    if (pEvent->spontaneous() || m_pGameData == 0) {
        return;
    }

    resizeTileset(pEvent->size());

    m_pBackground->sizeChanged(width(), height());
    updateBackground();

    setSceneRect(0, 0, width(), height());
}

void GameView::resizeTileset(const QSize &size)
{
    if (m_pGameData == 0) {
        return;
    }

    QSize newtiles = m_pTiles->preferredTileSize(size, m_pGameData->m_width / 2, m_pGameData->m_height / 2);

    foreach (GameItem *item, items()) {
        item->prepareForGeometryChange();
    }

    m_pTiles->reloadTileset(newtiles);

    updateItemsImages(items());
    updateItemsPosition(items());
}

void GameView::updateItemsImages(QList<GameItem *> gameItems)
{
    for (int i = 0; i < gameItems.size(); ++i) {
        GameItem *pGameItem = gameItems.at(i);

        QPixmap selPix;
        QPixmap unselPix;
        QPixmap facePix;

        USHORT usFaceId = (m_pGameData->BoardData(pGameItem->getGridPosZ(), pGameItem->getGridPosY(), pGameItem->getGridPosX()) - TILE_OFFSET);

        pGameItem->setFaceId(usFaceId);

        facePix = m_pTiles->tileface(usFaceId);
        selPix = m_pTiles->selectedTile(m_angle);
        unselPix = m_pTiles->unselectedTile(m_angle);

        // Set the background pictures to the item.
        int iShadowWidth = selPix.width() - m_pTiles->levelOffsetX() - facePix.width();
        int iShadowHeight = selPix.height() - m_pTiles->levelOffsetY() - facePix.height();

        pGameItem->setAngle(m_angle, &selPix, &unselPix, iShadowWidth, iShadowHeight);
        pGameItem->setFace(&facePix);
    }

    // Repaint the view.
    update();
}

void GameView::setStatusText(QString const &rText)
{
    emit statusTextChanged(rText, m_lGameNumber);
}

void GameView::updateBackground()
{
    // qCDebug(KMAHJONGG_LOG) << "Update the background";
    // TODO - The background should be a scene-item? See updateItemsPosition().

    QBrush brush(m_pBackground->getBackground());
    setBackgroundBrush(brush);
}

void GameView::setGameData(GameData *pGameData)
{
    m_pGameData = pGameData;

    addItemsFromBoardLayout();
    populateItemNumber();
}

GameData * GameView::getGameData() const
{
    return m_pGameData;
}

QString GameView::getTilesetPath() const
{
    return *m_pTilesetPath;
}

QString GameView::getBackgroundPath() const
{
    return *m_pBackgroundPath;
}

void GameView::setMatch(bool bMatch)
{
    m_bMatch = bMatch;
}

bool GameView::getMatch() const
{
    return m_bMatch;
}
