/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>
    SPDX-FileCopyrightText: 2006-2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMAHJONGG_H
#define KMAHJONGG_H

// KF
#include <KXmlGuiWindow>

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
    explicit KMahjongg(QWidget * parent = nullptr);
    ~KMahjongg() override;

public Q_SLOTS:
    void startNewGame(int num = -1);

    /**
     * Load the settings... */
    void loadSettings();

    void showStatusText(const QString & msg, long board);

    void showItemNumber(int maximum, int current, int left);

    void gameOver(unsigned short numRemoved, unsigned short cheats);

    /**
     * Connected to GameView::demoOrMoveListAnimationOver(bool) signal. */
    void demoOrMoveListAnimationOver(bool demoGameLost);

protected:
    void setupKAction();
    void setupStatusBar();
    void changeEvent(QEvent * event) override;
    void closeEvent(QCloseEvent * event) override;

private Q_SLOTS:
    void showSettings();
    void startNewNumeric();
    void saveGame();
    void loadGame();
    void restartGame();
    void undo();
    void redo();
    void pause();
    void demoMode();
    void displayTime(const QString & timestring);
    void showHighscores();
    void slotBoardEditor();
    void noMovesAvailable();
    void toggleFullscreen(bool fullscreen);

private:
    enum class GameState { Gameplay,
                           Demo,
                           Paused,
                           Finished,
                           Stuck };
    GameState m_gameState;

    void updateState(GameState state);
    void updateUndoAndRedoStates();
    void loadLayout();
    void saveSettings();

    bool m_bLastRandomSetting;

    GameView * m_gameView;
    GameData * m_gameData;
    GameScene * m_gameScene;

    KMahjonggLayout * m_boardLayout;

    Editor * m_boardEditor;

    QLabel * m_gameNumLabel;
    QLabel * m_tilesLeftLabel;
    QLabel * m_statusLabel;
    QLabel * m_gameTimerLabel;

    QAction * m_undoAction;
    QAction * m_redoAction;

    KGameClock * m_gameTimer;

    KToggleAction * m_pauseAction;
    KToggleAction * m_fullscreenAction;
    KToggleAction * m_demoAction;

    static const int gameDataVersion;
};

#endif
