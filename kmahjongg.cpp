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

#include <qfile.h>
#include <qvalidator.h>

#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <klineeditdlg.h>
#include <kmenubar.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kstdgameaction.h>
#include <kkeydialog.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <qlabel.h>
#include <kstatusbar.h>

#include "kmahjongg.h"
#include "version.h"
#include "settings.h"
#include "kautoconfig.h"
#include "GameTimer.h"
#include "Preview.h"
#include "Editor.h"

static const char *gameMagic = "kmahjongg-game-v1.0";

//----------------------------------------------------------
// Defines
//----------------------------------------------------------
#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2
#define ID_STATUS_GAME       3

int is_paused = 0;

/**
    Constructor.
*/
KMahjongg::KMahjongg( QWidget* parent, const char *name)
    : KMainWindow(parent, name)
{
    boardEditor = 0;

    // init board widget
    bw = new BoardWidget( this );
    setCentralWidget( bw );
 
    previewLoad = new Preview(this);
    
    setupStatusBar();

    setupKAction();
    
    gameTimer = new GameTimer(toolBar());
    toolBar()->insertWidget(ID_GAME_TIMER, gameTimer->width() , gameTimer);
    toolBar()->alignItemRight( ID_GAME_TIMER, true );
    
    setAutoSaveSettings();
    showStatusbar->setChecked(!statusBar()->isHidden());
    showToolbar->setChecked(!toolBar()->isHidden());

    theHighScores = new HighScore(this);


    bDemoModeActive = false;
    bShowMatchingTiles = false;

    connect( bw, SIGNAL( statusTextChanged(const QString&, long) ),
                 SLOT( showStatusText(const QString&, long) ) );

    connect( bw, SIGNAL( message(const QString&) ),
                 SLOT( showMessage(const QString&) ) );

    connect( bw, SIGNAL( tileNumberChanged(int,int,int) ),
                 SLOT( showTileNumber(int,int,int) ) );

    connect( bw, SIGNAL( demoModeChanged(bool) ),
                 SLOT( demoModeChanged(bool) ) );

    connect( bw, SIGNAL( gameOver(unsigned short , unsigned short)), this,
                SLOT( gameOver(unsigned short , unsigned short)));


    connect(bw, SIGNAL(gameCalculated(void)),
      this, SLOT(timerReset(void)));

    // Make connections for the preview load dialog
    connect( previewLoad, SIGNAL( boardRedraw(bool) ),
              bw,   SLOT( drawBoard(bool) ) );

    connect( previewLoad, SIGNAL( layoutChange(void) ),
              this,   SLOT( newGame(void) ) );


    connect( previewLoad, SIGNAL( loadBackground(const QString&, bool) ),
              bw,   SLOT(loadBackground(const QString&, bool) ) );

    connect( previewLoad, SIGNAL( loadTileset(const QString &) ),
               bw,  SLOT(loadTileset(const QString&) ) );
    connect( previewLoad, SIGNAL( loadBoard(const QString&) ),
                 SLOT(loadBoardLayout(const QString&) ) );

    startNewGame(  );

}

// ---------------------------------------------------------
KMahjongg::~KMahjongg()
{
    delete previewLoad;
    delete theHighScores;
    delete bw;
}

// ---------------------------------------------------------
void KMahjongg::setupKAction()
{
    // game
    KStdGameAction::gameNew(this, SLOT(newGame()), actionCollection());
    KStdGameAction::load(this, SLOT(loadGame()), actionCollection());
    KStdGameAction::save(this, SLOT(saveGame()), actionCollection());
    KStdGameAction::quit(this, SLOT(close()), actionCollection());
    (void)new KAction(i18n("New Numbered Game..."), "newnum", 0, this, SLOT(startNewNumeric()), actionCollection(), "game_new_numeric");
    (void)new KAction(i18n("Open Th&eme..."), 0, this, SLOT(openTheme()), actionCollection(), "game_open_theme");
    (void)new KAction(i18n("Open &Tileset..."), 0, this, SLOT(openTileset()), actionCollection(), "game_open_tileset");
    (void)new KAction(i18n("Open &Background..."), 0, this, SLOT(openBackground()), actionCollection(), "game_open_background");
    (void)new KAction(i18n("Open La&yout..."), 0, this, SLOT(openLayout()), actionCollection(), "game_open_layout");
    (void)new KAction(i18n("Sa&ve Theme..."), 0, this, SLOT(saveTheme()), actionCollection(), "game_save_theme");
    // originally "file" ends here
    KStdGameAction::hint(bw, SLOT(helpMove()), actionCollection());
    (void)new KAction(i18n("Shu&ffle"), "reload", 0, bw, SLOT(shuffle()), actionCollection(), "move_shuffle");
    demoAction = KStdGameAction::demo(this, SLOT(demoMode()), actionCollection());
    showMatchingTilesAction = new KToggleAction(i18n("Show &Matching Tiles"), 0, this, SLOT(showMatchingTiles()), actionCollection(), "options_show_matching_tiles");
    KStdGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    pauseAction = KStdGameAction::pause(this, SLOT(pause()), actionCollection());

    // TODO: store the background ; open on startup
    // TODO: same about layout
    // TODO: same about theme

    // move
    undoAction = KStdGameAction::undo(this, SLOT(undo()), actionCollection());
    redoAction = KStdGameAction::redo(this, SLOT(redo()), actionCollection());

    // edit
    (void)new KAction(i18n("&Board Editor..."), 0, this, SLOT(slotBoardEditor()), actionCollection(), "edit_board_editor");

    // settings
    showToolbar = KStdAction::showToolbar(this, SLOT(toggleToolbar()),
	actionCollection());
    showStatusbar = KStdAction::showStatusbar(this, SLOT(toggleStatusbar()),
	actionCollection());
    KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
    KStdAction::keyBindings(this, SLOT(keyBindings()), actionCollection());

    createGUI();
}

// ---------------------------------------------------------
void KMahjongg::setupStatusBar()
{
    // The following isn't possible with the new KStatusBar anymore.
    // The correct fix is probably to reverse the order of adding the
    // widgets. :/
    // Just commenting it out for now (order is not as important
    // as compilation), in case someone comes up with a better fix.
    // pStatusBar->setInsertOrder( KStatusBar::RightToLeft );

    tilesLeftLabel= new QLabel("Removed: 0000/0000", statusBar());
    tilesLeftLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(tilesLeftLabel, tilesLeftLabel->sizeHint().width(), ID_STATUS_GAME);


    gameNumLabel = new QLabel("Game: 000000000000000000000", statusBar());
    gameNumLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(gameNumLabel, gameNumLabel->sizeHint().width(), ID_STATUS_TILENUMBER);


    statusLabel= new QLabel("Kmahjongg", statusBar());
    statusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(statusLabel, statusLabel->sizeHint().width(), ID_STATUS_MESSAGE);

  //  pStatusBar->setAlignment( ID_STATUS_TILENUMBER, AlignCenter );
}

void KMahjongg::setDisplayedWidth()
{
  bw->setDisplayedWidth();
  setFixedSize( bw->size() +
    QSize( 2, (!statusBar()->isHidden() ? statusBar()->height() : 0)
         + 2 + menuBar()->height() ) );
  toolBar()->setFixedWidth(bw->width());
  toolBar()->alignItemRight( ID_GAME_TIMER, true );
  bw->drawBoard();
}


// ---------------------------------------------------------
void KMahjongg::startNewNumeric()
{
        QIntValidator v( 0, 268435457, this );
        bool ok;
        QString s = KLineEditDlg::getText(i18n("New Game"),i18n("Enter game number:"),QString::null,&ok,this,&v);
	if (ok) {
		startNewGame( (int)KGlobal::locale()->readNumber(s, &ok) );
	}
}

void KMahjongg::undo()
{
    bw->Game.allow_redo += bw->undoMove();
    demoModeChanged(false);
}

void KMahjongg::redo()
{
    if (bw->Game.allow_redo >0) {
            bw->Game.allow_redo--;
            bw->redoMove();
            demoModeChanged(false);
    }
}

void  KMahjongg::showSettings(){
  options = new KDialogBase (this, "Settings", false, i18n("Settings"), KDialogBase::Default | KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel);
  KAutoConfig *kautoconfig = new KAutoConfig(options, "KAutoConfig");

  connect(options, SIGNAL(okClicked()), kautoconfig, SLOT(saveSettings()));
  connect(options, SIGNAL(okClicked()), this, SLOT(closeSettings()));
  connect(options, SIGNAL(applyClicked()), kautoconfig, SLOT(saveSettings()));
  connect(options, SIGNAL(defaultClicked()), kautoconfig, SLOT(resetSettings()));

  Settings *settings = new Settings(options, "Settings");
  options->setMainWidget(settings);
  kautoconfig->addWidget(settings, "General");

  kautoconfig->retrieveSettings();
  options->show();

  connect(kautoconfig, SIGNAL(settingsChanged()), bw, SLOT(loadSettings()));
  connect(kautoconfig, SIGNAL(settingsChanged()), SLOT(setDisplayedWidth()));
}

void  KMahjongg::closeSettings(){
  options->close(true);
}

void KMahjongg::toggleToolbar(){
 if (toolBar()->isHidden())
   toolBar()->show();
 else
   toolBar()->hide();
}

void KMahjongg::toggleStatusbar(){
  if (statusBar()->isHidden())
    statusBar()->show();
  else 
    statusBar()->hide();
  setDisplayedWidth();
}

void KMahjongg::keyBindings()
{
    KKeyDialog::configure(actionCollection(), this);
}

void KMahjongg::demoMode()
{
    if( bDemoModeActive ) {
            bw->stopDemoMode();
    } else {
        // we assume demo mode removes tiles so we can
        // disbale redo here.
        bw->Game.allow_redo=false;
        bw->startDemoMode();
    }

}

void KMahjongg::pause()
{
    is_paused = !is_paused;
    demoModeChanged(false);
    gameTimer->pause();
    bw->pause();
}

void KMahjongg::showMatchingTiles()
{
    bShowMatchingTiles = ! bShowMatchingTiles;
    bw->setShowMatch( bShowMatchingTiles );
    showMatchingTilesAction->setChecked(bShowMatchingTiles);
}

void KMahjongg::showHighscores()
{
    theHighScores->exec(bw->getLayoutName());
}

void KMahjongg::openTheme()
{
    previewLoad->initialise(Preview::theme, EXT_THEME);
    previewLoad->exec();
}

void KMahjongg::saveTheme()
{
    previewLoad->initialise(Preview::theme, EXT_THEME);
    previewLoad->saveTheme();
}

void KMahjongg::openLayout()
{
    previewLoad->initialise(Preview::board, EXT_LAYOUT);
    previewLoad->exec();
}

void KMahjongg::openBackground()
{
    previewLoad->initialise(Preview::background, EXT_BACKGROUND);
    previewLoad->exec();
}

void KMahjongg::openTileset()
{
    previewLoad->initialise(Preview::tileset, EXT_TILESET);
    previewLoad->exec();
}

void KMahjongg::slotBoardEditor()
{
    if (!boardEditor)
            boardEditor = new Editor(this);
    boardEditor->exec();
}

//----------------------------------------------------------
// signalled from the prieview dialog to generate a new game
// we don't make startNewGame a slot because it has a default
// param.

void KMahjongg::newGame(void)
{
    startNewGame();
}



// ---------------------------------------------------------
void KMahjongg::startNewGame( int item )
{
    if( ! bDemoModeActive ) {
        bw->calculateNewGame(item);

	// initialise button states
	bw->Game.allow_redo = bw->Game.allow_undo = 0;

	timerReset();

    	// update the initial enabled/disabled state for
    	// the menu and the tool bar.
    	demoModeChanged(false);
    }
}

// ---------------------------------------------------------
void KMahjongg::timerReset(void) {

	// initialise the scoring system
	gameElapsedTime = 0;

	// start the game timer
	gameTimer->start();

}


// ---------------------------------------------------------

void KMahjongg::gameOver(
	unsigned short numRemoved,
	unsigned short cheats)
{
	int time;
	int score;

	gameTimer->pause();
	KMessageBox::information(this, i18n("Game over: You have won!"));
	bw->animateMoveList();
	int elapsed = gameTimer->toInt();

	time = score = 0;

	// get the time in milli secs
	// subtract from 20 minutes to get bonus. if longer than 20 then ignore
	time = (60*20) - gameTimer->toInt();
	if (time <0)
		time =0;
	// conv back to  secs (max bonus = 60*20 = 1200

	// points per removed tile bonus (for deragon max = 144*10 = 1440
	score += (numRemoved * 20);
	// time bonus one point per second under one hour
	score += time;
	// points per cheat penalty (max penalty = 1440 for dragon)
	score -= (cheats *20);
	if (score < 0)
		score = 0;

	theHighScores->checkHighScore(score, elapsed, bw->getGameNum(), bw->getBoardName());

	timerReset();
}

// ---------------------------------------------------------
void KMahjongg::showStatusText( const QString &msg, long board )
{
    statusLabel->setText(msg);
    QString str = i18n("Game number: %1").arg(board);
    gameNumLabel->setText(str);

}

// ---------------------------------------------------------
void KMahjongg::showMessage( const QString &msg )
{
    KMessageBox::information( this, msg );
}

// ---------------------------------------------------------
void KMahjongg::showTileNumber( int iMaximum, int iCurrent, int iLeft )
{
    // Hmm... seems iCurrent is the number of remaining tiles, not removed ...
    //QString szBuffer = i18n("Removed: %1/%2").arg(iCurrent).arg(iMaximum);
    QString szBuffer = i18n("Removed: %1/%2  Pairs left: %3").arg(iMaximum-iCurrent).arg(iMaximum).arg(iLeft);
    tilesLeftLabel->setText(szBuffer);

    // Update here since undo allow is effected by demo mode
    // removal. However we only change the enabled state of the
    // items when not in demo mode
    bw->Game.allow_undo = iMaximum != iCurrent;

    // update undo menu item, if demomode is inactive
    if( ! bDemoModeActive && !is_paused)
    {

//        pMenuBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
//        toolBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
        undoAction->setEnabled(bw->Game.allow_undo);
    }
}


// ---------------------------------------------------------
void KMahjongg::demoModeChanged( bool bActive)
{
    bDemoModeActive = bActive;

    pauseAction->setChecked(is_paused);
    demoAction->setChecked(bActive || is_paused);

    if (is_paused)
        stateChanged("paused");
    else if (bActive)
        stateChanged("active");
    else {
        stateChanged("inactive");
        undoAction->setEnabled(bw->Game.allow_undo);
	    redoAction->setEnabled(bw->Game.allow_redo);
    }
}

void KMahjongg::loadBoardLayout(const QString &file) {
	bw->loadBoardLayout(file);
}

void KMahjongg::tileSizeChanged(void) {
	bw->tileSizeChanged();
	setDisplayedWidth();
}


void KMahjongg::loadGame(void) {
	GAMEDATA in;
	char buffer[1024];
    QString fname;

    // Get the name of the file to load
    KURL url = KFileDialog::getOpenURL( NULL, "*.kmgame", this, i18n("Load Game" ) );

    if ( url.isEmpty() )
	return;

    KIO::NetAccess::download( url, fname );

    // open the file for reading
    FILE *outFile = fopen( QFile::encodeName(fname), "r");
    if (outFile == NULL) {
	KMessageBox::sorry(this,
		i18n("Could not read from file. Aborting."));
	return;
    }

    // verify the magic
    fscanf(outFile, "%1023s\n", buffer);
    if (buffer != gameMagic) {
	KMessageBox::sorry(this,
		i18n("File format not recognized."));
	fclose(outFile);
	return;
    }

    //ed the elapsed time
    fscanf(outFile, "%1023s\n", buffer);
    gameTimer->fromString(buffer);

    // suck out all the game data
    fread(&in, sizeof(GAMEDATA), 1, outFile);
    memcpy(&bw->Game, &in, sizeof(GAMEDATA));

    // close the file before exit
    fclose(outFile);

    KIO::NetAccess::removeTempFile( fname );

    // refresh the board
    bw->drawBoard();
}

void KMahjongg::saveGame(void) {

    // Get the name of the file to save
    KURL url = KFileDialog::getSaveURL( NULL, "*.kmgame", this, i18n("Save Game" ) );

    if ( url.isEmpty() )
	return;

   if( !url.isLocalFile() )
   {
      KMessageBox::sorry( this, i18n( "Only saving to local files currently supported." ) );
      return;
   }

    FILE *outFile = fopen( QFile::encodeName(url.path()), "w");
    if (outFile == NULL) {
	KMessageBox::sorry(this,
		i18n("Could not write to file. Aborting."));
	return;
    }

    // stick in the magic id string
    fprintf(outFile, "%s\n", gameMagic);

    // Now stick in the elapsed time for the game
    fprintf(outFile, "%s\n", gameTimer->toString().utf8().data());


    // chuck in all the game data
    fwrite(&bw->Game, sizeof(GAMEDATA), 1, outFile);

    // close the file before exit
    fclose(outFile);
}


#include "kmahjongg.moc"
