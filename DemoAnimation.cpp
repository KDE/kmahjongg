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

#include "DemoAnimation.h"
#include "GameData.h"
#include "kmahjongg_debug.h"

#include <QList>


DemoAnimation::DemoAnimation(QObject * pParent)
    : QTimer(pParent),
    m_iStep(0),
    m_iAnimationSpeed(0),
    m_pGameData(nullptr)
{
    connect(this, &DemoAnimation::timeout, this, &DemoAnimation::timeoutOccurred);
}

DemoAnimation::~DemoAnimation()
{
}

void DemoAnimation::setAnimationSpeed(int iAnimationSpeed)
{
    m_iAnimationSpeed = iAnimationSpeed;
}

int DemoAnimation::getAnimationSpeed() const
{
    return m_iAnimationSpeed;
}

void DemoAnimation::start(GameData * pGameData)
{
    m_pGameData = pGameData;

    QTimer::start(m_iAnimationSpeed);
}

void DemoAnimation::stop()
{
    QTimer::stop();

    m_iStep = 0;
}

void DemoAnimation::timeoutOccurred()
{
    switch (m_iStep++ % 5) {
    case 0:
        // Test if we got a game data object.
        if (m_pGameData == nullptr) {
            qCDebug(KMAHJONGG_LOG) << "m_pGameData is null";

            stop();
            return;
        }

        if (!m_pGameData->findMove(m_stFirst, m_stSecond)) {
            // First stop the animation.
            stop();

            if (m_pGameData->TileNum == 0) {
                // The computer has won the game.
                emit gameOver(true);
            } else {
                // The computer lost the game.
                // setStatusText(i18n("Your computer has lost the game."));
                emit gameOver(false);

                // while (Game->TileNum < Game->MaxTileNum) {
                //     putTileInBoard(Game->MoveListData(Game->TileNum), false);
                //     Game->TileNum++;
                //     putTileInBoard(Game->MoveListData(Game->TileNum));
                //     Game->TileNum++;
                //     drawTileNumber();
                // }
            }
        }

        break;
    case 1:
    case 3:
        emit changeItemSelectedState(m_stFirst, true);
        emit changeItemSelectedState(m_stSecond, true);

        break;
    case 2:
        emit changeItemSelectedState(m_stFirst, false);
        emit changeItemSelectedState(m_stSecond, false);

        break;
    case 4:
        m_stFirst.f -= TILE_OFFSET;
        emit removeItem(m_stFirst);
        m_stSecond.f -= TILE_OFFSET;
        emit removeItem(m_stSecond);

        break;
    }
}
