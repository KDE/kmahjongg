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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <string.h>
#include <stdlib.h>

#include <qmsgbox.h>
#include <qtimer.h>
#include <qaccel.h>
#include <qfile.h>

#include <kmessagebox.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kio/netaccess.h>
#include <kmenubar.h>
#include <kstddirs.h>

#include "kmahjongg.moc"
#include "version.h"
#include "PrefsDlg.h"
#include <qpainter.h>
#include "Progress.h"

#include <kaboutdata.h>


static const char *description = I18N_NOOP("KDE Game");


//----------------------------------------------------------
// Defines
//----------------------------------------------------------
#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2
#define ID_STATUS_GAME	     3

#define ANIMSPEED    200

// tiles symbol names:
#define TILE_OFFSET      2

#define TILE_CHARACTER  ( 0 + TILE_OFFSET)
#define TILE_BAMBOO      ( 9 + TILE_OFFSET)
#define TILE_ROD         (18 + TILE_OFFSET)
#define TILE_SEASON      (27 + TILE_OFFSET)
#define TILE_WIND        (31 + TILE_OFFSET)
#define TILE_DRAGON      (36 + TILE_OFFSET)
#define TILE_FLOWER      (39 + TILE_OFFSET)

#define ID_FILE_NEW              101
#define ID_FILE_LOAD_TILESET     102
#define ID_FILE_LOAD_BACKGND     103
#define ID_FILE_LOAD_THEME       104
#define ID_FILE_LOAD_GAME        105
#define ID_FILE_LOAD_BOARD	 106
#define ID_FILE_SAVE_GAME        107
#define ID_FILE_EXIT             108
#define ID_FILE_NEW_NUMERIC	 109
#define ID_FILE_SAVE_THEME	 110

#define ID_EDIT_UNDO             201
#define ID_EDIT_REDO             202
#define ID_EDIT_BOARD_EDIT       203
#define ID_EDIT_PREFS            204


#define ID_GAME_HELP             301
#define ID_GAME_DEMO             302
#define ID_GAME_SHOW             303
#define ID_GAME_SHOW_HISCORE     305
#define ID_GAME_PAUSE            306
#define ID_GAME_CONTINUE	 307
#define ID_GAME_SHUFFLE		 308

#define ID_GAME_TIMER 		999

#define EXT_BACKGROUND		"*.bgnd"
#define EXT_TILESET		"*.tileset"
#define EXT_LAYOUT              "*.layout"
#define EXT_THEME		"*.theme"

int debug_flag = 0;

int is_paused = 0;

Progress *splash=NULL;

void progress(const QString& foo) {
	if (splash != NULL) {
		splash->status(foo);
	}
}


// ---------------------------------------------------------
int main( int argc, char** argv )
{
    KAboutData aboutData( "kmahjongg", I18N_NOOP("KMahjongg"),
      KMAHJONGG_VERSION, description, KAboutData::License_GPL,
      "(c) 1997, Mathias Mueller");
    aboutData.addAuthor("Mathias Mueller",0, "in5y158@public.uni-hamburg.de");
    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication a;

 //    splash = new Progress();
 //    splash->show();

     //p.repaint(0,0,-1,-1);

    KMahjonggWidget *w = new KMahjonggWidget();

 //   splash->hide();
    w->show();

    return( a.exec() );
}


// ---------------------------------------------------------
/**
    Constructor.
*/
KMahjonggWidget::KMahjonggWidget()
    : KMainWindow(0, "kmahjonggwidget")
{
    prefsDlg = new PrefsDlg(this);
    previewLoad = new Preview(this);
    setCaption("");
    boardEditor = 0;
    progress("Reading Preferences");
    preferences.initialise(KGlobal::config());

    bShowStatusbar = preferences.showStatus();

progress("Creating board widget");
    // init board widget
    bw = new BoardWidget( this );
    setCentralWidget( bw );

progress("Initialising status bar");
    setupStatusBar();

progress("Initialising menu bar");
    setupMenuBar();

progress("Setting up tool bar");
    setupToolBar();

progress("Initialising highscores");
    theHighScores = new HighScore(this);


progress("Initialising game number dialog");
    gameNum = new GameNum(this);

    bDemoModeActive = false;
    bShowMatchingTiles = false;

progress("Updating status bar");
    updateStatusbar( bShowStatusbar );

progress("Connecting signals");
    connect( bw, SIGNAL( statusTextChanged(const QString&, long) ),
                 SLOT( showStatusText(const QString&, long) ) );

    connect( bw, SIGNAL( message(const QString&) ),
                 SLOT( showMessage(const QString&) ) );

    connect( bw, SIGNAL( tileNumberChanged(int,int) ),
                 SLOT( showTileNumber(int,int) ) );

    connect( bw, SIGNAL( demoModeChanged(bool) ),
                 SLOT( demoModeChanged(bool) ) );

	connect(bw, SIGNAL(  gameOver(unsigned short , unsigned short)),
		    this, SLOT(  gameOver(unsigned short , unsigned short)));


	connect(bw, SIGNAL(gameCalculated(void)),
		this, SLOT(timerReset(void)));


    // preferences signals
    connect( prefsDlg, SIGNAL( boardRedraw(bool ) ),
                bw, SLOT( drawBoard(bool) ) );
    connect( prefsDlg, SIGNAL( statusBar(int) ),
                 SLOT( statusBarMode(int) ) );
    connect( prefsDlg, SIGNAL( backgroundModeChanged(void) ),
                 SLOT( backgroundMode(void) ) );
    connect( prefsDlg, SIGNAL(showRemovedChanged(void) ),
	      SLOT(setDisplayedWidth() ));
    connect( prefsDlg, SIGNAL(tileSizeChanged(void) ),
	      SLOT(tileSizeChanged() ));

	
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

progress("Starting new game");

    startNewGame(  );

}

// ---------------------------------------------------------
KMahjonggWidget::~KMahjonggWidget()
{
    delete pStatusBar;
    delete pMenuBar;
}



// ---------------------------------------------------------
void KMahjonggWidget::setupToolBar()
{
	toolBar = new KToolBar( this );

	// new game
	toolBar->insertButton(BarIcon("filenew"),
		ID_FILE_NEW, TRUE, i18n("New game."));
	// new numbered game
	toolBar->insertButton(UserIcon("newnum"),
		ID_FILE_NEW_NUMERIC, TRUE, i18n("New numbered game."));
	// open game
	toolBar->insertButton(BarIcon("fileopen"),
		ID_FILE_LOAD_GAME, TRUE, i18n("Open game."));
	// save game
	toolBar->insertButton(BarIcon("filesave"),
		ID_FILE_SAVE_GAME, TRUE, i18n(" Save game."));
	// undo move
	toolBar->insertButton(BarIcon("undo"),
		ID_EDIT_UNDO, TRUE, i18n("Undo move."));
	// redo move
	toolBar->insertButton(BarIcon("redo"),
		ID_EDIT_REDO, TRUE, i18n("Redo move."));
	// pause
	toolBar->insertButton(BarIcon("player_pause"),
		ID_GAME_PAUSE, TRUE, i18n("Pause game."));
	// play
	toolBar->insertButton(BarIcon("1rightarrow"),
		ID_GAME_CONTINUE, TRUE, i18n("Play game."));
	// Show hint

	toolBar->insertButton(BarIcon("help"),
		ID_GAME_HELP, TRUE, i18n("Hint."));
	


	// add the timer widget

   	 gameTimer = new GameTimer(toolBar);
    	toolBar->insertWidget(ID_GAME_TIMER, gameTimer->width() , gameTimer);

	toolBar->alignItemRight( ID_GAME_TIMER, true );
	toolBar->setBarPos(KToolBar::Top);
	toolBar->show();
	
    connect( toolBar,  SIGNAL(clicked(int) ), SLOT( menuCallback(int) ) );
}


// ---------------------------------------------------------
void KMahjonggWidget::setupStatusBar()
{
    pStatusBar = new KStatusBar( this );

    // The following isn't possible with the new KStatusBar anymore.
    // The correct fix is probably to reverse the order of adding the
    // widgets. :/
    // Just commenting it out for now (order is not as important
    // as compilation), in case someone comes up with a better fix.
    // pStatusBar->setInsertOrder( KStatusBar::RightToLeft );

    tilesLeftLabel= new QLabel("Removed: 0000/0000", pStatusBar);
    tilesLeftLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    pStatusBar->addWidget(tilesLeftLabel, tilesLeftLabel->sizeHint().width(), ID_STATUS_GAME);


    gameNumLabel = new QLabel("Game: 000000000000000000000", pStatusBar);
    gameNumLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    pStatusBar->addWidget(gameNumLabel, gameNumLabel->sizeHint().width(), ID_STATUS_TILENUMBER);


    statusLabel= new QLabel("Kmahjongg", pStatusBar);
    statusLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    pStatusBar->addWidget(statusLabel, statusLabel->sizeHint().width(), ID_STATUS_MESSAGE);

  //  pStatusBar->setAlignment( ID_STATUS_TILENUMBER, AlignCenter );

    pStatusBar->show();
}

// ---------------------------------------------------------
void KMahjonggWidget::updateStatusbar( bool bShow )
{
  //    pMenuBar->setItemChecked( ID_VIEW_STATUSBAR, bShow );
    if (bShow)
       statusBar()->show();
    else
       statusBar()->hide();
    setDisplayedWidth();

}

void KMahjonggWidget::setDisplayedWidth() {
    bw->setDisplayedWidth();
    setFixedSize( bw->size() +
                  QSize( 2, (preferences.showStatus() ? pStatusBar->height() : 0)
                     + 2 + pMenuBar->height() ) );
	toolBar->setFixedWidth(bw->width());
	toolBar->alignItemRight( ID_GAME_TIMER, true );
	bw->drawBoard();
}


// ---------------------------------------------------------
void KMahjonggWidget::setupMenuBar()
{

  // set up the file menu
  KPopupMenu *file = new KPopupMenu;
  file->insertItem(SmallIcon("filenew"), i18n("New game"), ID_FILE_NEW);
  file->insertItem(i18n("New numbered game..."), ID_FILE_NEW_NUMERIC);
  file->insertSeparator();
  file->insertItem(SmallIcon("fileopen"), i18n("&Open Game... "), ID_FILE_LOAD_GAME);
  file->insertItem(i18n("Open The&me..."), ID_FILE_LOAD_THEME);
  file->insertItem(i18n("Open &Tileset..."), ID_FILE_LOAD_TILESET);
  file->insertItem(i18n("Open &Background..."), ID_FILE_LOAD_BACKGND);
  file->insertItem( i18n("Open &Layout..."), ID_FILE_LOAD_BOARD );
  file->insertSeparator();
  file->insertItem(i18n("&Save Theme..."), ID_FILE_SAVE_THEME);
  file->insertItem(SmallIcon("filesave"), i18n("&Save Game..."), ID_FILE_SAVE_GAME);
  file->insertSeparator();
  file->insertItem(SmallIcon("exit"), i18n("&Quit "), ID_FILE_EXIT);

  KPopupMenu *edit = new KPopupMenu;
  edit->insertItem(SmallIcon("undo"), i18n("&Undo"), ID_EDIT_UNDO);
  edit->insertItem(SmallIcon("redo"), i18n("&Redo"), ID_EDIT_REDO);
  edit->insertSeparator();
  edit->insertItem(i18n("&Board editor..."), ID_EDIT_BOARD_EDIT);
  edit->insertSeparator();
  edit->insertItem(SmallIcon("configure"), i18n("&Preferences..."), ID_EDIT_PREFS);


  KPopupMenu *game = new KPopupMenu;
  game->insertItem( i18n("&Help me"),           ID_GAME_HELP );
  game->insertItem( SmallIcon("reload"), i18n("Shu&ffle"),           ID_GAME_SHUFFLE );
  game->insertItem( i18n("&Demo mode"),         ID_GAME_DEMO );
  game->insertItem( i18n("Show &matching tiles"), ID_GAME_SHOW );
  game->insertItem( i18n("&Show Hiscores..."), ID_GAME_SHOW_HISCORE );
  game->insertSeparator();
  game->insertItem( SmallIcon("player_pause"), i18n("&Pause"), ID_GAME_PAUSE );
  game->insertItem( SmallIcon("1rightarrow"), i18n("&Continue"), ID_GAME_CONTINUE );

  QPopupMenu *help = helpMenu(QString(i18n("Mahjongg"))
           + " " + KMAHJONGG_VERSION
           + i18n("\n\nOriginal version by Mathias Mueller")
           + " (in5y158@public.uni-hamburg.de)"
           + i18n("\n\nRewritten and extended by ")
           + "David Black"
           + " (david.black@connectfree.co.uk)"
	   + i18n("\n\nTile set contributor and web page maintainer: ")
	   + "Osvaldo Stark (starko@dnet.it)"
	   + i18n("\nsee http://freeweb.dnet.it/kmj/ for tile sets and layouts") );

    pMenuBar = new KMenuBar( this );

    pMenuBar->insertItem( i18n("&File"), file );
    pMenuBar->insertItem( i18n("&Edit"), edit );
    pMenuBar->insertItem( i18n("&Game"), game );
    pMenuBar->insertSeparator();
    pMenuBar->insertItem( i18n("&Help"), help );

    // initialize menu accelerators
    //
    // file menu
    pMenuBar->setAccel(CTRL+Key_N, ID_FILE_NEW);
    pMenuBar->setAccel(CTRL+Key_O, ID_FILE_LOAD_GAME);
    pMenuBar->setAccel(CTRL+Key_S, ID_FILE_SAVE_GAME);
    pMenuBar->setAccel( CTRL+Key_Q, ID_FILE_EXIT );

    pMenuBar->setAccel(CTRL+Key_Z, ID_EDIT_UNDO);

    pMenuBar->show();

    connect( pMenuBar,  SIGNAL(activated(int) ), SLOT( menuCallback(int) ) );
}

// ---------------------------------------------------------
void KMahjonggWidget::menuCallback( int item )
{
    switch( item )
    {
        case ID_FILE_NEW:
	  	startNewGame();
	  	break;
	case ID_FILE_NEW_NUMERIC:
		if (gameNum->exec()) {
			startNewGame(gameNum->getNumber());
		}
		break;
        case ID_FILE_EXIT:
	    	// write configuration
	    	kapp->quit();
            	break;
        case ID_GAME_HELP:
            	bw->helpMove();
            	break;
        case ID_EDIT_UNDO:
            	bw->Game.allow_redo += bw->undoMove();
	    	demoModeChanged(false);
            	break;
        case ID_EDIT_REDO:
	    	if (bw->Game.allow_redo >0) {
            		bw->Game.allow_redo--;
			bw->redoMove();
	    		demoModeChanged(false);
	    	}
            	break;
        case ID_EDIT_PREFS:
	      	prefsDlg->initialise();
	      	prefsDlg->exec();
            preferences.sync();
            	break;

        case ID_GAME_DEMO:
            	if( bDemoModeActive ) {
                	bw->stopDemoMode();
            	} else {
			// we assume demo mode removes tiles so we can
			// disbale redo here.
			bw->Game.allow_redo=false;	
            		bw->startDemoMode();
	    	}
            	break;

        case ID_GAME_PAUSE:
        case ID_GAME_CONTINUE:
	    	is_paused = !is_paused;
	    	demoModeChanged(false);
	    	gameTimer->pause();
	    	bw->pause();
            	break;
	case ID_GAME_SHUFFLE:
		bw->shuffle();
		break;
        case ID_GAME_SHOW:
            	bShowMatchingTiles = ! bShowMatchingTiles;
            	bw->setShowMatch( bShowMatchingTiles );
            	pMenuBar->setItemChecked( ID_GAME_SHOW, bShowMatchingTiles );
            	break;
        case ID_FILE_LOAD_BOARD:
	    	previewLoad->initialise(Preview::board, EXT_LAYOUT);
	 	previewLoad->exec();
	    	break;
        case ID_FILE_LOAD_BACKGND:
	    	previewLoad->initialise(Preview::background, EXT_BACKGROUND);
	    	previewLoad->exec();
	    	break;
        case ID_FILE_LOAD_TILESET:
	    	previewLoad->initialise(Preview::tileset, EXT_TILESET);
	    	previewLoad->exec();
	    	break;
        case ID_FILE_LOAD_THEME:
	    	previewLoad->initialise(Preview::theme, EXT_THEME);
	    	previewLoad->exec();
	    	break;
	case ID_EDIT_BOARD_EDIT:
		if (!boardEditor)
		    boardEditor = new Editor(this);
		boardEditor->exec();
		break;
	case ID_GAME_SHOW_HISCORE:
		theHighScores->exec(bw->getLayoutName());
		break;
	case ID_FILE_LOAD_GAME:
		loadGame();
		break;
	case ID_FILE_SAVE_GAME:
		saveGame();
		break;
	case ID_FILE_SAVE_THEME:
	    	previewLoad->initialise(Preview::theme, EXT_THEME);
	    	previewLoad->saveTheme();
		break;
    }
}


//----------------------------------------------------------
// signalled from the prieview dialog to generate a new game
// we don't make startNewGame a slot because it has a default
// param.

void KMahjonggWidget::newGame(void) {
	startNewGame();

}



// ---------------------------------------------------------
void KMahjonggWidget::startNewGame( int item )
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
void KMahjonggWidget::timerReset(void) {

	// initialise the scoring system
	gameElapsedTime = 0;

	// start the game timer
	gameTimer->start();

}


// ---------------------------------------------------------

void KMahjonggWidget::gameOver(
	unsigned short numRemoved,
	unsigned short cheats)
{
	int time;
	int score;

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

}


// ---------------------------------------------------------
void KMahjonggWidget::closeEvent( QCloseEvent* e )
{
    kapp->quit();
    e->accept();
}

// ---------------------------------------------------------
void KMahjonggWidget::showStatusText( const QString &msg, long board )
{
    statusLabel->setText(msg);
    QString str = i18n("Game Number: %1").arg(board);
    gameNumLabel->setText(str);

}

// ---------------------------------------------------------
void KMahjonggWidget::showMessage( const QString &msg )
{
    KMessageBox::information( this, msg );
}

// ---------------------------------------------------------
void KMahjonggWidget::showTileNumber( int iMaximum, int iCurrent )
{
    // Hmm... seems iCurrent is the number of remaining tiles, not removed ...
    //QString szBuffer = i18n("Removed: %1/%2").arg(iCurrent).arg(iMaximum);
    QString szBuffer = i18n("Removed: %1/%2").arg(iMaximum-iCurrent).arg(iMaximum);
    tilesLeftLabel->setText(szBuffer);

    // Update here since undo allow is effected by demo mode
    // removal. However we only change the enabled state of the
    // items when not in demo mode
    bw->Game.allow_undo = iMaximum != iCurrent;

    // update undo menu item, if demomode is inactive
    if( ! bDemoModeActive && !is_paused)
    {
        pMenuBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
        toolBar->setItemEnabled( ID_EDIT_UNDO, bw->Game.allow_undo);
    }
}


// ---------------------------------------------------------
// Set a menu button and tool bar items enabled state. Saves
// a lot of redundant code for menu's and toolbars.
void KMahjonggWidget::enableItem(int item, bool state)
{
	pMenuBar->setItemEnabled( item, state);
    	toolBar->setItemEnabled( item,  state);
}


// ---------------------------------------------------------
void KMahjonggWidget::demoModeChanged( bool bActive)
{
    bDemoModeActive = bActive;

    if (bActive || is_paused) {

	
        enableItem( ID_EDIT_UNDO, false);
        enableItem( ID_EDIT_REDO, false);
		if (!is_paused) {
	      enableItem( ID_GAME_PAUSE, false);
	      enableItem( ID_GAME_CONTINUE, false);
	      enableItem( ID_GAME_DEMO, true);
		} else {
		    enableItem( ID_GAME_PAUSE, !is_paused);
		    enableItem( ID_GAME_CONTINUE, is_paused);
	        enableItem( ID_GAME_DEMO, false);
		}
    } else {
        enableItem( ID_EDIT_UNDO, bw->Game.allow_undo);
        enableItem( ID_EDIT_REDO, bw->Game.allow_redo);
	    enableItem( ID_GAME_PAUSE, !is_paused);
	    enableItem( ID_GAME_CONTINUE, is_paused);
	    enableItem( ID_GAME_DEMO, true);
    }

    enableItem( ID_FILE_LOAD_TILESET, !bActive && !is_paused );
    enableItem( ID_FILE_NEW,  !bActive  && !is_paused);
    enableItem( ID_FILE_LOAD_BACKGND, !bActive && !is_paused );
    enableItem( ID_FILE_LOAD_THEME, !bActive  && !is_paused);
    enableItem( ID_FILE_LOAD_GAME, !bActive  && !is_paused);
    enableItem( ID_FILE_SAVE_GAME, !bActive  && !is_paused);

    enableItem( ID_EDIT_BOARD_EDIT, !bActive  && !is_paused);
    enableItem( ID_EDIT_PREFS, !bActive  && !is_paused);

    enableItem( ID_GAME_HELP, !bActive  && !is_paused);
    enableItem( ID_GAME_SHOW, !bActive  && !is_paused);
    enableItem( ID_GAME_SHOW_HISCORE, !bActive  && !is_paused);
    enableItem( ID_FILE_LOAD_BOARD, !bActive  && !is_paused);

}


void KMahjonggWidget::statusBarMode(int onOff) {
	updateStatusbar(onOff);
}


void BoardWidget::getFileOrDefault(QString filename, QString type, QString &res)
{
	QString picsPos = "pics/";
	picsPos += "default.";
	picsPos += type;

	QFile test;
	test.setName(filename);
	if (test.exists()) {
		res = filename;
	}
        else {
	        res = locate("appdata", picsPos);
	}

        if (res.isEmpty()) {
		KMessageBox::error(this,
			    i18n("Kmahjongg could not locate the file: ") +
			    filename +
			    i18n("\nOr the default file of type: ")+
			    type +
			    i18n("\nKmahjongg will now terminate") );
		exit(0);	
	}
}
	

void KMahjonggWidget::backgroundMode(void) {
	bw->updateScaleMode();
}

void KMahjonggWidget::loadBoardLayout(const QString &file) {
	bw->loadBoardLayout(file);
}

void KMahjonggWidget::tileSizeChanged(void) {
	bw->tileSizeChanged();
	setDisplayedWidth();
}


void KMahjonggWidget::loadGame(void) {
	GAMEDATA in;
	char buffer[1024];
    QString fname;

    // Get the name of the file to load
    KURL url = KFileDialog::getOpenURL( NULL, "*.kmgame", this, i18n("Load game." ) );

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
    fscanf(outFile, "%s\n", buffer);
    if (buffer != gameMagic) {
	KMessageBox::sorry(this,
		i18n("File format not recognised."));
	fclose(outFile);
	return;
    }

    //ed the elapsed time
    fscanf(outFile, "%s\n", buffer);
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

void KMahjonggWidget::saveGame(void) {

    // Get the name of the file to save
    KURL url = KFileDialog::getSaveURL( NULL, "*.kmgame", this, i18n("Save game." ) );

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


// ---------------------------------------------------------
/**
    Constructor.
    Loads tileset and background bitmaps.
*/
BoardWidget::BoardWidget( QWidget* parent )
  : QWidget( parent ), theTiles(preferences.miniTiles())
{
    setBackgroundColor( QColor( 0,0,0 ) );

    TimerState = Stop;
    gamePaused = false;
    iTimerStep = 0;
    matchCount = 0;
    showMatch = false;
    MouseClickPos1.e = BoardLayout::depth;     // mark tile position as invalid
    MouseClickPos2.e = BoardLayout::depth;

    // initially we force a redraw
    updateBackBuffer=true;




    // Load tileset. First try to load the last use tileset

    QString tFile;


progress("Loading tileset");
    getFileOrDefault(preferences.tileset(), "tileset", tFile);

    if (!loadTileset(tFile)){
	KMessageBox::error(this,
		   i18n("An error ocurred when loading the tileset file\n")+
		   tFile +
		   i18n("Kmahjongg will now terminate"));
	exit(0);
    }

progress("loading background image");
    getFileOrDefault(preferences.background(), "bgnd", tFile);

    // Load background
    if( ! loadBackground(tFile, false ) )
    {
	KMessageBox::error(this,
		   i18n("An error ocurred when loading the background image\n")+
		   tFile +
		   i18n("Kmahjongg will now terminate"));
	exit(0);
    }

progress("loading layout");
    getFileOrDefault(preferences.layout(), "layout", tFile);
    if( ! loadBoardLayout(tFile) )
    {
	KMessageBox::error(this,
		   i18n("An error ocurred when loading the board layout\n")+
		   tFile +
		   i18n("Kmahjongg will now terminate"));
	exit(0);
    }
    setDisplayedWidth();	

progress("drawing board");
    show();
}

void BoardWidget::setDisplayedWidth() {

    if (preferences.showRemoved()){
    	setFixedSize( requiredWidth() , requiredHeight());
    } else {
    	setFixedSize( requiredWidth() - ((theTiles.width())*4)
		, requiredHeight());
    }
}


// for a given cell x y calc how that cell is shadowed
// returnd left = width of left hand side shadow
// t = height of top shadow
// c = width and height of corner shadow

void BoardWidget::calcShadow(int e, int y, int x, int &l, int &t, int &c) {
	
	l = t = c = 0;
	if ((Game.shadowHeight(e,y,x) != 0) ||
	   (Game.shadowHeight(e,y-1,x) != 0) ||
	   (Game.shadowHeight(e,y,x-1) != 0)) {
		return;
	}	
	int a,b;

	a=Game.shadowHeight(e,y,x-2);
	b=Game.shadowHeight(e,y-1,x-2);
	if (a != 0 || b != 0)
	   l = (a>b) ? a : b;
	a=Game.shadowHeight(e,y-2,x);
	b=Game.shadowHeight(e,y-2,x-1);
	if (a != 0 || b != 0)
	   t = (a>b) ? a : b;

	c = Game.shadowHeight(e, y-2, x-2);
}

// draw a triangular shadow from the top right to the bottom left.
// one such shadow is a right hand edge of a shadow line.
// if a second shadow botton left to top right is rendered over it
// then the shadow becomes a box (ie in the middle of the run)

void BoardWidget::shadowTopLeft(int depth, int sx, int sy, int rx, int ry, QPixmap *src, bool flag) {
	if (depth) {
		int shadowPixels= (depth+1) * theTiles.shadowSize();
		int xOffset=theTiles.qWidth()-shadowPixels;
	    	for (int p=0; p<shadowPixels; p++) {	
             	    bitBlt( &backBuffer,
			    sx+xOffset,	sy+p,	
                    	    src,
			    rx+xOffset,	ry+p,
			    shadowPixels-p,
			    1, CopyROP );
		}
		// Now aafter rendering the triangle, fill in the rest of
		// the quater width
		if (flag && ((theTiles.qWidth() - shadowPixels) > 0))
             	    bitBlt( &backBuffer,
			    sx,	sy,	
                    	    src,
			    rx,	ry,
			    theTiles.qWidth() - shadowPixels,
			    shadowPixels, CopyROP );
	}
}

// Second triangular shadow generator see above
void BoardWidget::shadowBotRight(int depth, int sx, int sy, int rx, int ry, QPixmap *src, bool flag) {
	if (depth) {
		int shadowPixels= (depth+1) * theTiles.shadowSize();
		int xOffset=theTiles.qWidth();
	    	for (int p=0; p<shadowPixels; p++) {	
             	    bitBlt( &backBuffer,
			    sx+xOffset-p, 	/* step to shadow right start */
			    sy+p,		/* down for each line */
                    	    src,
			    rx+xOffset-p,	/* step to shadow right start */
			    ry+p,
			    p, 			/* increace width each line down */
			    1, CopyROP );
		}
		if (flag && ((theTiles.qHeight() - shadowPixels) >0))
             	    bitBlt( &backBuffer,
			    sx+xOffset-shadowPixels,	
			    sy+shadowPixels,	
                    	    src,
			    rx+xOffset-shadowPixels,
			    ry+shadowPixels,
			    shadowPixels,
			    theTiles.qHeight()-shadowPixels, CopyROP );

	}
}




void BoardWidget::shadowArea(int z, int y, int x, int sx, int sy,int rx, int ry, QPixmap *src)
{
	// quick check to see if we are obscured
	if (z < BoardLayout::depth-1) 	{
	    if ((x >= 0) && (y<BoardLayout::height)) {
		if (Game.Mask[z+1][y][x] && Game.Board[z+1][y][x]) {
			return;
		}
	    }
	}




	// offset to pass tile depth indicator
	sx+=theTiles.shadowSize();
	rx+=theTiles.shadowSize();
	


	// We shadow the top right hand edge of the tile with a
	// triangular border. If the top shadow covers it all
	// well and good, otherwise if its smaller, part of the
	// triangle will show through.

	shadowTopLeft(Game.shadowHeight(z+1, y-1, x), sx, sy, rx,ry,src, true);
	shadowBotRight(Game.shadowHeight(z+1, y, x+1), sx, sy, rx, ry, src, true);
	shadowTopLeft(Game.shadowHeight(z+1, y-1, x+1), sx, sy, rx,ry,src, false);
	shadowBotRight(Game.shadowHeight(z+1, y-1, x+1), sx, sy, rx, ry, src, false);

	return;

}

// ---------------------------------------------------------
void BoardWidget::paintEvent( QPaintEvent* pa )
{
    QPixmap  *back;

    int xx = pa->rect().left();
    int xheight = pa->rect().height();
    int xwidth  = pa->rect().width();


    if (gamePaused) {
        // If the game is paused, then blank out the board.
        // We tolerate no cheats around here folks..
        bitBlt( this, xx, pa->rect().top(),
                theBackground.getBackground(), xx,
                pa->rect().top(), xwidth, xheight, CopyROP );
	return;
    }

    // if the repaint is because of a window redraw after a move
    // or a menu roll up, then just blit in the last rendered image
    if (!updateBackBuffer) {
    	bitBlt(this, xx,pa->rect().top(),
		&backBuffer, xx, pa->rect().top(), xwidth, xheight, CopyROP);
	return;
    }

    // update the complete drawArea

    back = theBackground.getBackground();


    backBuffer.resize(back->width(), back->height());



    // erase out with the background
    bitBlt( &backBuffer, xx, pa->rect().top(),
                theBackground.getBackground(), xx,pa->rect().top(), xwidth, xheight, CopyROP );

    // initial offset on the screen of tile 0,0
    int xOffset = theTiles.width()/2;
    int yOffset = theTiles.height()/2;
    //short tile = 0;

    // shadow the background first
    if (preferences.showShadows()) {
        for (int by=0; by <BoardLayout::height+1; by++)
	    for (int bx=-1; bx < BoardLayout::width+1; bx++)
 	        shadowArea(-1, by, bx,
			bx*theTiles.qWidth()+xOffset-theTiles.shadowSize(), 	
			by*theTiles.qHeight()+yOffset+theTiles.shadowSize(),
			bx*theTiles.qWidth()+xOffset-theTiles.shadowSize(), 	
			by*theTiles.qHeight()+yOffset+theTiles.shadowSize(),
			theBackground.getShadowBackground());
    }




    // we iterate over the depth stacking order. Each sucessive level is
    // drawn one indent up and to the right. The indent is the width
    // of the 3d relief on the tile left (tile shadow width)
    for (int z=0; z<BoardLayout::depth; z++) {
        // we draw down the board so the tile below over rights our border
        for (int y = 0; y < BoardLayout::height; y++) {
            // drawing right to left to prevent border overwrite
            for (int x=BoardLayout::width-1; x>=0; x--) {
                int sx = x*(theTiles.qWidth()  )+xOffset;
                int sy = y*(theTiles.qHeight()  )+yOffset;



		// skip if no tile to display
		if (!Game.tilePresent(z,y,x))
			continue;

                QPixmap *t;
		QPixmap *s;
		if (Game.hilighted[z][y][x]) {
		   t= theTiles.selectedPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		   s= theTiles.selectedShadowPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		} else {
		   t= theTiles.unselectedPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
		   s= theTiles.unselectedShadowPixmaps(
				Game.Board[z][y][x]-TILE_OFFSET);
                }

                // Only one compilcation. Since we render top to bottom , left
                // to right situations arise where...:
                // there exists a tile one q height above and to the left
                // in this situation we would draw our top left border over it
                // we simply split the tile draw so the top half is drawn
                // minus border

                if (x > 1 && y > 0 && Game.tilePresent(z, y-1, x-2)){
                    bitBlt( &backBuffer,
			    sx+theTiles.shadowSize(), sy,
                            t, theTiles.shadowSize() ,0,
		            t->width()-theTiles.shadowSize(),
		  	    t->height()/2, CopyROP );
                    bitBlt( &backBuffer, sx, sy+t->height()/2,
                        t, 0,t->height()/2,t->width(),t->height()/2,CopyROP);
                } else {

                bitBlt( &backBuffer, sx, sy,
                    t, 0,0, t->width(), t->height(), CopyROP );
                }

		
		if (preferences.showShadows() && z<BoardLayout::depth - 1) {
		    for (int xp = 0; xp <= 1; xp++)  {
			for (int yp=0; yp <= 1; yp++) {
				shadowArea(z, y+yp, x+xp,
					sx+(xp*theTiles.qWidth()),
					sy+(yp*theTiles.qHeight()),
					xp*theTiles.qWidth(),
					yp*theTiles.qHeight(),
					s);
			}
		    }

		}



            }
        }
        xOffset +=theTiles.shadowSize();
        yOffset -=theTiles.shadowSize();
    }





    // Now we add the list of cancelled tiles

    // we start blitting as usuall right to left, top to bottom, first
    // we calculate the start pos of the first tile, allowing space for
    // the upwards at rightwards creep when stacking in 3d
    unsigned short xPos = backBuffer.width()-(3*theTiles.shadowSize())-theTiles.width();
    unsigned short yPos = (3*theTiles.shadowSize());

    for (int pos=0; pos < 9; pos++) {
	int last = 0;
	int tile=0;
	// dragon?
	if (pos >= 0 && pos < 3) {
		last = removedDragon[pos];
		tile = TILE_DRAGON+pos;
	} else {
	    //Wind?
	    if (pos >= 3 && pos < 7) {
	        last = removedWind[pos-3];
		tile = TILE_WIND+pos-3;
	    } else {
		if (pos == 7) {
		    for (int t=0; t<4;t++) {
		        if (removedFlower[t]) {
			    last++;
			    tile=TILE_FLOWER+t;
			}
		    }			
		} else {
		    for (int t=0; t<4;t++) {
		        if (removedSeason[t]) {
			    last++;
			    tile=TILE_SEASON+t;
			}
		    }
		}
	    }
	}

	    stackTiles(tile, last, xPos, yPos);
	    stackTiles(TILE_ROD+pos, removedRod[pos],
		xPos - (1*(theTiles.width() - theTiles.shadowSize())) , yPos);
	    stackTiles(TILE_BAMBOO+pos, removedBamboo[pos],
		xPos - (2*(theTiles.width() - theTiles.shadowSize())) , yPos);
	    stackTiles(TILE_CHARACTER+pos, removedCharacter[pos],
		xPos - (3*(theTiles.width() - theTiles.shadowSize())) , yPos);
	


	yPos += theTiles.height()-theTiles.shadowSize();
    }


    updateBackBuffer=false;
    bitBlt(this, xx,pa->rect().top(), &backBuffer, xx, pa->rect().top(), xwidth, xheight, CopyROP);


}

void BoardWidget::stackTiles(unsigned char t, unsigned short h, unsigned short x,unsigned  short y)
{

    int ss = theTiles.shadowSize();
    QPainter p(&backBuffer);
    QPen line;
    p.setBackgroundMode(OpaqueMode);
    p.setBackgroundColor(black);




    line.setWidth(1);	
    line.setColor(white);	
    p.setPen(line);
    int x2 = x+theTiles.width()-ss-1;
    int y2 = y+theTiles.height()-1;
    p.drawLine(x, y+ss, x2, y+ss);
    p.drawLine(x, y+ss, x, y2);
    p.drawLine(x2, y+ss, x2, y2);
    p.drawLine(x+1, y2, x2, y2);

   // p.fillRect(x+1, y+ss+1, theTiles.width()-ss-2, theTiles.height()-ss-2, QBrush(lightGray));

    for (unsigned short pos=0; pos < h; pos++) {
       QPixmap *p = theTiles.unselectedPixmaps(t-TILE_OFFSET);
       bitBlt( &backBuffer, x+(pos*ss), y-(pos*ss),
                    p, 0,0, p->width(), p->height(), CopyROP );  	
    }

}


void BoardWidget::pause() {
	gamePaused = !gamePaused;
	drawBoard(true);
}
// ---------------------------------------------------------
int BoardWidget::undoMove()
{
    cancelUserSelectedTiles();

    if( Game.TileNum < Game.MaxTileNum )
    {
	
        clearRemovedTilePair(Game.MoveList[Game.TileNum], Game.MoveList[Game.TileNum+1]);
        putTile( Game.MoveList[Game.TileNum], false );
        Game.TileNum++;
        putTile( Game.MoveList[Game.TileNum] );
        Game.TileNum++;
        drawTileNumber();
        setStatusText( i18n("Undo operation done successfully.") );
	return 1;
    }
    else {
        setStatusText(i18n("What do you want to undo? You have done nothing!"));
	return 0;
	}
}

// ---------------------------------------------------------
void BoardWidget::helpMove()
{
    cancelUserSelectedTiles();
    stopMatchAnimation();

    if( findMove( TimerPos1, TimerPos2 ) )
    {
		cheatsUsed++;
        iTimerStep = 1;
        helpMoveTimeout();
    }
    else
        setStatusText( i18n("Sorry, you have lost the game.") );
}
// ---------------------------------------------------------
void BoardWidget::helpMoveTimeout()
{
    if( iTimerStep & 1 )
    {
        hilightTile( TimerPos1, true, false );
        hilightTile( TimerPos2, true );
    }
    else
    {
        hilightTile( TimerPos1, false, false );
        hilightTile( TimerPos2, false );
    }
    // restart timer
    if( iTimerStep++ < 8 )
        QTimer::singleShot( ANIMSPEED, this, SLOT( helpMoveTimeout() ) );
}

// ---------------------------------------------------------
void BoardWidget::startDemoMode()
{
    cancelUserSelectedTiles();
    stopMatchAnimation();
    calculateNewGame();

    if( TimerState == Stop )
    {
        TimerState = Demo;
        iTimerStep = 0;
        emit demoModeChanged( true );
        setStatusText( i18n("Demo mode. Click mousebutton to stop.") );
        demoMoveTimeout();
    }
}
// ---------------------------------------------------------
void BoardWidget::stopDemoMode()
{
    TimerState = Stop;    // stop demo
    calculateNewGame();
    setStatusText( i18n("Now it's you again.") );
    emit demoModeChanged( false );
    emit gameCalculated();
}
// ---------------------------------------------------------
void BoardWidget::demoMoveTimeout()
{
    switch( iTimerStep++ % 6 )
    {
        // at firts, find new matching tiles
        case 0:
            if( ! findMove( TimerPos1, TimerPos2 ) )
	    {
                // if computer has won
	        if( Game.TileNum == 0 )
                {
                    animateMoveList();
                }		
                // else computer has lost
                else
                {
                    setStatusText( i18n("Your computer has lost the game.") );
                    while( Game.TileNum < Game.MaxTileNum )
                    {
                        putTile( Game.MoveList[Game.TileNum], false );
                        Game.TileNum++;
                        putTile( Game.MoveList[Game.TileNum] );
                        Game.TileNum++;
                        drawTileNumber();
                    }
                }
                TimerState = Stop;
                startDemoMode();
                return;
            }
            break;
	// hilight matching tiles two times
        case 1:
        case 3:
            if( TimerState == Demo )
	    {
                hilightTile( TimerPos1, true, false );
                hilightTile( TimerPos2, true );
            }
            break;
	
        case 2:
        case 4:
            hilightTile( TimerPos1, false, false );
            hilightTile( TimerPos2, false );
            break;
	// remove matching tiles from game board
        case 5:
            setRemovedTilePair(TimerPos1, TimerPos2);	
            removeTile( TimerPos1, false );
            removeTile( TimerPos2 );
            drawTileNumber();
            break;
    }
    // restart timer if demo mode is still active
    if( TimerState == Demo )
        QTimer::singleShot( ANIMSPEED, this, SLOT( demoMoveTimeout() ) );
}

// ---------------------------------------------------------
void BoardWidget::setShowMatch( bool show )
{
    if( showMatch )
        stopMatchAnimation();
    showMatch = show;
}
// ---------------------------------------------------------
void BoardWidget::matchAnimationTimeout()
{
    if (matchCount == 0)
        return;

    if( iTimerStep++ & 1 )
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {


            hilightTile(PosTable[Pos], true);
        }
    }
    else
    {
        for(short Pos = 0; Pos < matchCount; Pos++)
        {
            hilightTile(PosTable[Pos], false);
        }
    }
    if( TimerState == Match )
        QTimer::singleShot( ANIMSPEED, this, SLOT( matchAnimationTimeout() ) );
}
// ---------------------------------------------------------
void BoardWidget::stopMatchAnimation()
{
    for(short Pos = 0; Pos < matchCount; Pos++)
    {
        hilightTile(PosTable[Pos], false);
    }
    TimerState = Stop;
    matchCount = 0;
}

void BoardWidget::redoMove(void)
{
        	
	setRemovedTilePair(Game.MoveList[Game.TileNum-1],Game.MoveList[Game.TileNum-2]);
        removeTile(Game.MoveList[Game.TileNum-1], false);
        removeTile(Game.MoveList[Game.TileNum-1]);
        drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::animateMoveList()
{
    setStatusText( i18n("Congratulations. You have won!") );

    while( Game.TileNum < Game.MaxTileNum )
    {
        // put back all tiles
        putTile(Game.MoveList[Game.TileNum]);
        Game.TileNum++;
        putTile(Game.MoveList[Game.TileNum], false);
        Game.TileNum++;
        drawTileNumber();
    }
    while( Game.TileNum > 0 )
    {
        // remove all tiles
        removeTile(Game.MoveList[Game.TileNum-1], false);
        removeTile(Game.MoveList[Game.TileNum-1]);
        drawTileNumber();
    }

    calculateNewGame();
}

// ---------------------------------------------------------
void BoardWidget::calculateNewGame( int gNumber)
{
    cancelUserSelectedTiles();
    initialiseRemovedTiles();
    setStatusText( i18n("Calculating new game...") );


    if( !loadBoard())
    {
        setStatusText( i18n("Error converting board information!") );
        return;
    }

    if (gNumber == -1) {
    	gameGenerationNum = 0;
    } else {
	gameGenerationNum = gNumber;
    }

    random.setSeed(gameGenerationNum);

    // try max. 64 times
    for( short nr=0; nr<64; nr++ )
    {
        if( generateStartPosition2() )
        {
            drawBoard();
            setStatusText( i18n("Ready. Now it's your turn.") );
			cheatsUsed=0;
	    return;
        }
    }

    drawBoard();
    setStatusText( i18n("Error generating new game!") );
}


bool BoardWidget::generateStartPosition2() {
	int totalTiles=0;
	POSITION tilesLeft[BoardLayout::maxTiles];

	memset(tilesLeft, 0, sizeof(tilesLeft));

	// zero out all face values	
	for (int z=0; z< BoardLayout::depth; z++) {
	    for (int y=0; y<BoardLayout::height; y++) {
		for (int x=0; x<BoardLayout::width; x++) {
			Game.Board[z][y][x]=0;
			if (Game.Mask[z][y][x] == '1') {
				tilesLeft[totalTiles].x = x;		
				tilesLeft[totalTiles].y = y;		
				tilesLeft[totalTiles].e = z;
				tilesLeft[totalTiles].f = 254;		
				totalTiles++;
			}
		}
	    }
	}

	// Initialise the faces to allocate. For the classic
	// dragon board there are 144 tiles. So we allocate and
	// randomise the assignment of 144 tiles. If there are > 144
	// tiles we will reallocate and re-randomise as we run out.
	// One advantage of this method is that the pairs to assign are
	// non-linear. In kmahjongg 0.4, If there were > 144 the same
	// allocation series was followed. So 154 = 144 + 10 rods.
	// 184 = 144 + 40 rods (20 pairs) which overwhemed the board
	// with rods and made deadlock games more likely.


	int remaining = totalTiles;
	randomiseFaces();

	for (int tile=0; tile <totalTiles; tile+=2) {
		int p1;
		int p2;




		if (remaining > 2) {
			p2 = p1 = random.getLong(remaining-2);
			int bail = 0;	
			while (p1 == p2) {
				p2 = random.getLong(remaining-2);

				if (bail >= 100) {
					printf("Bail!\n");
					break;
				}
				if ((tilesLeft[p1].y == tilesLeft[p2].y) &&
				    (tilesLeft[p1].e == tilesLeft[p2].e)) {
					// skip if on same y line
					bail++;
					p2=p1;
					continue;
				}
			}
		} else {
			p1 = 0;
			p2 = 1;
		}
		POSITION a, b;
		a = tilesLeft[p1];
		b = tilesLeft[p2];
		tilesLeft[p1] = tilesLeft[remaining - 1];
		tilesLeft[p2] = tilesLeft[remaining - 2];
		remaining -= 2;	


		getFaces(a, b);
		Game.putTile(a);
		Game.putTile(b);
	}

    Game.TileNum = Game.MaxTileNum;
	return 1;	
}

void BoardWidget::getFaces(POSITION &a, POSITION &b) {
	a.f = tilePair[tilesUsed];
	b.f = tilePair[tilesUsed+1];
	tilesUsed += 2;

	if (tilesUsed >= 144) {
		randomiseFaces();
	}	
}

void BoardWidget::randomiseFaces(void) {
	int nr;
	int numAlloced=0;
	// stick in 144 tiles in pairsa.

        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_CHARACTER+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_BAMBOO+(nr/4); // 4*9 Tiles
        for( nr=0; nr<9*4; nr++)
		tilePair[numAlloced++] = TILE_ROD+(nr/4); // 4*9 Tiles
        for( nr=0; nr<4;   nr++)
		tilePair[numAlloced++] = TILE_FLOWER+nr;         // 4 Tiles
        for( nr=0; nr<4;   nr++)
		tilePair[numAlloced++] = TILE_SEASON+nr;         // 4 Tiles
        for( nr=0; nr<4*4; nr++)
		tilePair[numAlloced++] = TILE_WIND+(nr/4);  // 4*4 Tiles
        for( nr=0; nr<3*4; nr++)
		tilePair[numAlloced++] = TILE_DRAGON+(nr/4);     // 3*4 Tiles


	//randomise. Keep pairs together. Ie take two random
	//odd numbers (n,x) and swap n, n+1 with x, x+1

	int at=0;
	int to=0;
	for (int r=0; r<200; r++) {


		to=at;
		while (to==at) {
			to = random.getLong(144);

			if ((to & 1) != 0)
				to--;

		}
		UCHAR tmp = tilePair[at];
		tilePair[at] = tilePair[to];
		tilePair[to] = tmp;
		tmp = tilePair[at+1];
		tilePair[at+1] = tilePair[to+1];
		tilePair[to+1] = tmp;


		at+=2;
		if (at >= 144)
			at =0;
	}

	tilesAllocated = numAlloced;
	tilesUsed = 0;

/*
	printf("Allocated %d tiles\n",numAlloced);
	for (int t=0; t<numAlloced; t++) {
		printf("%2.2X ", tilePair[t]);
		if ((t % 20) == 19)
			printf("\n");
	}

	printf("\n");
*/

}


// ---------------------------------------------------------
bool isFlower( UCHAR Tile )
{
    return( Tile >= TILE_FLOWER  &&  Tile <=TILE_FLOWER+3 );
}
bool isSeason( UCHAR Tile )
{
    return( Tile >= TILE_SEASON  &&  Tile <=TILE_SEASON+3 );
}
bool isBamboo(UCHAR t) {
    return( t >= TILE_BAMBOO && t <TILE_BAMBOO+9);
}
bool isCharacter(UCHAR t) {
    return( t >= TILE_CHARACTER && t <TILE_CHARACTER + 9);
}
bool isRod(UCHAR t) {
    return( t >= TILE_ROD && t <TILE_ROD + 9);
}
bool isDragon(UCHAR t) {
    return( t >= TILE_DRAGON && t < TILE_DRAGON +3);
}
bool isWind(UCHAR t) {
    return( t >= TILE_WIND && t < TILE_WIND +4);
}


bool BoardWidget::isMatchingTile( POSITION& Pos1, POSITION& Pos2 )
{
    // don't compare 'equal' positions
    if( memcmp( &Pos1, &Pos2, sizeof(POSITION) ) )
    {
        UCHAR FA = Pos1.f;
        UCHAR FB = Pos2.f;

        if( (FA == FB)
         || ( isFlower( FA ) && isFlower( FB ) )
         || ( isSeason( FA ) && isSeason( FB ) ) )
            return( true );
    }
    return( false );
}

// ---------------------------------------------------------
bool BoardWidget::findMove( POSITION& posA, POSITION& posB )
{
    short Pos_Ende = Game.MaxTileNum;  // Ende der PosTable

    for( short E=0; E<BoardLayout::depth; E++ )
    {
        for( short Y=0; Y<BoardLayout::height-1; Y++ )
        {
            for( short X=0; X<BoardLayout::width-1; X++ )
            {
                if( Game.Mask[E][Y][X] != (UCHAR) '1' )
                    continue;
                if( ! Game.Board[E][Y][X] )
                    continue;
                if( E < 4 )
                {
                    if( Game.Board[E+1][Y][X] || Game.Board[E+1][Y+1][X] ||
                        Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                        continue;
                }
                if( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
                    (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
                    continue;

                Pos_Ende--;
                PosTable[Pos_Ende].e = E;
                PosTable[Pos_Ende].y = Y;
                PosTable[Pos_Ende].x = X;
                PosTable[Pos_Ende].f = Game.Board[E][Y][X];




            }
        }
    }

 //   PosTable[0].e = BoardLayout::depth;  // 1. Paar noch nicht gefunden
    iPosCount = 0;  // Hier Anzahl der gefunden Paare merken

	
    // The new tile layout with non-contiguos horizantle spans
    // can lead to huge numbers of matching pairs being exposed.
    // we alter the loop to bail out when BoardLayout::maxTiles/2 pairs are found
    // (or less);
    while( Pos_Ende < Game.MaxTileNum-1 && iPosCount <BoardLayout::maxTiles-2)
    {
        for( short Pos = Pos_Ende+1; Pos < Game.MaxTileNum; Pos++)
        {
            if( isMatchingTile(PosTable[Pos], PosTable[Pos_Ende]) )
            {
		if (iPosCount <BoardLayout::maxTiles-2) {
                	PosTable[iPosCount++] = PosTable[Pos_Ende];
                	PosTable[iPosCount++] = PosTable[Pos];
		}
            }
        }
        Pos_Ende++;
    }

    if( iPosCount>=2 )
    {
        random.setSeed(0); // WABA: Why is the seed reset?
        short Pos = random.getLong(iPosCount) & -2;  // Gerader Wert
        posA = PosTable[Pos];
        posB = PosTable[Pos+1];

        return( true );
    }
    else
        return( false );
}

// ---------------------------------------------------------
short BoardWidget::findAllMatchingTiles( POSITION& posA )
{
    short Pos = 0;

    for( short E=0; E<BoardLayout::depth; E++ )
    {
        for( short Y=1; Y<BoardLayout::height-1; Y++ )
        {
            for( short X=1; X<BoardLayout::width-1; X++ )
            {
                if( Game.Mask[E][Y][X] != (UCHAR) '1' )
                    continue;
                if( ! Game.Board[E][Y][X] )
                    continue;
                if( E < 4 )
                {
                    if( Game.Board[E+1][Y][X] || Game.Board[E+1][Y+1][X] ||
                        Game.Board[E+1][Y][X+1] || Game.Board[E+1][Y+1][X+1] )
                        continue;
                }
                if( (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1]) &&
                    (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2]) )
                    continue;

                PosTable[Pos].e = E;
                PosTable[Pos].y = Y;
                PosTable[Pos].x = X;
                PosTable[Pos].f = Game.Board[E][Y][X];

                if( isMatchingTile(posA, PosTable[Pos]) )
                    Pos++;
            }
        }
    }
    return Pos;
}



// ---------------------------------------------------------
// This function replaces the old method of hilighting by
// modifying color 21 to color 20. This was single tileset
// specific. We now have two tile faces, one selected one not.

void BoardWidget::hilightTile( POSITION& Pos, bool on, bool doRepaint )
{

	if (on) {
		Game.hilighted[Pos.e][Pos.y][Pos.x]=1;
	} else {
		Game.hilighted[Pos.e][Pos.y][Pos.x]=0;
	}
	if (doRepaint) {
		updateBackBuffer=true;
		repaint(0,0,-1,-1, false);
	}
}



// ---------------------------------------------------------
void BoardWidget::drawBoard(bool )
{

   updateBackBuffer=true;
   repaint(0,0,-1,-1,false);
   drawTileNumber();
}

// ---------------------------------------------------------
void BoardWidget::putTile( POSITION& Pos, bool doRepaint )
{
    short E=Pos.e;
    short Y=Pos.y;
    short X=Pos.x;

	// we ensure that any tile we put on has highlighting off
    Game.putTile( E, Y, X, Pos.f );
	Game.hilighted[E][Y][X] = 0;
    if (doRepaint) {
	updateBackBuffer=true;
       repaint(0,0,-1,-1, false);
    }
}


// ---------------------------------------------------------
void BoardWidget::removeTile( POSITION& Pos , bool doRepaint)
{

    short E = Pos.e;
    short Y = Pos.y;
    short X = Pos.x;

    Game.TileNum--;                    // Eine Figur weniger
    Game.MoveList[Game.TileNum] = Pos; // Position ins Protokoll eintragen



    // remove tile from game board
    Game.putTile( E, Y, X, 0 );
    if (doRepaint) {
	updateBackBuffer=true;
    	repaint(0,0,-1, -1, false);
    }
}

// ---------------------------------------------------------
void BoardWidget::mousePressEvent ( QMouseEvent* event )
{
    if (gamePaused)
	return;

    if( event->button() == LeftButton )
    {
        if( TimerState == Demo )
	{
            stopDemoMode();
        }
        else if( showMatch )
	{
            stopMatchAnimation();
        }

        if( MouseClickPos1.e == BoardLayout::depth )       // first tile
	{
            transformPointToPosition( event->pos(), MouseClickPos1 );



            if( MouseClickPos1.e != BoardLayout::depth && showMatch )
            {
                matchCount = findAllMatchingTiles( MouseClickPos1 );
                TimerState = Match;
                iTimerStep = 1;
                matchAnimationTimeout();
				cheatsUsed++;
            }
        }
        else                                // second tile
        {
            transformPointToPosition( event->pos(), MouseClickPos2 );
            if( MouseClickPos2.e == BoardLayout::depth )
            {
                cancelUserSelectedTiles();
            }
            else
            {
                if( isMatchingTile( MouseClickPos1, MouseClickPos2 ) )
                {
		    // update the removed tiles (we do this before the remove below
		    // so that we only require 1 screen paint for both actions)
		    setRemovedTilePair(MouseClickPos1, MouseClickPos2);

		    // now we remove the tiles from the board
                    removeTile(MouseClickPos1, false);
                    removeTile(MouseClickPos2);

		    // removing a tile means redo is impossible without
		    // a further undo.
		    Game.allow_redo=false;
		    demoModeChanged(false);
                    drawTileNumber();

                    // if no tiles are left, the player has `won`, so celebrate
                    if( Game.TileNum == 0 )
                    {
						
                        KMessageBox::information(this, i18n("Game over: You have won!"));
                        animateMoveList();
			gameOver(Game.MaxTileNum,cheatsUsed);
                    }
                    // else if no more moves are possible, display the sour grapes dialog
                    else if( ! findMove( TimerPos1, TimerPos2 ) )
                    {
                        KMessageBox::information(this, i18n("Game over: You have no moves left"));
                    }
                }
                else
                {
                    // redraw tiles in normal state
                    hilightTile( MouseClickPos1, false, false );
                    hilightTile( MouseClickPos2, false );
                }
                MouseClickPos1.e = BoardLayout::depth;     // mark tile position as invalid
                MouseClickPos2.e = BoardLayout::depth;
            }
        }
    }
}


// ----------------------------------------------------------
/**
    Transform window point to board position.

    @param  point          Input: Point in window coordinates
    @param  MouseClickPos  Output: Position in game board
*/
void BoardWidget::transformPointToPosition(
        const QPoint& point,
        POSITION&     MouseClickPos
    )
{
    short E,X,Y;

    // iterate over E coordinate from top to bottom
    for( E=BoardLayout::depth-1; E>=0; E-- )
    {
        // calculate mouse coordiantes --> position in game board
	// the factor -theTiles.width()/2 must keep track with the
	// offset for blitting in the print Event (FIX ME)
        X = ((point.x()-theTiles.width()/2)- (E+1)*theTiles.shadowSize()) / theTiles.qWidth();
        Y = ((point.y()-theTiles.height()/2) + E*theTiles.shadowSize()) / theTiles.qHeight();


	// changed to allow x == 0
        // skip when position is illegal
        if (X<0 || X>=BoardLayout::width || Y<0 || Y>=BoardLayout::height)
		continue;

        //
        switch( Game.Mask[E][Y][X] )
        {
            case (UCHAR)'3':    X--;Y--;
                                break;

            case (UCHAR)'2':    X--;
                                break;

            case (UCHAR)'4':    Y--;
                                break;

            case (UCHAR)'1':    break;

            default :           continue;
        }
        // if gameboard is empty, skip
        if ( ! Game.Board[E][Y][X] ) continue;
        // tile must be 'free' (nothing left, right or above it)
        if( E < 4 )
        {
            if( Game.Board[E+1][Y][X]   || Game.Board[E+1][Y+1][X] ||
                (X<BoardLayout::width-2 && Game.Board[E+1][Y][X+1]) ||
	        (X<BoardLayout::width-2 && Game.Board[E+1][Y+1][X+1]) )
                continue;
        }

	// No left test on left edge
        if (( X > 0) && (Game.Board[E][Y][X-1] || Game.Board[E][Y+1][X-1])) {
		if ((X<BoardLayout::width-2) && (Game.Board[E][Y][X+2] || Game.Board[E][Y+1][X+2])) {
	

            	continue;
		}
	}

        // here, position is legal
        MouseClickPos.e = E;
        MouseClickPos.y = Y;
        MouseClickPos.x = X;
        MouseClickPos.f = Game.Board[E][Y][X];
        // give visible feedback
        hilightTile( MouseClickPos );
        break;
    }
}

// ---------------------------------------------------------
bool BoardWidget::loadBoard( )
{
    GAMEDATA newGame;

    memset( &newGame, 0, sizeof( newGame ) );
    theBoardLayout.copyBoardLayout((UCHAR *) newGame.Mask, newGame.MaxTileNum);
    Game = newGame;
    return(true);
}

// ---------------------------------------------------------
void BoardWidget::setStatusText( const QString & pszText )
{
    emit statusTextChanged( pszText, gameGenerationNum );
}



// ---------------------------------------------------------
bool BoardWidget::loadBackground(
        const QString& pszFileName,
        bool        bShowError
    )
{
    if( ! theBackground.load( pszFileName, requiredWidth(), requiredHeight()) )
    {
        if( bShowError )
            KMessageBox::sorry(this, i18n("Failed to load image:\n%1").arg(pszFileName) );
        return( false );
    }
    preferences.setBackground(pszFileName);
    return( true );
}

// ---------------------------------------------------------
void BoardWidget::drawTileNumber()
{
    emit tileNumberChanged( Game.MaxTileNum, Game.TileNum );
}

// ---------------------------------------------------------
void BoardWidget::cancelUserSelectedTiles()
{
    if( MouseClickPos1.e != BoardLayout::depth )
    {
        hilightTile( MouseClickPos1, false ); // redraw tile
        MouseClickPos1.e = BoardLayout::depth;    // mark tile invalid
    }
}

// ---------------------------------------------------------

void BoardWidget::setRemovedTilePair(POSITION &a, POSITION &b) {

	if (isFlower(a.f)) {
		removedFlower[a.f-TILE_FLOWER]++;
		removedFlower[b.f-TILE_FLOWER]++;
		return;
	}

	if (isSeason(a.f)) {
		removedSeason[a.f-TILE_SEASON]++;
		removedSeason[b.f-TILE_SEASON]++;
		return;
	}
	if (isCharacter(a.f)) {
		removedCharacter[a.f - TILE_CHARACTER]+=2;
		return;
	}

	if (isBamboo(a.f)) {
		removedBamboo[a.f - TILE_BAMBOO]+=2;
		return;
	}
	if (isRod(a.f)) {
		removedRod[a.f - TILE_ROD]+=2;
		return;
	}
	if (isDragon(a.f)){
		removedDragon[a.f - TILE_DRAGON]+=2;
		return;
	}
	if (isWind(a.f)){
		removedWind[a.f - TILE_WIND]+=2;
		return;
	}
}

void BoardWidget::clearRemovedTilePair(POSITION &a, POSITION &b) {

        if (isFlower(a.f)) {
                removedFlower[a.f-TILE_FLOWER]--;
                removedFlower[b.f-TILE_FLOWER]--;
                return;
        }

        if (isSeason(a.f)) {
                removedSeason[a.f-TILE_SEASON]--;
                removedSeason[b.f-TILE_SEASON]--;
                return;
        }
        if (isCharacter(a.f)) {
                removedCharacter[a.f - TILE_CHARACTER]-=2;
                return;
        }

        if (isBamboo(a.f)) {
                removedBamboo[a.f - TILE_BAMBOO]-=2;
                return;
        }
        if (isRod(a.f)){
                removedRod[a.f - TILE_ROD]-=2;
                return;
        }
        if (isDragon(a.f)){
                removedDragon[a.f - TILE_DRAGON]-=2;
                return;
        }
        if (isWind(a.f)){
                removedWind[a.f - TILE_WIND]-=2;
                return;
        }
}


void BoardWidget::initialiseRemovedTiles(void) {
	for (int pos=0; pos<9; pos++) {
		removedCharacter[pos]=0;
		removedBamboo[pos]=0;
		removedRod[pos]=0;
		removedDragon[pos %3] = 0;
		removedFlower[pos % 4] = 0;
		removedWind[pos % 4] = 0;
		removedSeason[pos % 4] = 0;
		
	}

}

// ---------------------------------------------------------
void BoardWidget::showMessage( const QString& pszText )
{
    emit message( pszText );
}



bool BoardWidget::loadTileset(const QString &path) {

  if (theTiles.loadTileset(path)) {
    preferences.setTileset(path);
    return(true);
  } else {
    return(false);
  }

}

bool BoardWidget::loadBoardLayout(const QString &file) {
  if (theBoardLayout.loadBoardLayout(file)) {
	preferences.setLayout(file);
	return true;
  } else {
	return false;
  }
}

void BoardWidget::updateScaleMode(void) {


	theBackground.scaleModeChanged();
}



// calculate the required window width (board + removed tiles)
int BoardWidget::requiredWidth(void) {
	int res = ((BoardLayout::width+12)* theTiles.qWidth());

	return(res);
}

// calculate the required window height (board + removed tiles)
int BoardWidget::requiredHeight(void) {

	int res = ((BoardLayout::height+3)* theTiles.qHeight());
	return(res);
}

void BoardWidget::tileSizeChanged(void) {
	theTiles.setScaled(preferences.miniTiles());
	theBackground.sizeChanged(requiredWidth(), requiredHeight());

}

// shuffle the remaining tiles around, useful if a deadlock ocurrs
// this is a big cheat so we penalise the user.
void BoardWidget::shuffle(void) {
	int count = 0;
	// copy positions and faces of the remaining tiles into
	// the pos table
	for (int e=0; e<BoardLayout::depth; e++) {
	    for (int y=0; y<BoardLayout::height; y++) {
		for (int x=0; x<BoardLayout::width; x++) {
		    if (Game.Board[e][y][x] && Game.Mask[e][y][x] == '1') {
			PosTable[count].e = e;
			PosTable[count].y = y;
			PosTable[count].x = x;
			PosTable[count].f = Game.Board[e][y][x];
			count++;
		    }
		}
	    }

	}


	// now lets randomise the faces, selecting 400 pairs at random and
	// swapping the faces.
	for (int ran=0; ran < 400; ran++) {
		int pos1 = random.getLong(count);
		int pos2 = random.getLong(count);
		if (pos1 == pos2)
			continue;
		BYTE f = PosTable[pos1].f;
		PosTable[pos1].f = PosTable[pos2].f;
		PosTable[pos2].f = f;
	}

	// put the rearranged tiles back.
	for (int p=0; p<count; p++)
		Game.putTile(PosTable[p]);


	// force a redraw
	
	updateBackBuffer=true;
       repaint(0,0,-1,-1, false);


	// I consider this s very bad cheat so, I punish the user
	// 300 points per use
	cheatsUsed += 15;
}






