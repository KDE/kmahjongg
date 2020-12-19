/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// own
#include "movelistanimation.h"

// KMahjongg
#include "gamedata.h"

MoveListAnimation::MoveListAnimation(QObject * parent)
    : QTimer(parent)
    , m_step(0)
    , m_animationSpeed(0)
    , m_gameData(nullptr)
{
    connect(this, &MoveListAnimation::timeout, this, &MoveListAnimation::timeoutOccurred);
}

MoveListAnimation::~MoveListAnimation()
{
}

void MoveListAnimation::setAnimationSpeed(int animationSpeed)
{
    m_animationSpeed = animationSpeed;
}

int MoveListAnimation::getAnimationSpeed() const
{
    return m_animationSpeed;
}

void MoveListAnimation::start(GameData * gameData)
{
    m_gameData = gameData;

    // Test whether the tileNum is max or 0.
    if (m_gameData->m_tileNum == m_gameData->m_maxTileNum) {
        m_direction = AnimationDirection::Forward;
    } else {
        m_direction = AnimationDirection::Backward;
    }

    QTimer::start(m_animationSpeed);
}

void MoveListAnimation::stop()
{
    QTimer::stop();

    m_step = 0;
}

void MoveListAnimation::timeoutOccurred()
{
    if (m_gameData == nullptr) {
        return;
    }

    if (m_direction == AnimationDirection::Forward) {
        // Remove items...
        Q_EMIT removeItem(m_gameData->MoveListData(m_gameData->m_tileNum));
        Q_EMIT removeItem(m_gameData->MoveListData(m_gameData->m_tileNum));

        if (m_gameData->m_tileNum == 0) {
            m_direction = AnimationDirection::Backward;
        }
    } else {
        ++m_gameData->m_tileNum;
        Q_EMIT addItem(m_gameData->MoveListData(m_gameData->m_tileNum));
        ++m_gameData->m_tileNum;
        Q_EMIT addItem(m_gameData->MoveListData(m_gameData->m_tileNum));

        if (m_gameData->m_tileNum == m_gameData->m_maxTileNum) {
            m_direction = AnimationDirection::Forward;
        }
    }
}
