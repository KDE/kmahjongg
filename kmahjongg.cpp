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

#include "kmahjongg.h"

#include "prefs.h"
#include "kmahjongglayoutselector.h"
#include "ui_settings.h"
#include "Editor.h"
#include "GameView.h"
#include "GameScene.h"
#include "GameData.h"
#include "kmahjongglayout.h"

#include <kmahjonggconfigdialog.h>

#include <limits.h>

#include <QPixmapCache>
#include <QLabel>
#include <QDesktopWidget>

#include <KAboutData>
#include <KAction>
#include <KConfigDialog>
#include <KInputDialog>
#include <KMenuBar>
#include <KMessageBox>
#include <KStandardGameAction>
#include <KStandardAction>
#include <KIcon>
#include <KScoreDialog>
#include <KGameClock>

#include <kio/netaccess.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>


#define ID_STATUS_TILENUMBER 1
#define ID_STATUS_MESSAGE    2
#define ID_STATUS_GAME       3


static const char *gameMagic = "kmahjongg-gamedata";
static int gameDataVersion = 1;

/**
 * This class implements
 *
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com> */
class Settings : public QWidget, public Ui::Settings
{
public:
    /**
     * Constructor */
    Settings(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

KMahjongg::KMahjongg(QWidget *parent)
    : KXmlGuiWindow(parent),
    m_bPaused(false),
    m_pGameView(NULL),
    m_pGameData(NULL),
    m_pBoardLayout(new KMahjonggLayout())
{
    //Use up to 3MB for global application pixmap cache
    QPixmapCache::setCacheLimit(3 * 1024);

    // minimum area required to display the field
    setMinimumSize(320, 320);

    // init board widget
    m_pGameScene = new GameScene();

    // load the layout
    loadLayout();

    // init game data
    m_pGameData = new GameData(m_pBoardLayout->board());

    // init view and add to window
    m_pGameView = new GameView(m_pGameScene, m_pGameData, this);
    setCentralWidget(m_pGameView);

    // Initialize boardEditor
    boardEditor = new Editor();
    boardEditor->setModal(false);

    // Set the tileset setted in the the settings.
    boardEditor->setTilesetFromSettings();

    setupStatusBar();
    setupKAction();

    gameTimer = new KGameClock(this);

    connect(gameTimer, SIGNAL(timeChanged(QString)), this, SLOT(displayTime(QString)));
    connect(m_pGameView, SIGNAL(statusTextChanged(QString, long)),
                SLOT(showStatusText(QString, long)));
    connect(m_pGameView, SIGNAL(itemNumberChanged(int, int, int)),
                SLOT(showItemNumber(int, int, int)));
    connect(m_pGameView, SIGNAL(gameOver(unsigned short, unsigned short)), this,
                SLOT(gameOver(unsigned short, unsigned short)));
    connect(m_pGameView, SIGNAL(demoOrMoveListAnimationOver(bool)),
            this, SLOT(demoOrMoveListAnimationOver(bool)));
    connect(m_pGameScene, SIGNAL(rotateCW()), m_pGameView, SLOT(angleSwitchCW()));
    connect(m_pGameScene, SIGNAL(rotateCCW()), m_pGameView, SLOT(angleSwitchCCW()));


    mFinished = false;
    bDemoModeActive = false;

    loadSettings();

    startNewGame();
}

KMahjongg::~KMahjongg()
{
    delete m_pGameView;
    delete m_pGameScene;
    delete m_pBoardLayout;
    delete boardEditor;
    delete m_pGameData;
}

void KMahjongg::setupKAction()
{
    KStandardGameAction::gameNew(this, SLOT(newGame()), actionCollection());
    KStandardGameAction::load(this, SLOT(loadGame()), actionCollection());
    KStandardGameAction::save(this, SLOT(saveGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardGameAction::restart(this, SLOT(restartGame()), actionCollection());

    QAction *newNumGame = actionCollection()->addAction(QLatin1String("game_new_numeric"));
    newNumGame->setText(i18n("New Numbered Game..."));
    connect(newNumGame, SIGNAL(triggered(bool)), SLOT(startNewNumeric()));

    QAction *action = KStandardGameAction::hint(m_pGameView, SLOT(helpMove()), this);
    actionCollection()->addAction(action->objectName(), action);

    QAction *shuffle = actionCollection()->addAction(QLatin1String("move_shuffle"));
    shuffle->setText(i18n("Shu&ffle"));
    shuffle->setIcon(KIcon(QLatin1String("view-refresh")));
    connect(shuffle, SIGNAL(triggered(bool)), m_pGameView, SLOT(shuffle()));

    KAction *angleccw = actionCollection()->addAction(QLatin1String("view_angleccw"));
    angleccw->setText(i18n("Rotate View Counterclockwise"));
    angleccw->setIcon(KIcon(QLatin1String("object-rotate-left")));
    angleccw->setShortcuts(KShortcut("f"));
    connect(angleccw, SIGNAL(triggered(bool)), m_pGameView, SLOT(angleSwitchCCW()));

    KAction *anglecw = actionCollection()->addAction(QLatin1String("view_anglecw"));
    anglecw->setText(i18n("Rotate View Clockwise"));
    anglecw->setIcon(KIcon(QLatin1String("object-rotate-right")));
    anglecw->setShortcuts(KShortcut("g"));
    connect(anglecw, SIGNAL(triggered(bool)), m_pGameView, SLOT(angleSwitchCW()));

    demoAction = KStandardGameAction::demo(this, SLOT(demoMode()), actionCollection());

    KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    pauseAction = KStandardGameAction::pause(this, SLOT(pause()), actionCollection());

    // move
    undoAction = KStandardGameAction::undo(this, SLOT(undo()), actionCollection());
    redoAction = KStandardGameAction::redo(this, SLOT(redo()), actionCollection());

    // edit
    QAction *boardEdit = actionCollection()->addAction(QLatin1String("game_board_editor"));
    boardEdit->setText(i18n("&Board Editor"));
    connect(boardEdit, SIGNAL(triggered(bool)), SLOT(slotBoardEditor()));

    // settings
    KStandardAction::preferences(this, SLOT(showSettings()), actionCollection());
    setupGUI(qApp->desktop()->availableGeometry().size() * 0.7);
}

void KMahjongg::setupStatusBar()
{
    gameTimerLabel = new QLabel(i18n("Time: 0:00:00"), statusBar());
    statusBar()->addWidget(gameTimerLabel);

    QFrame *timerDivider = new QFrame(statusBar());
    timerDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(timerDivider);

    tilesLeftLabel = new QLabel(i18n("Removed: 0000/0000"), statusBar());
    statusBar()->addWidget(tilesLeftLabel, 1);

    QFrame *tileDivider = new QFrame(statusBar());
    tileDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(tileDivider);

    gameNumLabel = new QLabel(i18n("Game: 000000000000000000000"), statusBar());
    statusBar()->addWidget(gameNumLabel);

    QFrame *gameNumDivider = new QFrame(statusBar());
    gameNumDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(gameNumDivider);

    statusLabel = new QLabel("Kmahjongg", statusBar());
    statusBar()->addWidget(statusLabel);
}

void KMahjongg::displayTime(const QString& timestring)
{
    gameTimerLabel->setText(i18n("Time: ") + timestring);
}

void KMahjongg::startNewNumeric()
{
    bool ok;
    int s = KInputDialog::getInteger(i18n("New Game"), i18n("Enter game number:"), 0, 0, INT_MAX, 1,
        &ok, this);

    if (ok) {
        startNewGame(s);
    }
}

void KMahjongg::undo()
{
    m_pGameView->undo();
    demoModeChanged(false);
}

void KMahjongg::redo()
{
    m_pGameView->redo();
    demoModeChanged(false);
}

void KMahjongg::showSettings()
{
    if (KConfigDialog::showDialog("settings")) {
        return;
    }

    //Use the classes exposed by LibKmahjongg for our configuration dialog
    KMahjonggConfigDialog *dialog = new KMahjonggConfigDialog(this, "settings", Prefs::self());

    //The Settings class is ours
    dialog->addPage(new Settings(0), i18n("General"), "games-config-options");
    dialog->addPage(new KMahjonggLayoutSelector(0, Prefs::self()), i18n("Board Layout"), "games-con"
        "fig-board");
    dialog->addTilesetPage();
    dialog->addBackgroundPage();
    dialog->setHelp(QString(),"kmahjongg");

    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(loadSettings()));
    connect(dialog, SIGNAL(settingsChanged(QString)), boardEditor, SLOT(setTilesetFromSettings()));

    dialog->show();
}

void KMahjongg::loadLayout()
{
    if (!m_pBoardLayout->load(Prefs::layout())) {
        kDebug() << "Error loading the layout. Try to load the default layout.";

        m_pBoardLayout->loadDefault();
    }
}

void KMahjongg::loadSettings()
{
    // Just load the new layout, if it is not already set.
    if (m_pBoardLayout->path() != Prefs::layout()) {
        loadLayout();

        delete m_pGameData;
        m_pGameData = new GameData(m_pBoardLayout->board());
        m_pGameView->setGameData(m_pGameData);

        startNewGame();
    }

    if (m_pGameView == NULL) {
        kDebug() << "GameView not initialised!";

        return;
    }

    // Just set the new tileset, if it is not already set.
    m_pGameView->setTilesetPath(Prefs::tileSet());
    if (m_pGameView->getTilesetPath() != Prefs::tileSet()) {
        if (!m_pGameView->setTilesetPath(Prefs::tileSet())) {
            kDebug() << "An error occurred when loading the tileset" << Prefs::tileSet() <<
                "KMahjongg will continue with the default tileset.";
        }
    }


    m_pGameView->setBackgroundPath(Prefs::background());

    // Set the showmatchingtiles option.
    m_pGameView->setMatch(Prefs::showMatchingTiles());
}

void KMahjongg::demoMode()
{
    if (demoAction->isChecked()) {
        demoModeChanged(true);
        gameTimer->setTime(0);
        gameTimer->pause();
        m_pGameView->startDemo();
    } else {
        demoModeChanged(false);
        startNewGame();
    }
}

void KMahjongg::pause()
{
    m_bPaused = !m_bPaused;

    if (m_bPaused) {
        gameTimer->pause();
    } else {
        gameTimer->resume();
    }

    m_pGameView->pause(m_bPaused);

    demoModeChanged(false);
}

void KMahjongg::showHighscores()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    ksdialog.setConfigGroup(m_pBoardLayout->authorProperty("Name"));
    ksdialog.exec();
}

void KMahjongg::slotBoardEditor()
{
    boardEditor->setVisible(true);

    // Set the default size.
    boardEditor->setGeometry(Prefs::editorGeometry());
}

void KMahjongg::newGame()
{
    startNewGame();
}

void KMahjongg::startNewGame(int item)
{
    if (!bDemoModeActive) {
        m_pGameView->createNewGame(item);

        timerReset();

        // update the initial enabled/disabled state for
        // the menu and the tool bar.
        mFinished = false;
        demoModeChanged(false);
    }
}

void KMahjongg::timerReset()
{
    // initialise the scoring system
    gameElapsedTime = 0;

    // start the game timer
    gameTimer->restart();
}

void KMahjongg::demoOrMoveListAnimationOver(bool bDemoGameLost)
{
    if (bDemoGameLost) {
        KMessageBox::information(this, i18n("Your computer has lost the game."));
    }

    demoModeChanged(false);
    startNewGame();
}

void KMahjongg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateEvent = (QWindowStateChangeEvent *) event;

        if ((isMinimized() && stateEvent->oldState() != Qt::WindowMinimized)
            || (!isMinimized() && stateEvent->oldState() == Qt::WindowMinimized)) {
            pause();
        }
    }
}

void KMahjongg::gameOver(unsigned short numRemoved, unsigned short cheats)
{
    int time;
    int score;

    gameTimer->pause();

    KMessageBox::information(this, i18n("You have won!"));

    mFinished = true;
    demoModeChanged(false);

    time = score = 0;

    // get the time in milli secs
    // subtract from 20 minutes to get bonus. if longer than 20 then ignore
    time = (60 * 20) - gameTimer->seconds();
    if (time < 0) {
        time =0;
    }
    // conv back to  secs (max bonus = 60*20 = 1200

    // points per removed tile bonus (for deragon max = 144*10 = 1440
    score += (numRemoved * 20);
    // time bonus one point per second under one hour
    score += time;
    // points per cheat penalty (max penalty = 1440 for dragon)
    score -= (cheats * 20);
    if (score < 0) {
        score = 0;
    }

    //TODO: add gameNum as a Custom KScoreDialog field?
    //int elapsed = gameTimer->seconds();
    //long gameNum = m_pGameView->getGameNumber();
    //theHighScores->checkHighScore(score, elapsed, gameNum, m_pGameView->getBoardName());
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    ksdialog.setConfigGroup(m_pBoardLayout->authorProperty("Name"));
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Score].setNum(score);
    scoreInfo[KScoreDialog::Time] = gameTimer->timeString();
    if(ksdialog.addScore(scoreInfo, KScoreDialog::AskName)) {
        ksdialog.exec();
    }

    m_pGameView->startMoveListAnimation();
}


void KMahjongg::showStatusText(const QString &msg, long board)
{
    statusLabel->setText(msg);
    QString str = i18n("Game number: %1", board);
    gameNumLabel->setText(str);
}

void KMahjongg::showItemNumber(int iMaximum, int iCurrent, int iLeft)
{
    QString szBuffer = i18n("Removed: %1/%2  Combinations left: %3", iMaximum - iCurrent, iMaximum,
        iLeft);
    tilesLeftLabel->setText(szBuffer);

    // update undo menu item, if demomode is inactive
    if (!m_bPaused && !mFinished) {
        undoAction->setEnabled(m_pGameView->checkUndoAllowed());
        redoAction->setEnabled(m_pGameView->checkRedoAllowed());
    }
}

void KMahjongg::demoModeChanged(bool bActive)
{
    bDemoModeActive = bActive;

    pauseAction->setChecked(m_bPaused);
    demoAction->setChecked(bActive || m_bPaused);

    if (m_bPaused) {
        stateChanged("paused");
    } else if (mFinished) {
        stateChanged("finished");
    } else if (bActive) {
        stateChanged("active");
    } else {
        stateChanged("inactive");
        undoAction->setEnabled(m_pGameView->checkUndoAllowed());
        redoAction->setEnabled(m_pGameView->checkRedoAllowed());
    }
}

void KMahjongg::restartGame()
{
    if (!bDemoModeActive) {
        m_pGameView->createNewGame(m_pGameView->getGameNumber());

        timerReset();

        // update the initial enabled/disabled state for
        // the menu and the tool bar.
        mFinished = false;
        demoModeChanged(false);

        if (m_bPaused) {
            pauseAction->setChecked(false);
            pause();
        }
    }
}

void KMahjongg::loadGame()
{
    QString fname;

    // Get the name of the file to load
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*.kmgame", this, i18n("Load Game" ));

    if (url.isEmpty()) {
        return;
    }

    KIO::NetAccess::download(url, fname, this);

    // open the file for reading
    QFile infile(fname);

    if (!infile.open(QIODevice::ReadOnly)) {
        KMessageBox::sorry(this, i18n("Could not read from file. Aborting."));
        return;
    }

    QDataStream in(&infile);

    // verify the magic
    QString magic;
    in >> magic;

    if (QString::compare(magic, gameMagic, Qt::CaseSensitive) != 0) {
        KMessageBox::sorry(this, i18n("File is not a KMahjongg game."));
        infile.close();

        return;
    }

    // Read the version
    qint32 version;
    in >> version;

    if (version == gameDataVersion) {
        in.setVersion(QDataStream::Qt_4_0);
    } else {
        KMessageBox::sorry(this, i18n("File format not recognized."));
        infile.close();

        return;
    }

    QString theTilesName;
    QString theBackgroundName;
    QString theBoardLayoutName;
    in >> theTilesName;
    m_pGameView->setTilesetPath(theTilesName);
    in >> theBackgroundName;
    m_pGameView->setBackgroundPath(theBackgroundName);
    in >> theBoardLayoutName;

    //GameTime
    uint seconds;
    in >> seconds;
    gameTimer->setTime(seconds);

    // Load the boardlayout.
    if (m_pBoardLayout->path() != Prefs::layout()) {
        if (!m_pBoardLayout->load(Prefs::layout())) {
            kDebug() << "Error loading the layout. Try to load the default layout.";

            m_pBoardLayout->loadDefault();
        }
    }

    GameData *pGameDataOld = m_pGameData;
    m_pGameData = new GameData(m_pBoardLayout->board());
    m_pGameData->loadFromStream(in);
    m_pGameView->setGameData(m_pGameData);

    delete pGameDataOld;

    infile.close();

    KIO::NetAccess::removeTempFile(fname);

    mFinished = false;
    demoModeChanged(false);
}

void KMahjongg::saveGame()
{
    //Pause timer
    gameTimer->pause();

    // Get the name of the file to save
    KUrl url = KFileDialog::getSaveUrl(KUrl(), "*.kmgame", this, i18n("Save Game"));

    if (url.isEmpty()) {
        gameTimer->resume();

        return;
    }

    if (!url.isLocalFile()) {
        KMessageBox::sorry(this, i18n("Only saving to local files currently supported."));
        gameTimer->resume();

        return;
    }

    QFile outfile(url.path());

    if (!outfile.open(QIODevice::WriteOnly)) {
        KMessageBox::sorry(this, i18n("Could not write saved game."));
        gameTimer->resume();

        return;
    }

    QDataStream out(&outfile);

    // Write a header with a "magic number" and a version
    out << QString(gameMagic);
    out << (qint32) gameDataVersion;
    out.setVersion(QDataStream::Qt_4_0);

    out << m_pGameView->getTilesetPath();
    out << m_pGameView->getBackgroundPath();
    out << m_pBoardLayout->path();

    //GameTime
    out << gameTimer->seconds();

    // Write the Game data
    m_pGameData->saveToStream(out);

    outfile.close();
    gameTimer->resume();
}


#include "kmahjongg.moc"
