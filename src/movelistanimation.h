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

// Qt
#include <QTimer>

// KMahjongg
#include "kmtypes.h"

// Forward declarations...
class GameData;

enum class AnimationDirection { Forward,
                                Backward };

/**
 * A class for a demo animation with the help of selection.
 *
 * @author Christian Krippendorf */
class MoveListAnimation : public QTimer
{
    Q_OBJECT

public:
    explicit MoveListAnimation(QObject * parent = nullptr);
    ~MoveListAnimation();

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
    int m_step;
    int m_animationSpeed;

    AnimationDirection m_direction;

    GameData * m_gameData;
};

#endif // MOVELISTANIMATION_H
