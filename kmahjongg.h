/*

    $Id$

    kmahjongg, the classic mahjongg game for KDE project

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef _KMAHJONGG_H
#define _KMAHJONGG_H

#include <kmainwindow.h>

#include "KmTypes.h"
#include "Tileset.h"
#include "Background.h"
#include "BoardLayout.h"
#include "Preview.h"
#include "HighScore.h"
#include "boardwidget.h"

class GameTimer;
class Editor;

class KToggleAction;
class KDialogBase;
class QLabel;

/**
    ...
    @author Mathias
*/
class KMahjongg : public KMainWindow
{
    Q_OBJECT

    public:
        KMahjongg( QWidget* parent = 0, const char *name = 0);
        ~KMahjongg();

    public slots:
        void startNewGame( int num = -1 );
        void showStatusText ( const QString& , long);
        void showMessage    ( const QString& );
        void showTileNumber( int iMaximum, int iCurrent, int iLeft );
        void demoModeChanged( bool bActive );
        void gameOver( unsigned short removed, unsigned short cheats);
	void loadBoardLayout(const QString&);
	void setDisplayedWidth();
	void newGame(void);
	void timerReset(void);

	void tileSizeChanged(void);


private slots:
  void showSettings();
  void closeSettings();
	
  void startNewNumeric();
  void saveGame(void);
  void loadGame(void);
  void undo();
  void redo();
  void pause();
  void demoMode();
  void showMatchingTiles();
  void showHighscores();
  void slotBoardEditor();
  void openTheme();
  void saveTheme();
  void openLayout();
  void openBackground();
  void openTileset();
  void keyBindings();

protected:
  void setupKAction();
  void setupStatusBar();
  void enableItem(int item, bool state){}

private:
  KDialogBase *options;
 
  // number of seconds since the start of the game
  unsigned long gameElapsedTime;
  BoardWidget* bw;

  QLabel *gameNumLabel;
  QLabel *tilesLeftLabel;
  QLabel *statusLabel;

  GameTimer    *gameTimer;
  HighScore    *theHighScores;
  Preview      *previewLoad;
  Editor*      boardEditor;

  bool         bShowMatchingTiles;
  bool         bDemoModeActive;

  KToggleAction *showMatchingTilesAction, *pauseAction, *demoAction;
  KAction *undoAction, *redoAction;

};

#endif

