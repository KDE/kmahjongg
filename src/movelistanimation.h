/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

public Q_SLOTS:

Q_SIGNALS:
    /**
     * Emit to remove the given item. */
    void removeItem(POSITION & stItem);

    /**
     * Emit to add the given item. */
    void addItem(POSITION & stItem);

private Q_SLOTS:
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
