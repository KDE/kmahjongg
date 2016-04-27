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

#include "movelistanimation.h"
#include "gamedata.h"

#include <QList>


MoveListAnimation::MoveListAnimation(QObject * parent)
    : QTimer(parent),
    m_step(0),
    m_animationSpeed(0),
    m_gameData(nullptr)
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
        emit removeItem(m_gameData->MoveListData(m_gameData->m_tileNum));
        emit removeItem(m_gameData->MoveListData(m_gameData->m_tileNum));

        if (m_gameData->m_tileNum == 0) {
            m_direction = AnimationDirection::Backward;
        }
    } else {
        ++m_gameData->m_tileNum;
        emit addItem(m_gameData->MoveListData(m_gameData->m_tileNum));
        ++m_gameData->m_tileNum;
        emit addItem(m_gameData->MoveListData(m_gameData->m_tileNum));

        if (m_gameData->m_tileNum == m_gameData->m_maxTileNum) {
            m_direction = AnimationDirection::Forward;
        }
    }
}
