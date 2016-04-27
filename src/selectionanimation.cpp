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
#include "selectionanimation.h"

// Qt
#include <QList>

// KMahjongg
#include "gameitem.h"

SelectionAnimation::SelectionAnimation(QObject * parent)
    : QTimer(parent)
    , m_animationSpeed(0)
    , m_repetitions(0)
    , m_finishedRepetitions(0)
    , m_itemsSelected(false)
    , m_gameItems(new QList<GameItem *>())
{
    connect(this, &SelectionAnimation::timeout, this, &SelectionAnimation::timeoutOccurred);
}

SelectionAnimation::~SelectionAnimation()
{
    delete m_gameItems;
}

void SelectionAnimation::setRepetitions(int repetitions)
{
    m_repetitions = repetitions;
}

void SelectionAnimation::setAnimationSpeed(int animationSpeed)
{
    m_animationSpeed = animationSpeed;
}

int SelectionAnimation::getAnimationSpeed() const
{
    return m_animationSpeed;
}

int SelectionAnimation::getRepetitions() const
{
    return m_repetitions;
}

void SelectionAnimation::addGameItems(QList<GameItem *> gameItems)
{
    while (gameItems.size() > 0) {
        m_gameItems->append(gameItems.takeFirst());
    }
}

void SelectionAnimation::addGameItem(GameItem * gameItem)
{
    m_gameItems->append(gameItem);
}

QList<GameItem *> SelectionAnimation::getGameItems() const
{
    return *m_gameItems;
}

void SelectionAnimation::start()
{
    QTimer::start(m_animationSpeed);
}

void SelectionAnimation::stop()
{
    m_gameItems->clear();
    m_finishedRepetitions = 8;
    QTimer::stop();
    m_finishedRepetitions = 0;
    setSelectedGameItems(false);
    m_itemsSelected = false;
}

void SelectionAnimation::timeoutOccurred()
{
    if (m_itemsSelected) {
        // Items are selected, so deselect them.
        setSelectedGameItems(false);
        m_itemsSelected = false;
    } else {
        // Any animation repetitions left?
        if (m_repetitions > m_finishedRepetitions) {
            setSelectedGameItems(true);
            m_itemsSelected = true;
            ++m_finishedRepetitions;
            start();
        } else {
            stop();
        }
    }
}

void SelectionAnimation::setSelectedGameItems(bool selected)
{
    for (int i = 0; i < m_gameItems->size(); ++i) {
        m_gameItems->at(i)->setSelected(selected);
    }
}
