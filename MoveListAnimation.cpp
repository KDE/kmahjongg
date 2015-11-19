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

#include "MoveListAnimation.h"
#include "GameData.h"

#include <QList>

#include <KDebug>


MoveListAnimation::MoveListAnimation(QObject * pParent)
    : QTimer(pParent),
    m_iStep(0),
    m_iAnimationSpeed(0),
    m_pGameData(NULL)
{
    connect(this, &MoveListAnimation::timeout, this, &MoveListAnimation::timeoutOccurred);
}

MoveListAnimation::~MoveListAnimation()
{
}

void MoveListAnimation::setAnimationSpeed(int iAnimationSpeed)
{
    m_iAnimationSpeed = iAnimationSpeed;
}

int MoveListAnimation::getAnimationSpeed() const
{
    return m_iAnimationSpeed;
}

void MoveListAnimation::start(GameData * pGameData)
{
    m_pGameData = pGameData;

    // Test whether the tileNum is max or 0.
    if (m_pGameData->TileNum == m_pGameData->MaxTileNum) {
        m_direction = Forward;
    } else {
        m_direction = Backward;
    }

    QTimer::start(m_iAnimationSpeed);
}

void MoveListAnimation::stop()
{
    QTimer::stop();

    m_iStep = 0;
}

void MoveListAnimation::timeoutOccurred()
{
    if (m_pGameData == NULL) {
        return;
    }

    if (m_direction == Forward) {
        // Remove items...
        emit removeItem(m_pGameData->MoveListData(m_pGameData->TileNum));
        emit removeItem(m_pGameData->MoveListData(m_pGameData->TileNum));

        if (m_pGameData->TileNum == 0) {
            m_direction = Backward;
        }
    } else {
        m_pGameData->TileNum++;
        emit addItem(m_pGameData->MoveListData(m_pGameData->TileNum));
        m_pGameData->TileNum++;
        emit addItem(m_pGameData->MoveListData(m_pGameData->TileNum));

        if (m_pGameData->TileNum == m_pGameData->MaxTileNum) {
            m_direction = Forward;
        }
    }
}
