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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <limits.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kconfigdialog.h>
#include <kinputdialog.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstdgameaction.h>
#include <kstdaction.h>
#include <kicon.h>

#include <QPixmapCache>

#include <kio/netaccess.h>
#include <klocale.h>
#include <ktoggleaction.h>

#include "prefs.h"
#include "kmahjongg.h"
#include "ui_settings.h"
#include "GameTimer.h"
#include "Editor.h"

static const char *gameMagic = "kmahjongg-game-v1.0";

//----------------------------------------------------------
// Defines
//----------------------------------------------------------
#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2
#define ID_STATUS_GAME       3

class Settings : public QWidget, public Ui::Settings
{
public:
    Settings( QWidget* parent )
        : QWidget( parent )
    {
        setupUi(this);
    }
};

int is_paused = 0;

/**
    Constructor.
*/
KMahjongg::KMahjongg( QWidget* parent)
    : KMainWindow(parent)
{
    //Use up to 5MB for global application pixmap cache
    QPixmapCache::setCacheLimit(5*1024);
    // minimum area required to display the field
    setMinimumSize ( 320, 320 );
    // init board widget
    bw = new BoardWidget( this );
    setCentralWidget( bw );

    previewLoad = new Preview(this);

    setupStatusBar();
    setupKAction();

    QWidget *hbox = new QWidget(toolBar());
    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();

    gameTimer = new GameTimer(hbox);
    layout->addWidget(gameTimer);
    toolBar()->addWidget(hbox);

    theHighScores = new HighScore(this);


    bDemoModeActive = false;

    connect( bw, SIGNAL( statusTextChanged(const QString&, long) ),
                 SLOT( showStatusText(const QString&, long) ) );

    connect( bw, SIGNAL( tileNumberChanged(int,int,int) ),
                 SLOT( showTileNumber(int,int,int) ) );

    connect( bw, SIGNAL( demoModeChanged(bool) ),
                 SLOT( demoModeChanged(bool) ) );

    connect( bw, SIGNAL( gameOver(unsigned short , unsigned short)), this,
                SLOT( gameOver(unsigned short , unsigned short)));


    connect(bw, SIGNAL(gameCalculated()),
      this, SLOT(timerReset()));

    // Make connections for the preview load dialog
    connect( previewLoad, SIGNAL( boardRedraw(bool) ),
              bw,   SLOT( drawBoard(bool) ) );

    connect( previewLoad, SIGNAL( layoutChange() ),
              this,   SLOT( newGame() ) );


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
    KStdGameAction::restart(this, SLOT(restartGame()), actionCollection());

    KAction* newNumGame = new KAction(i18n("New Numbered Game..."), actionCollection(), "game_new_numeric");
    connect(newNumGame, SIGNAL(triggered(bool)), SLOT(startNewNumeric()));

    /*TODO reimplement with game type and preferences
    KAction* openTheme = new KAction(i18n("Open Th&eme..."), actionCollection(), "game_open_theme");
    connect(openTheme, SIGNAL(triggered(bool)), SLOT(openTheme()));

    KAction* openTileset = new KAction(i18n("Open &Tileset..."), actionCollection(), "game_open_tileset");
    connect(openTileset, SIGNAL(triggered(bool)), SLOT(openTileset()));

    KAction* openBkgnd = new KAction(i18n("Open &Background..."), actionCollection(), "game_open_background");
    connect(openBkgnd, SIGNAL(triggered(bool)), SLOT(openBackground()));*/

    KAction* openLayout = new KAction(i18n("Open La&yout..."), actionCollection(), "game_open_layout");
    connect(openLayout, SIGNAL(triggered(bool)), SLOT(openLayout()));

    /*TODO reimplement with game type and preferences
    KAction* saveTheme = new KAction(i18n("Sa&ve Theme..."), actionCollection(), "game_save_theme");
    connect(saveTheme, SIGNAL(triggered(bool)), SLOT(saveTheme()));*/

    // originally "file" ends here
    KStdGameAction::hint(bw, SLOT(helpMove()), actionCollection());
    KAction* shuffle = new KAction(KIcon("reload"), i18n("Shu&ffle"), actionCollection(), "move_shuffle");
    connect(shuffle, SIGNAL(triggered(bool)), bw, SLOT(shuffle()));
    KAction* angleccw = new KAction(i18n("Switch Angle Counter Clockwise"), actionCollection(), "view_angleccw");
    angleccw->setShortcut( KShortcut( "f"  ) );
    KAction* anglecw = new KAction(i18n("Switch Angle Clockwise"), actionCollection(), "view_anglecw");
    anglecw->setShortcut( KShortcut( "g"  ) );
    connect(angleccw, SIGNAL(triggered(bool)), bw, SLOT(angleSwitchCCW()));
    connect(anglecw, SIGNAL(triggered(bool)), bw, SLOT(angleSwitchCW()));
    demoAction = KStdGameAction::demo(this, SLOT(demoMode()), actionCollection());
    showMatchingTilesAction = new KToggleAction(i18n("Show &Matching Tiles"), actionCollection(), "options_show_matching_tiles");
    connect(showMatchingTilesAction, SIGNAL(triggered(bool)), SLOT(showMatchingTiles()));
    showMatchingTilesAction->setCheckedState(KGuiItem(i18n("Hide &Matching Tiles")));
    showMatchingTilesAction->setChecked(Prefs::showMatchingTiles());
    bw->setShowMatch( Prefs::showMatchingTiles() );
    KStdGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    pauseAction = KStdGameAction::pause(this, SLOT(pause()), actionCollection());

    //TODO maybe add standard resizing actions for the view?
    //we are currently using a resizable window for testing
    //KStdAction::actualSize(this, SLOT(makeFit()), actionCollection());
    //KStdAction::zoomIn(this, SLOT(makeLarger()), actionCollection());
    //KStdAction::zoomOut(this, SLOT(makeSmaller()), actionCollection());

    // TODO: store the background ; open on startup
    // TODO: same about layout
    // TODO: same about theme

    // move
    undoAction = KStdGameAction::undo(this, SLOT(undo()), actionCollection());
    redoAction = KStdGameAction::redo(this, SLOT(redo()), actionCollection());

    // edit
    KAction* boardEdit = new KAction(i18n("&Board Editor"), actionCollection(), "edit_board_editor");
    connect(boardEdit, SIGNAL(triggered(bool)), SLOT(slotBoardEditor()));

    // settings
    KStdAction::preferences(this, SLOT(showSettings()), actionCollection());

    setupGUI();
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
    statusBar()->addWidget(tilesLeftLabel);

    gameNumLabel = new QLabel("Game: 000000000000000000000", statusBar());
    gameNumLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(gameNumLabel, 1);


    statusLabel= new QLabel("Kmahjongg", statusBar());
    statusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(statusLabel);
}

void KMahjongg::setDisplayedWidth()
{
  bw->setDisplayedWidth();
/*  setFixedSize( bw->size() +
    QSize( 2, (!statusBar()->isHidden() ? statusBar()->height() : 0)
         + 2 + menuBar()->height() ) );
  toolBar()->setFixedWidth(bw->width());*/
  bw->drawBoard();
}


// ---------------------------------------------------------
void KMahjongg::startNewNumeric()
{
    bool ok;
    int s = KInputDialog::getInteger(i18n("New Game"),i18n("Enter game number:"),0,0,INT_MAX,1,&ok,this);
    if (ok) startNewGame(s);
}

void KMahjongg::undo()
{
    bw->Game->allow_redo += bw->undoMove();
    demoModeChanged(false);
}

void KMahjongg::redo()
{
    if (bw->Game->allow_redo >0) {
            bw->Game->allow_redo--;
            bw->redoMove();
            demoModeChanged(false);
    }
}

/**
 * Show Configure dialog.
 */
void KMahjongg::showSettings(){
  if(KConfigDialog::showDialog("settings"))
    return;

  KConfigDialog *dialog = new KConfigDialog(this, "settings", Prefs::self(), KPageDialog::Plain);
  dialog->addPage(new Settings(0), i18n("General"), "package_settings");
  connect(dialog, SIGNAL(settingsChanged(const QString &)), bw, SLOT(loadSettings()));
  connect(dialog, SIGNAL(settingsChanged(const QString &)), this, SLOT(setDisplayedWidth()));
  dialog->show();
}

void KMahjongg::demoMode()
{
    if( bDemoModeActive ) {
            bw->stopDemoMode();
    } else {
        // we assume demo mode removes tiles so we can
        // disbale redo here.
        bw->Game->allow_redo=false;
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
    Prefs::setShowMatchingTiles(!Prefs::showMatchingTiles());
    bw->setShowMatch( Prefs::showMatchingTiles() );
    showMatchingTilesAction->setChecked(Prefs::showMatchingTiles());
    Prefs::writeConfig();
}

void KMahjongg::showHighscores()
{
    theHighScores->exec(bw->getLayoutName());
}

/*TODO reimplement with game type and preferences
void KMahjongg::openTheme()
{
    previewLoad->initialise(Preview::theme);
    previewLoad->exec();
}

void KMahjongg::saveTheme()
{
    previewLoad->initialise(Preview::theme);
    previewLoad->saveTheme();
}*/

void KMahjongg::openLayout()
{
    previewLoad->initialise(Preview::board);
    previewLoad->exec();
}

/*TODO reimplement with game type and preferences
void KMahjongg::openBackground()
{
    previewLoad->initialise(Preview::background);
    previewLoad->exec();
}

void KMahjongg::openTileset()
{
    previewLoad->initialise(Preview::tileset);
    previewLoad->exec();
}*/

void KMahjongg::slotBoardEditor()
{
    Editor *boardEditor = new Editor(this);
    boardEditor->exec();
    delete boardEditor;
}

//----------------------------------------------------------
// signalled from the prieview dialog to generate a new game
// we don't make startNewGame a slot because it has a default
// param.

void KMahjongg::newGame()
{
    startNewGame();
}



// ---------------------------------------------------------
void KMahjongg::startNewGame( int item )
{
    if( ! bDemoModeActive ) {
        bw->calculateNewGame(item);

	// initialise button states
	bw->Game->allow_redo = bw->Game->allow_undo = 0;

	timerReset();

    	// update the initial enabled/disabled state for
    	// the menu and the tool bar.
    	demoModeChanged(false);
    }
}

// ---------------------------------------------------------
void KMahjongg::timerReset() {

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
        long gameNum = bw->getGameNum();
	KMessageBox::information(this, i18n("You have won!"));
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

	theHighScores->checkHighScore(score, elapsed, gameNum, bw->getBoardName());

	timerReset();
}

// ---------------------------------------------------------
void KMahjongg::showStatusText( const QString &msg, long board )
{
    statusLabel->setText(msg);
    QString str = i18n("Game number: %1", board);
    gameNumLabel->setText(str);

}

// ---------------------------------------------------------
void KMahjongg::showTileNumber( int iMaximum, int iCurrent, int iLeft )
{
    // Hmm... seems iCurrent is the number of remaining tiles, not removed ...
    //QString szBuffer = i18n("Removed: %1/%2").arg(iCurrent).arg(iMaximum);
    QString szBuffer = i18n("Removed: %1/%2  Combinations left: %3", iMaximum-iCurrent, iMaximum, iLeft);
    tilesLeftLabel->setText(szBuffer);

    // Update here since undo allow is effected by demo mode
    // removal. However we only change the enabled state of the
    // items when not in demo mode
    bw->Game->allow_undo = iMaximum != iCurrent;

    // update undo menu item, if demomode is inactive
    if( ! bDemoModeActive && !is_paused)
    {

//        pMenuBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
//        toolBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
        undoAction->setEnabled(bw->Game->allow_undo);
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
        undoAction->setEnabled(bw->Game->allow_undo);
	    redoAction->setEnabled(bw->Game->allow_redo);
    }
}

void KMahjongg::loadBoardLayout(const QString &file) {
	bw->loadBoardLayout(file);
}

void KMahjongg::tileSizeChanged() {
	bw->tileSizeChanged();
	setDisplayedWidth();
}


void KMahjongg::loadGame() {
//TODO adapt to new GameData class
/*
	GAMEDATA in;
	char buffer[1024];
    QString fname;

    // Get the name of the file to load
    KUrl url = KFileDialog::getOpenUrl( KUrl(), "*.kmgame", this, i18n("Load Game" ) );

    if ( url.isEmpty() )
	return;

    KIO::NetAccess::download( url, fname, this );

    // open the file for reading
    FILE *outFile = fopen( QFile::encodeName(fname), "r");
    if (outFile == NULL) {
	KMessageBox::sorry(this,
		i18n("Could not read from file. Aborting."));
	return;
    }

    // verify the magic
    fscanf(outFile, "%1023s\n", buffer);
    if (strcmp(buffer, gameMagic) != 0) {
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

    KIO::NetAccess::removeTempFile( fname );*/

    // refresh the board
    bw->gameLoaded();
}

void KMahjongg::restartGame() {
    if( ! bDemoModeActive ) {
        bw->calculateNewGame(bw->getGameNum());

        // initialise button states
        bw->Game->allow_redo = bw->Game->allow_undo = 0;

        timerReset();

        // update the initial enabled/disabled state for
        // the menu and the tool bar.
        demoModeChanged(false);
        if (is_paused)
        {
            pauseAction->setChecked(false);
            is_paused = false;
            bw->pause();
        }
    }
}

void KMahjongg::saveGame() {
//TODO adapt to new GameData class
/*
    // Get the name of the file to save
    KUrl url = KFileDialog::getSaveUrl( KUrl(), "*.kmgame", this, i18n("Save Game" ) );

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
    fprintf(outFile, "%s\n", gameTimer->toString().toUtf8().constData());


    // chuck in all the game data
    fwrite(&bw->Game, sizeof(GAMEDATA), 1, outFile);

    // close the file before exit
    fclose(outFile);*/
}


#include "kmahjongg.moc"
