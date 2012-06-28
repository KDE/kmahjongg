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
#include "GameItem.h"

#include <QList>


DemoAnimation::DemoAnimation(QObject * pParent)
    : QTimer(pParent),
    m_iAnimationSpeed(0),
    m_iRepetitions(0),
    m_iFinishedRepetitions(0)
{
    connect(this, SIGNAL(timeout()), this, SLOT(timeoutOccurred()));
}

DemoAnimation::~DemoAnimation()
{
}

void DemoAnimation::setRepetitions(int iRepetitions)
{
    m_iRepetitions = iRepetitions;
}

void DemoAnimation::setAnimationSpeed(int iAnimationSpeed)
{
    m_iAnimationSpeed = iAnimationSpeed;
}

int DemoAnimation::getAnimationSpeed() const
{
    return m_iAnimationSpeed;
}

int DemoAnimation::getRepetitions() const
{
    return m_iRepetitions;
}

void DemoAnimation::start()
{
    QTimer::start(m_iAnimationSpeed);
}

void DemoAnimation::stop()
{
    QTimer::stop();
}

void DemoAnimation::timeoutOccurred()
{
}
