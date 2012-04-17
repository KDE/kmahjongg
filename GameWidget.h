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

#include "KmTypes.h"


class GameScene;
class GameData;
class KMahjonggLayout;
class KMahjonggTileset;
class KMahjonggBackground;

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
    GameWidget(QGraphicsScene *pScene = 0, QWidget *pParent = 0);

    /**
     * Destructor */
    ~GameWidget();

    /**
     * Sets the status text.
     *
     * @param rText The new status text. */
    void setStatusText(QString const &rText);

    /**
     * Updates the whole widget.
     *
     * @param bShowTiles True if the tiles should be displayed, else false. */
    void updateWidget(bool bShowTiles);

    /**
     * Override from QWidget. */
    virtual void resizeEvent(QResizeEvent *pEvent);

public slots:
    /**
     * Sets the tileset path and tries to load it.
     *
     * @param rTilesetPath The path to the tileset.
     * @return True if setting and therfore loading success, else false. */
    bool setTilesetPath(QString const &rTilesetPath);

    /**
     * Sets the background path and tries to load it.
     *
     * @param rBackgroundPath The path to the background.
     * @return True if setting and therfore loading success, else false. */
    bool setBackgroundPath(QString const &rBackgroundPath);

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

    /**
     * Resize the tileset to the given size. */
    void resizeTileset(QSize const &rSize);

    /**
     * Updates the background by creating a new QPalette object. */
    void updateBackground();

    int cheatsUsed;
    long m_lGameNumber;
    bool m_bGamePaused;

    GameData *m_pGameData;

    KMahjonggLayout *m_pBoardLayout;
    KMahjonggTileset *m_pTiles;
    KMahjonggBackground *m_pBackground;

    TileViewAngle m_angle;
};


#endif // GAMEWIDGET_H
