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

#include "SelectionAnimation.h"
#include "GameItem.h"

#include <QList>


SelectionAnimation::SelectionAnimation(QObject * pParent)
    : QTimer(pParent),
    m_iAnimationSpeed(0),
    m_iRepetitions(0),
    m_iFinishedRepetitions(0),
    m_bItemsSelected(false),
    m_pGameItems(new QList<GameItem *>())
{
    connect(this, &SelectionAnimation::timeout, this, &SelectionAnimation::timeoutOccurred);
}

SelectionAnimation::~SelectionAnimation()
{
    delete m_pGameItems;
}

void SelectionAnimation::setRepetitions(int iRepetitions)
{
    m_iRepetitions = iRepetitions;
}

void SelectionAnimation::setAnimationSpeed(int iAnimationSpeed)
{
    m_iAnimationSpeed = iAnimationSpeed;
}

int SelectionAnimation::getAnimationSpeed() const
{
    return m_iAnimationSpeed;
}

int SelectionAnimation::getRepetitions() const
{
    return m_iRepetitions;
}

void SelectionAnimation::addGameItems(QList<GameItem *> gameItems)
{
    while(gameItems.size() > 0) {
        m_pGameItems->append(gameItems.takeFirst());
    }
}

void SelectionAnimation::addGameItem(GameItem * pGameItem)
{
    m_pGameItems->append(pGameItem);
}

QList<GameItem *> SelectionAnimation::getGameItems() const
{
    return *m_pGameItems;
}

void SelectionAnimation::start()
{
    QTimer::start(m_iAnimationSpeed);
}

void SelectionAnimation::stop()
{
    m_pGameItems->clear();
    m_iFinishedRepetitions = 8;
    QTimer::stop();
    m_iFinishedRepetitions = 0;
    setSelectedGameItems(false);
    m_bItemsSelected = false;
}

void SelectionAnimation::timeoutOccurred()
{
    if (m_bItemsSelected) {
        // Items are selected, so deselect them.
        setSelectedGameItems(false);
        m_bItemsSelected = false;
    } else {
        // Any animation repetitions left?
        if (m_iRepetitions > m_iFinishedRepetitions) {
            setSelectedGameItems(true);
            m_bItemsSelected = true;
            m_iFinishedRepetitions++;
            start();
        } else {
            stop();
        }
    }
}

void SelectionAnimation::setSelectedGameItems(bool bSelected)
{
    for (int i = 0; i < m_pGameItems->size(); i++) {
        m_pGameItems->at(i)->setSelected(bSelected);
    }
}
