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

#ifndef SELECTIONANIMATION_H
#define SELECTIONANIMATION_H

#include <QTimer>


// Forward declarations...
template <class T> class QList;
class GameItem;

/**
 * A class for animating GameItems with the help of selection.
 *
 * @author Christian Krippendorf */
class SelectionAnimation : public QTimer
{
    Q_OBJECT

public:
    /**
     * Constructor */
    SelectionAnimation(QObject *pParent = 0);

    /**
     * Destructor */
    ~SelectionAnimation();

    /**
     * Set the count of repetitions.
     *
     * @param iRepetitions The number of repetitions. */
    void setRepetitions(int iRepetitions);

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
     * Get the number of repetitions set.
     *
     * @return The number of repetitions. */
    int getRepetitions() const;

    /**
     * Add the GameItems that should be selected.
     *
     * @param pGameItem The game item that should be selected. */
    void addGameItem(GameItem * pGameItem);

    /**
     * Add the GameItems that should be selected.
     *
     * @param gameItems The game items that should be selected. */
    void addGameItems(QList<GameItem *> gameItems);

    /**
     * Remove all game items for animation. */
    void clearGameItems();

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

public slots:

signals:

private slots:
    /**
     * The timeout occurred. */
    void timeoutOccurred();

private:
    /**
     * Set the selection state of all game items.
     *
     * @param bSelected True if all items should be selected and false for deselecting all items. */
    void setSelectedGameItems(bool bSelected);

    int m_iAnimationSpeed;
    int m_iRepetitions;
    int m_iFinishedRepetitions;
    bool m_bItemsSelected;

    QList<GameItem *> * m_pGameItems;
};


#endif // SELECTIONANIMATION_H

