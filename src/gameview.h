/*
    SPDX-FileCopyrightText: 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

// Qt
#include <QGraphicsView>

// KMahjongg
#include "kmtypes.h"

constexpr int ANIMATION_SPEED = 200;

// Forward declaration...
class GameScene;
class GameData;
class GameItem;
class GameBackground;
class GameRemovedTiles;
class SelectionAnimation;
class MoveListAnimation;
class DemoAnimation;
class KMahjonggTileset;
class KMahjonggBackground;
class QMouseEvent;

/**
 * The Mahjongg board where the tiles (GameItems) will be painted.
 *
 * @author Christian Krippendorf */
class GameView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param gameScene The related GameScene object.
     * @param gameData The related GameData object.
     * @param parent The parent widget.
     */
    GameView(GameScene * gameScene, GameData * gameData, QWidget * parent = nullptr);
    ~GameView() override;

    /**
     * Items where added to the scene and should now be layouted.
     *
     * @param gameItems The items of which the positions should be updated. */
    void updateItemsPosition(const QList<GameItem *> &gameItems);

    /**
     * Overloaded function of scene game item positioning.
     */
    void updateItemsPosition();

    /**
     * Updates the whole widget.
     *
     * @param showTiles True if the tiles should be displayed, else false. */
    void updateWidget(bool showTiles);

    /**
     * Override from QGraphcisView. */
    virtual QList<GameItem *> getGameItems() const;

    /**
     * Set whether removed tiles should be shown.
     * @param show True if removed tiles should be shown.
     */
    void showRemovedTiles(bool show);

    /**
     * Override from QGraphicsView. */
    GameScene * scene() const;

    /**
     * Set the GameData object.
     *
     * @param gameData The game data object. */
    void setGameData(GameData * gameData);

    /**
     * Get the GameData object that is actually set.
     *
     * @return The actual GameData object. */
    GameData * getGameData() const;

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

    /**
     * Test for active help animation and maybe close.
     *
     * @param stop Stop the help animation if running.
     * @return Return true if the help animation was running else false. */
    bool checkHelpAnimationActive(bool stop = false);

    /**
     * Test for active demo animation and maybe close.
     *
     * @param stop Stop the demo animation if running.
     * @return Return true if the demo animation was running else false. */
    bool checkDemoAnimationActive(bool stop = false);

    /**
     * Test for active move list animation and maybe close.
     *
     * @param stop Stop the move list animation if running.
     * @return Return true if the move list animation was running else false. */
    bool checkMoveListAnimationActive(bool stop = false);

    /**
     * Set the match variable. If set to true, the matching items to the selected will be animated.
     *
     * @param match The match value to set up. */
    void setMatch(bool match);

    /**
     * Get the match value.
     *
     * @return True when matching items of the selected one will be displayed, else false. */
    bool getMatch() const;

    /**
     * Gets the tilesetpath that is actually set.
     *
     * @return The tilesetpath as a string. */
    QString getTilesetPath() const;

    /**
     * Gets the background path that is actually set.
     *
     * @return The background path as a string. */
    QString getBackgroundPath() const;

    /**
     * Sets the tileset path and tries to load it.
     *
     * @param tilesetPath The path to the tileset.
     * @return True if setting and therefore loading success, else false. */
    bool setTilesetPath(QString const & tilesetPath);

    /**
     * Sets the background path and tries to load it.
     *
     * @param backgroundPath The path to the background.
     * @return True if setting and therefore loading success, else false. */
    bool setBackgroundPath(QString const & backgroundPath);

    /**
     * Undo the last move.
     *
     * @return True if successful, else false. */
    bool undo();

    /**
     * Redo the last undo.
     *
     * @return True if successful, else false. */
    bool redo();

    /**
     * Test if undo is allowed.
     *
     * @return True if allowed, else false. */
    bool checkUndoAllowed();

    /**
     * Test if redo is allowed.
     *
     * @return True if allowed, else false. */
    bool checkRedoAllowed();

    /**
     * Get the game number.
     *
     * @return The game number or -1 if no game number set. */
    long getGameNumber() const;

    /**
     * Set the game number.
     *
     * @param gameNumber Game number. */
    void setGameNumber(long gameNumber);

    /**
     * Search for a valid move silently or with an information text.
     *
     * @param silent False if a message should appears when no legal moves exist, else true.
     *        Default ist false!
     * @return True if a legal move exist, else false. */
    bool validMovesAvailable(bool silent = false);

    /**
     * Hide/show tiles when game is paused/unpaused. */
    void pause(bool isPaused);

    /**
     * Get whether a game was generated.
     *
     * @return True if game was generated, or false. */
    bool gameGenerated();

public Q_SLOTS:
    /**
     * Add a new item with the given position and update images, position and order. */
    void addItemAndUpdate(POSITION & stItemPos);

    /**
     * Remove the given item.
     *
     * @param stItemPos The item position. */
    void removeItem(POSITION & stItemPos);

    /**
     * Starts the demo animation. */
    void startDemo();

    /**
     * Switch the view angle to the next right around. */
    void angleSwitchCCW();

    /**
     * Switch the view angle to the next left around. */
    void angleSwitchCW();

    /**
     * Create a new game.
     *
     * @param gameNumber The game number to create or -1 for a random number. */
    void createNewGame(long gameNumber = -1);

    /**
     * Shuffle the position of items. */
    void shuffle();

    /**
     * Give a hint for a valid move. */
    void helpMove();

    /**
     * Give a hint to the matching tiles.
     *
     * @param gameItem The item we search matching tiles for. */
    void helpMatch(const GameItem * const gameItem);

    /**
     * Start the move list animation. */
    void startMoveListAnimation();

    /**
     * Clear the selection. */
    void clearSelectedTile();

protected:
    /**
     * Override from QGraphicsView. */
    void resizeEvent(QResizeEvent * event) override;

    /**
     * Override from QGraphicsView. */
    void mousePressEvent(QMouseEvent * mouseEvent) override;

Q_SIGNALS:
    /**
     * Emits when a new game was calculated. */
    void newGameCalculated();

    /**
     * Emits when the status text changed.
     *
     * @param text The new status text.
     * @param gameNumber The actual game number. */
    void statusTextChanged(const QString & text, long gameNumber);

    /**
     * Emits when the number of the items changed or could change.
     *
     * @param maxItemNum The max tile number.
     * @param itemNum The item number that are still there.
     * @param moveCount Number of moves. */
    void itemNumberChanged(int maxItemNum, int itemNum, int moveCount);

    /**
     * Emits when the game is over.
     *
     * @param removedItems The number of the removed items.
     * @param cheatsUsed The number of the cheats that are used. */
    void gameOver(unsigned short removedItems, unsigned short cheatsUsed);

    /**
     * Emits when demo is played out and lost or stopped by a mouse click, or
     * the MoveListAnimation is stopped by a mouse click.
     *
     * @param demoGameLost True if demo game is played out and lost. */
    void demoOrMoveListAnimationOver(bool demoGameLost);

    /**
     * Emits when no more moves are available. */
    void noMovesAvailable();

private Q_SLOTS:
    /**
     * When the right mouse is clicked, the given gameItem will be handled. This slot is connected to the game scene.
     *
     * @param gameItem the game item object. Don't delete the object. gameItem can be nullptr if no game item has been
     * clicked. */
    void rightMouseAction(GameItem * gameItem);

    /**
     * Add a new item with the given position.
     *
     * @param stItemPos The position for the new item.
     * @param updateImage True for updating the images else false.
     * @param updateOrder True for updating the order else false.
     * @param updatePosition True for updating the position else false. */
    void addItem(POSITION & stItemPos, bool updateImage = false, bool updateOrder = false,
                 bool updatePosition = false);

    /**
     * Add a new item.
     *
     * @param gameItem The new game item object.
     * @param updateImage True for updating the images else false.
     * @param updateOrder True for updating the order else false.
     * @param updatePosition True for updating the position else false. */
    void addItem(GameItem * gameItem, bool updateImage = false, bool updateOrder = false,
                 bool updatePosition = false);

    /**
     * When the game is over by the demo mode.
     *
     * @param won True if the computer won in demo mode, else false. */
    void demoGameOver(bool won);

    /**
     * Change the selected state of the given item.
     *
     * @param stItemPos The position of the item.
     * @param selected The selection state to set. */
    void changeItemSelectedState(POSITION & stItemPos, bool selected);

    /**
     * Gets called when a pair was selected. */
    void selectionChanged();

private:
    /**
     * Updates the images of the items.
     *
     * @param gameItem The items of which the images should be updated. */
    void updateItemsImages(const QList<GameItem *> &gameItems);

    /**
     * Updates the order of the items. */
    void updateItemsOrder();

    /**
     * Populates the number of the items, by emit a signal: itemNumberChanged(...). */
    void populateItemNumber();

    /**
     * Sets the status text.
     *
     * @param text The new status text. */
    void setStatusText(const QString & text);

    /**
     * Resize the tileset to the given size.
     *
     * @param size The new size of the tileset. */
    void resizeTileset(const QSize & size);

    /**
     * Updates the background by creating a new QPalette object. */
    void updateBackground();

    /**
     * Add all the items from the board layout to the scene object. */
    void addItemsFromBoardLayout();

    /**
     * Order the line starting by the item.
     *
     * @param startItem The item where the line starts.
     * @param xStart The x position of the item.
     * @param y The y position of the item.
     * @param z The z position of the item.
     * @param zCount The z count variable for the order. */
    void orderLine(GameItem * startItem, int xStart, int xEnd, int xCounter, int y,
                   int yCounter, int z, int & zCount);

    unsigned short m_cheatsUsed;
    quint32 m_gameNumber;
    bool m_gamePaused;
    bool m_match;
    bool m_gameGenerated;
    bool m_showRemovedTiles;

    qreal m_remTilesWidthFactor;

    GameData * m_gameData;
    GameItem * m_selectedItem;
    GameBackground * m_gameBackground;
    GameRemovedTiles * m_gameRemovedTiles;

    QString * m_tilesetPath;
    QString * m_backgroundPath;

    SelectionAnimation * m_helpAnimation;
    MoveListAnimation * m_moveListAnimation;
    DemoAnimation * m_demoAnimation;

    KMahjonggTileset * m_tiles;
    KMahjonggBackground * m_background;

    TileViewAngle m_angle;

    // Needed for disconnecting connection
    QMetaObject::Connection m_selectionChangedConnect;
};

#endif // GAMEVIEW_H
