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

// own
#include "demoanimation.h"

// KMahjongg
#include "gamedata.h"
#include "kmahjongg_debug.h"

DemoAnimation::DemoAnimation(QObject * parent)
    : QTimer(parent)
    , m_step(0)
    , m_animationSpeed(0)
    , m_gameData(nullptr)
{
    connect(this, &DemoAnimation::timeout, this, &DemoAnimation::timeoutOccurred);
}

DemoAnimation::~DemoAnimation()
{
}

void DemoAnimation::setAnimationSpeed(int animationSpeed)
{
    m_animationSpeed = animationSpeed;
}

int DemoAnimation::getAnimationSpeed() const
{
    return m_animationSpeed;
}

void DemoAnimation::start(GameData * gameData)
{
    m_gameData = gameData;

    QTimer::start(m_animationSpeed);
}

void DemoAnimation::stop()
{
    QTimer::stop();

    m_step = 0;
}

void DemoAnimation::timeoutOccurred()
{
    switch (m_step++ % 5) {
        case 0:
            // Test if we got a game data object.
            if (m_gameData == nullptr) {
                qCDebug(KMAHJONGG_LOG) << "m_pGameData is null";

                stop();
                return;
            }

            if (!m_gameData->findMove(m_stFirst, m_stSecond)) {
                // First stop the animation.
                stop();

                if (m_gameData->m_tileNum == 0) {
                    // The computer has won the game.
                    Q_EMIT gameOver(true);
                } else {
                    // The computer lost the game.
                    // setStatusText(i18n("Your computer has lost the game."));
                    Q_EMIT gameOver(false);
                }
            }

            break;
        case 1:
        case 3:
            Q_EMIT changeItemSelectedState(m_stFirst, true);
            Q_EMIT changeItemSelectedState(m_stSecond, true);

            break;
        case 2:
            Q_EMIT changeItemSelectedState(m_stFirst, false);
            Q_EMIT changeItemSelectedState(m_stSecond, false);

            break;
        case 4:
            m_stFirst.f -= TILE_OFFSET;
            Q_EMIT removeItem(m_stFirst);
            m_stSecond.f -= TILE_OFFSET;
            Q_EMIT removeItem(m_stSecond);

            break;
    }
}
