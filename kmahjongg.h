/* kmahjongg, the classic mahjongg game for KDE project
 *
 * Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
 * Copyright (C) 2006-2007 Mauricio Piacentini   <mauricio@tabuleiro.com>
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

#ifndef KMAHJONGG_H
#define KMAHJONGG_H

#include <kxmlguiwindow.h>

class KToggleAction;
class QLabel;
class KGameClock;
class Editor;
class GameView;
class GameScene;
class GameData;
class KMahjonggLayout;

/**
 * @author Mathias */
class KMahjongg : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit KMahjongg(QWidget *parent = 0);
    ~KMahjongg();

public slots:
    void startNewGame(int num = -1);

    /**
     * Load the settings... */
    void loadSettings();

    void showStatusText(const QString &msg, long board);

    void showItemNumber(int iMaximum, int iCurrent, int iLeft);

    void gameOver(unsigned short removed, unsigned short cheats);

    /**
     * Connected to GameView::demoOrMoveListAnimationOver(bool) signal. */
    void demoOrMoveListAnimationOver(bool bDemoGameLost);

protected:
    void setupKAction();
    void setupStatusBar();
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void showSettings();
    void startNewNumeric();
    void saveGame();
    void loadGame();
    void restartGame();
    void undo();
    void redo();
    void pause();
    void demoMode();
    void displayTime(const QString& timestring);
    void showHighscores();
    void slotBoardEditor();
    void noMovesAvailable();

private:
    enum class GameState { Gameplay, Demo, Paused, Finished };
    GameState m_gameState;

    void updateState(GameState state);
    void updateUndoAndRedoStates();
    void loadLayout();
    void saveSettings();

    bool m_bLastRandomSetting;

    GameView *m_pGameView;
    GameData *m_pGameData;
    GameScene *m_pGameScene;

    KMahjonggLayout *m_pBoardLayout;

    Editor *boardEditor;

    QLabel *gameNumLabel;
    QLabel *tilesLeftLabel;
    QLabel *statusLabel;
    QLabel *gameTimerLabel;

    QAction *undoAction;
    QAction *redoAction;

    KGameClock *gameTimer;

    KToggleAction *pauseAction;
    KToggleAction *demoAction;

    static const QString gameMagic;
    static const int gameDataVersion;
};

#endif
