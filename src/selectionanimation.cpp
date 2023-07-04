/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

#include "moc_selectionanimation.cpp"
