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

#ifndef MOVELISTANIMATION_H
#define MOVELISTANIMATION_H

#include "KmTypes.h"

#include <QTimer>

// Forward declarations...
class GameData;

enum class AnimationDirection {Forward, Backward};

/**
 * A class for a demo animation with the help of selection.
 *
 * @author Christian Krippendorf */
class MoveListAnimation : public QTimer
{
    Q_OBJECT

public:
    MoveListAnimation(QObject *pParent = 0);
    ~MoveListAnimation();

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
     * Emit to remove the given item. */
    void removeItem(POSITION & stItem);

    /**
     * Emit to add the given item. */
    void addItem(POSITION & stItem);

private slots:
    /**
     * The timeout occurred. */
    void timeoutOccurred();

private:
    int m_iStep;
    int m_iAnimationSpeed;

    AnimationDirection m_direction;

    GameData * m_pGameData;
};

#endif // MOVELISTANIMATION_H
