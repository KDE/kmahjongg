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

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

#include "KmTypes.h"


class GameScene;
class GameData;
class GameItem;
class KMahjonggLayout;
class KMahjonggTileset;
class KMahjonggBackground;

/**
 * The Mahjongg board where the tiles will be placed.
 *
 * @author Christian Krippendorf */
class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param pParent The parent widget.
     * @param pGameScene The related GameScene object. */
    GameView(GameScene *pGameScene, QWidget *pParent = 0);

    /**
     * Destructor */
    ~GameView();

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

    /**
     * Updates the images of the items. */
    void updateItemsImages();

    /**
     * Updates the order of the items. */
    void updateItemsOrder();

    /**
     * Set and get the GameData object. */
    void setGameData(GameData *pGameData);
    GameData * getGameData();

    /**
     * Set the angle of the view.
     *
     * @param angle The angle of to set up. */
    void setAngle(TileViewAngle angle);

    /**
     * Get the angle of the view.
     *
     * @return The angle of the view. */
    TileViewAngle getAngle() const;

public slots:
    /**
     * Switch the view angle to the next right around. */
    void angleSwitchCCW();

    /**
     * Switch the view angle to the next left around. */
    void angleSwitchCW();

    /**
     * Sets the tileset path and tries to load it.
     *
     * @param rTilesetPath The path to the tileset.
     * @return True if setting and therfore loading success, else false. */
    bool setTilesetPath(QString const &rTilesetPath);
    QString getTilesetPath();

    /**
     * Sets the background path and tries to load it.
     *
     * @param rBackgroundPath The path to the background.
     * @return True if setting and therfore loading success, else false. */
    bool setBackgroundPath(QString const &rBackgroundPath);
    QString getBackgroundPath();

    /**
     * Set the board layout path and tries to load it.
     *
     * @param rBoardLayoutPath THe path to the board layout.
     * @return True if loading success else false. */
    bool setBoardLayoutPath(QString const &rBoardLayoutPath);
    QString getBoardLayoutPath();

    /**
     * Create a new game.
     *
     * @param iGameNumber The game number to create or -1 for a random number. */
    void createNewGame(int iGameNumber = -1);

    /**
     * Items where added to the scene and should now be layouted. */
    void updateItemsPosition();

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

private slots:
    /**
     * Gets called when a pair was selected.
     *
     * @param pFirstSelectedItem The first selected item.
     * @param pSecondSelectedItem The second selected item. */
    void pairSelected(QGraphicsItem * pFirstSelectedItem, QGraphicsItem * pSecondSelectedItem);

private:
    /**
     * Reloads the board and therefore create a new GameData object. */
    void loadBoard();

    /**
     * Resize the tileset to the given size.
     *
     * @param rSize The new size of the tileset. */
    void resizeTileset(QSize const &rSize);

    /**
     * Updates the background by creating a new QPalette object. */
    void updateBackground();

    /**
     * Add all the items from the board layout to the scene object. */
    void addItemsFromBoardLayout();

    /**
     * Loads the board layout from the given path.
     *
     * @return True if loading success, else false. */
    bool loadBoardLayoutFromPath();


    int m_iCheatsUsed;
    long m_lGameNumber;
    bool m_bGamePaused;

    GameData *m_pGameData;

    QString *m_pBoardLayoutPath;
    QString *m_pTilesetPath;
    QString *m_pBackgroundPath;

    KMahjonggLayout *m_pBoardLayout;
    KMahjonggTileset *m_pTiles;
    KMahjonggBackground *m_pBackground;

    TileViewAngle m_angle;
};


#endif // GAMEVIEW_H
