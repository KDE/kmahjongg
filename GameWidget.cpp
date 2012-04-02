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
#include "kmahjongglayout.h"
#include "prefs.h"

#include <KLocale>
#include <KRandom>


GameWidget::GameWidget(QWidget *pParent)
    : QGraphicsView(pParent)
{
    // Set the board layout.
    m_pBoardLayout = new KMahjonggLayout();
    setBoardLayoutFile(Prefs::layout());

    // Init the game structure.
    m_pGameData = new GameData(m_pBoardLayout->board());
}

GameWidget::~GameWidget()
{
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
//            drawBoard(true);
            setStatusText(i18n("Ready. Now it is your turn."));

            // No cheats are used until now.
            cheatsUsed = 0;

            // Throw a signal, that a new game was calculated.
            emit newGameCalculated();

            return;
        }
    }

    // Hide the board cause something went wrong.
//    drawBoard(false);
    setStatusText(i18n("Error generating new game!"));
}
