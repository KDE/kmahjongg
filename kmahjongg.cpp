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

#include "kmahjongg.h"

#include <limits.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kconfigdialog.h>
#include <kinputdialog.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstandardgameaction.h>
#include <kstandardaction.h>
#include <kicon.h>
#include <KScoreDialog>

#include <QPixmapCache>
#include <QLabel>

#include <kio/netaccess.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>

#include "prefs.h"
#include "ui_settings.h"
#include "Editor.h"

#include <kmahjonggconfigdialog.h>

static const char *gameMagic = "kmahjongg-game-v1.0";

//----------------------------------------------------------
// Defines
//----------------------------------------------------------
#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2
#define ID_STATUS_GAME       3

/**
* This class implements
*
* longer description
*
* @author Mauricio Piacentini  <mauricio@tabuleiro.com>
*/
class Settings : public QWidget, public Ui::Settings
{
public:
    /**
     * Constructor */
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
    : KXmlGuiWindow(parent)
{
    //Use up to 3MB for global application pixmap cache
    QPixmapCache::setCacheLimit(3*1024);
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

    gameTimer = new KGameClock(this);

    connect( gameTimer, SIGNAL( timeChanged(const QString&)), this,
                SLOT( displayTime(const QString&)));

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
    delete bw;
}

// ---------------------------------------------------------
void KMahjongg::setupKAction()
{
    QAction *action;

    // game
    action = KStandardGameAction::gameNew(this, SLOT(newGame()), this);
    actionCollection()->addAction(action->objectName(), action);
    action = KStandardGameAction::load(this, SLOT(loadGame()), this);
    actionCollection()->addAction(action->objectName(), action);
    action = KStandardGameAction::save(this, SLOT(saveGame()), this);
    actionCollection()->addAction(action->objectName(), action);
    action = KStandardGameAction::quit(this, SLOT(close()), this);
    actionCollection()->addAction(action->objectName(), action);
    action = KStandardGameAction::restart(this, SLOT(restartGame()), this);
    actionCollection()->addAction(action->objectName(), action);

    QAction* newNumGame = actionCollection()->addAction("game_new_numeric");
    newNumGame->setText(i18n("New Numbered Game..."));
    connect(newNumGame, SIGNAL(triggered(bool)), SLOT(startNewNumeric()));

    /*TODO reimplement with game type and preferences
    QAction* openTheme = actionCollection()->addAction"game_open_theme");
    openTheme->setTexti18n("Open Th&eme...");
    connect(openTheme, SIGNAL(triggered(bool)), SLOT(openTheme()));

    QAction* openTileset = actionCollection()->addAction("game_open_tileset");
    openTileset->setText(i18n("Open &Tileset..."));
    connect(openTileset, SIGNAL(triggered(bool)), SLOT(openTileset()));*/

   /* QAction* openBkgnd = actionCollection()->addAction"game_open_background");
    openBkgnd->setTexti18n("Open &Background...");
    connect(openBkgnd, SIGNAL(triggered(bool)), SLOT(openBackground()));*/

    QAction* openLayout = actionCollection()->addAction("game_open_layout");
    openLayout->setText(i18n("Open La&yout..."));
    connect(openLayout, SIGNAL(triggered(bool)), SLOT(openLayout()));

    /*TODO reimplement with game type and preferences
    QAction* saveTheme = actionCollection()->addAction"game_save_theme");
    saveTheme->setTexti18n("Sa&ve Theme...");
    connect(saveTheme, SIGNAL(triggered(bool)), SLOT(saveTheme()));*/

    // originally "file" ends here
    action = KStandardGameAction::hint(bw, SLOT(helpMove()), this);
    actionCollection()->addAction(action->objectName(), action);
    QAction* shuffle = actionCollection()->addAction("move_shuffle");
    shuffle->setText(i18n("Shu&ffle"));
    shuffle->setIcon(KIcon("view-refresh"));
    connect(shuffle, SIGNAL(triggered(bool)), bw, SLOT(shuffle()));
    QAction* angleccw = actionCollection()->addAction("view_angleccw");
    angleccw->setText(i18n("Rotate View Counter Clockwise"));
    angleccw->setIcon(KIcon("object-rotate-right"));
    angleccw->setShortcuts( KShortcut( "f"  ) );
    QAction* anglecw = actionCollection()->addAction("view_anglecw");
    anglecw->setText(i18n("Rotate View Clockwise"));
    anglecw->setIcon(KIcon("object-rotate-left"));
    anglecw->setShortcuts( KShortcut( "g"  ) );
    connect(angleccw, SIGNAL(triggered(bool)), bw, SLOT(angleSwitchCCW()));
    connect(anglecw, SIGNAL(triggered(bool)), bw, SLOT(angleSwitchCW()));
    demoAction = KStandardGameAction::demo(this, SLOT(demoMode()), this);
    actionCollection()->addAction(demoAction->objectName(), demoAction);
    showMatchingTilesAction = new KToggleAction(i18n("Show &Matching Tiles"), this);
    actionCollection()->addAction("options_show_matching_tiles", showMatchingTilesAction);
    connect(showMatchingTilesAction, SIGNAL(triggered(bool)), SLOT(showMatchingTiles()));
    showMatchingTilesAction->setCheckedState(KGuiItem(i18n("Hide &Matching Tiles")));
    showMatchingTilesAction->setChecked(Prefs::showMatchingTiles());
    bw->setShowMatch( Prefs::showMatchingTiles() );
    action = KStandardGameAction::highscores(this, SLOT(showHighscores()), this);
    actionCollection()->addAction(action->objectName(), action);
    pauseAction = KStandardGameAction::pause(this, SLOT(pause()), this);
    actionCollection()->addAction(pauseAction->objectName(), pauseAction);

    //TODO maybe add standard resizing actions for the view?
    //we are currently using a resizable window for testing
    //KStandardAction::actualSize(this, SLOT(makeFit()), actionCollection());
    //KStandardAction::zoomIn(this, SLOT(makeLarger()), actionCollection());
    //KStandardAction::zoomOut(this, SLOT(makeSmaller()), actionCollection());

    // TODO: store the background ; open on startup
    // TODO: same about layout
    // TODO: same about theme

    // move
    undoAction = KStandardGameAction::undo(this, SLOT(undo()), this);
    actionCollection()->addAction(undoAction->objectName(), undoAction);
    redoAction = KStandardGameAction::redo(this, SLOT(redo()), this);
    actionCollection()->addAction(redoAction->objectName(), redoAction);

    // edit
    QAction* boardEdit = actionCollection()->addAction("edit_board_editor");
    boardEdit->setText(i18n("&Board Editor"));
    connect(boardEdit, SIGNAL(triggered(bool)), SLOT(slotBoardEditor()));

    // settings
    action = KStandardAction::preferences(this, SLOT(showSettings()), this);
    actionCollection()->addAction(action->objectName(), action);

    setupGUI();
}

// ---------------------------------------------------------
void KMahjongg::setupStatusBar()
{
    tilesLeftLabel= new QLabel(i18n("Removed: 0000/0000"), statusBar());
    tilesLeftLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(tilesLeftLabel, 1);

    gameNumLabel = new QLabel(i18n("Game: 000000000000000000000"), statusBar());
    gameNumLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(gameNumLabel);

    statusLabel= new QLabel("Kmahjongg", statusBar());
    statusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(statusLabel);

    gameTimerLabel = new QLabel(i18n("Time: 0:00:00"), statusBar());
    gameTimerLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    statusBar()->addWidget(gameTimerLabel);
}

void KMahjongg::displayTime(const QString& timestring)
{
    gameTimerLabel->setText(i18n("Time: ")+timestring);
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

  //Use the classes exposed by LibKmahjongg for our configuration dialog
  KMahjonggConfigDialog *dialog = new KMahjonggConfigDialog(this, "settings", Prefs::self());

  //The Settings class is ours
  dialog->addPage(new Settings(0), i18n("General"), "package_settings");
  dialog->addTilesetPage();
  dialog->addBackgroundPage();

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
    if (is_paused) gameTimer->resume(); else gameTimer->pause();
    is_paused = !is_paused;
    demoModeChanged(false);
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
    //theHighScores->exec(bw->getLayoutName());
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    ksdialog.setConfigGroup(bw->getLayoutName());
    ksdialog.exec();
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
	gameTimer->restart();

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

	int elapsed = gameTimer->seconds();

	time = score = 0;

	// get the time in milli secs
	// subtract from 20 minutes to get bonus. if longer than 20 then ignore
	time = (60*20) - gameTimer->seconds();
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

        //TODO: add gameNum as a Custom KScoreDialog field?
//	theHighScores->checkHighScore(score, elapsed, gameNum, bw->getBoardName());
        KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
        ksdialog.setConfigGroup(bw->getLayoutName());
        KScoreDialog::FieldInfo scoreInfo;
        scoreInfo[KScoreDialog::Score].setNum(score);
        scoreInfo[KScoreDialog::Time] = gameTimer->timeString();
        if(ksdialog.addScore( scoreInfo, KScoreDialog::AskName ))
          ksdialog.exec();

	bw->animateMoveList();

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
