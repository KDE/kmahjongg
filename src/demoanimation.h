/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef DEMOANIMATION_H
#define DEMOANIMATION_H

// Qt
#include <QTimer>

// KMahjongg
#include "kmtypes.h"

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
    explicit DemoAnimation(QObject * parent = nullptr);
    ~DemoAnimation() override;

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

private Q_SLOTS:
    void timeoutOccurred();

private:
    int m_step;
    int m_animationSpeed;

    POSITION m_stFirst;
    POSITION m_stSecond;

    GameData * m_gameData;
};

#endif // DEMOANIMATION_H
