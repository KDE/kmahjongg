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

#include "KmTypes.h"

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
    explicit DemoAnimation(QObject *pParent = 0);
    ~DemoAnimation();

    /**
     * Set the animation speed in milliseconds.
     *
     * @param iAnimationSpeed The animation speed in milliseconds. */
    void setAnimationSpeed(int iAnimationSpeed);

    /**
     * Get the animation speed in milliseconds.
     *
     * @return Get the animation speed in milliseconds. */
    int getAnimationSpeed() const;

    /**
     * Override of QTimer.
     *
     * @param pGameData The data object to handle with for this animation process. */
    void start(GameData * pGameData);

    /**
     * Override of QTimer. */
    void stop();

public slots:

signals:
    /**
     * Emits when the game is over.
     *
     * @param bWon True if computer won the game, else false. */
    void gameOver(bool bWon);

    /**
     * Emit to remove the given item. */
    void removeItem(POSITION & stItem);

    /**
     * Emit to set the selected state of the given item.
     *
     * @param stItem The position of the item to change the selected state.
     * @param bSelected THe item should be selected on true, else deselected. */
    void changeItemSelectedState(POSITION & stItem, bool bSelected);

private slots:
    void timeoutOccurred();

private:
    int m_iStep;
    int m_iAnimationSpeed;

    POSITION m_stFirst;
    POSITION m_stSecond;

    GameData * m_pGameData;
};

#endif // DEMOANIMATION_H
