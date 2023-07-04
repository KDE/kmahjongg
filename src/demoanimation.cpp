/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

#include "moc_demoanimation.cpp"
