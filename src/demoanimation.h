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

#ifndef DEMOANIMATION_H
#define DEMOANIMATION_H

#include "kmtypes.h"

#include <QTimer>


// Forward declarations...
class GameData;

/**
 * A class for a demo animation with the help of selection.
 *
 * @author Christian Krippendorf */
class DemoAnimation : public QTimer
{
    Q_OBJECT

public:
    explicit DemoAnimation(QObject *parent = 0);
    ~DemoAnimation();

    /**
     * Set the animation speed in milliseconds.
     *
     * @param animationSpeed The animation speed in milliseconds. */
    void setAnimationSpeed(int animationSpeed);

    /**
     * Get the animation speed in milliseconds.
     *
     * @return Get the animation speed in milliseconds. */
    int getAnimationSpeed() const;

    /**
     * Override of QTimer.
     *
     * @param gameData The data object to handle with for this animation process. */
    void start(GameData * gameData);

    /**
     * Override of QTimer. */
    void stop();

public slots:

signals:
    /**
     * Emits when the game is over.
     *
     * @param won True if computer won the game, else false. */
    void gameOver(bool won);

    /**
     * Emit to remove the given item. */
    void removeItem(POSITION & stItem);

    /**
     * Emit to set the selected state of the given item.
     *
     * @param stItem The position of the item to change the selected state.
     * @param selected THe item should be selected on true, else deselected. */
    void changeItemSelectedState(POSITION & stItem, bool selected);

private slots:
    void timeoutOccurred();

private:
    int m_step;
    int m_animationSpeed;

    POSITION m_stFirst;
    POSITION m_stSecond;

    GameData * m_gameData;
};

#endif // DEMOANIMATION_H
