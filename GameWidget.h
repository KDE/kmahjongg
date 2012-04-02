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

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QGraphicsView>


class GameScene;
class GameData;
class KMahjonggLayout;

/**
 * The Mahjongg board where the tiles will be placed.
 *
 * @author Christian Krippendorf */
class GameWidget : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param pParent The parent widget. */
    GameWidget(QWidget *pParent = 0);

    /**
     * Destructor */
    ~GameWidget();

    /**
     * Calculates a new game with the given number.
     *
     * @param iGameNumber The game number to create a new game from. */
    void calculateNewGame(int iGameNumber = -1);

    /**
     * Set the BoardLayout file and therefore load the new one.
     *
     * @param rBoardLayoutFile A reference to the BoardLayout file name.
     * @return True if loading the file success. */
    bool setBoardLayoutFile(QString const &rBoardLayoutFile);

    /**
     * Sets the status text.
     *
     * @param rText The new status text. */
    void setStatusText(QString const &rText);

signals:
    /**
     * Emits when a new game was calculated. */
    void newGameCalculated();

    /**
     * Emits when the status text changed.
     *
     * @param rText The new status text.
     * @param lGameNumber The actual game number. */
    void statusTextChanged(const QString &rText, long lGameNumber);

private:
    /**
     * Reloads the board and therefore create a new GameData object. */
    void loadBoard();

    int cheatsUsed;
    long m_lGameNumber;

    GameScene *m_pGameScene;
    GameData *m_pGameData;
    KMahjonggLayout *m_pBoardLayout;
};


#endif // GAMEWIDGET_H
