/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SELECTIONANIMATION_H
#define SELECTIONANIMATION_H

// Qt
#include <QTimer>

// Forward declarations...
template <class T>
class QList;
class GameItem;

/**
 * A class for animating GameItems with the help of selection.
 *
 * @author Christian Krippendorf */
class SelectionAnimation : public QTimer
{
    Q_OBJECT

public:
    explicit SelectionAnimation(QObject * parent = nullptr);
    ~SelectionAnimation() override;

    /**
     * Set the count of repetitions.
     *
     * @param repetitions The number of repetitions. */
    void setRepetitions(int repetitions);

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
     * Get the number of repetitions set.
     *
     * @return The number of repetitions. */
    int getRepetitions() const;

    /**
     * Add the GameItems that should be selected.
     *
     * @param gameItem The game item that should be selected. */
    void addGameItem(GameItem * gameItem);

    /**
     * Get all the game items.
     *
     * @return A list of game items. */
    QList<GameItem *> getGameItems() const;

    /**
     * Override of QTimer. */
    void start();

    /**
     * Override of QTimer. */
    void stop();

public Q_SLOTS:

Q_SIGNALS:

private Q_SLOTS:
    /**
     * The timeout occurred. */
    void timeoutOccurred();

private:
    /**
     * Set the selection state of all game items.
     *
     * @param selected True if all items should be selected and false for deselecting all items. */
    void setSelectedGameItems(bool selected);

    int m_animationSpeed;
    int m_repetitions;
    int m_finishedRepetitions;
    bool m_itemsSelected;

    QList<GameItem *> * m_gameItems;
};

#endif // SELECTIONANIMATION_H
