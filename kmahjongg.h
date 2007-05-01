/*
    kmahjongg, the classic mahjongg game for KDE project

    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006-2007 Mauricio Piacentini   <mauricio@tabuleiro.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef _KMAHJONGG_H
#define _KMAHJONGG_H

#include <kxmlguiwindow.h>

#include "KmTypes.h"
#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include <kgameclock.h>
#include "BoardLayout.h"
#include "Preview.h"
#include "boardwidget.h"

class QAction;
class KToggleAction;
class QLabel;

/**
   @short  Class Description

   @author Mathias
*/
class KMahjongg : public KXmlGuiWindow
{
    Q_OBJECT

    public:
    /**
     * Constructor @param parent */
        explicit KMahjongg( QWidget* parent = 0 );
    /**
     * Default Destructor */
        ~KMahjongg();

    public slots:
    /**
     * Slot Description @param num */
    void startNewGame( int num = -1 );
    /**
     * Slot Description
     * @param msg
     * @param board
     */
    void showStatusText ( const QString &msg, long board);
    /**
     * Slot Description
     * @param iMaximum
     * @param iCurrent
     * @param iLeft
     */
    void showTileNumber( int iMaximum, int iCurrent, int iLeft );
    /**
     * Slot Description @param bActive */
    void demoModeChanged( bool bActive );
    /**
     * Slot Description
     * @param removed
     * @param cheats
     */
    void gameOver( unsigned short removed, unsigned short cheats);
    /**
     * Load BoardLayout from file @param file*/
	void loadBoardLayout(const QString &file);
    /**
     * Slot Description */
	void setDisplayedWidth();
    /**
     * Slot Description */
	void newGame();
    /**
     * Slot Description */
	void timerReset();

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
  void showMatchingTiles();
  void showHighscores();
  void slotBoardEditor();
/*TODO reimplement with game type and preferences
  void openTheme();
  void saveTheme();*/
  void openLayout();
/*TODO reimplement with game type and preferences
  void openBackground();
  void openTileset();*/

protected:
    /**
     * Method Description */
    void setupKAction();
    /**
     * Method Description */
    void setupStatusBar();

private:
  // number of seconds since the start of the game
  unsigned long gameElapsedTime;
  BoardWidget* bw;

  QLabel *gameNumLabel;
  QLabel *tilesLeftLabel;
  QLabel *statusLabel;
  QLabel *gameTimerLabel;

  KGameClock    *gameTimer;
  Preview      *previewLoad;

  bool         bDemoModeActive;

  KToggleAction *showMatchingTilesAction, *pauseAction, *demoAction;
  QAction *undoAction, *redoAction;

};

#endif

