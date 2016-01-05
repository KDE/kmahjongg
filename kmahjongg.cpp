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
#include "kmahjongg_debug.h"
#include <kmahjonggconfigdialog.h>

#include <KAboutData>
#include <KActionCollection>
#include <KConfigDialog>
#include <KGameClock>
#include <KLocalizedString>
#include <KMessageBox>
#include <KScoreDialog>
#include <KStandardAction>
#include <KStandardGameAction>
#include <KToggleAction>

#include <QAction>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QIcon>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QPixmapCache>
#include <QShortcut>
#include <QStatusBar>
#include <QWindowStateChangeEvent>

#include <limits.h>

const QString KMahjongg::gameMagic = "kmahjongg-gamedata";
const int KMahjongg::gameDataVersion = 1;

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Settings : public QWidget, public Ui::Settings
{
public:
    Settings(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

KMahjongg::KMahjongg(QWidget *parent)
    : KXmlGuiWindow(parent),
    m_gameState(GameState::Gameplay),
    m_pGameView(nullptr),
    m_pGameData(nullptr),
    m_pBoardLayout(new KMahjonggLayout())
{
    //Use up to 3MB for global application pixmap cache
    QPixmapCache::setCacheLimit(3 * 1024);

    // minimum area required to display the field
    setMinimumSize(320, 320);

    // init board widget
    m_pGameScene = new GameScene();

    loadLayout();

    // init game data
    m_pGameData = new GameData(m_pBoardLayout->board());

    // init view and add to window
    m_pGameView = new GameView(m_pGameScene, m_pGameData, this);
    setCentralWidget(m_pGameView);

    boardEditor = new Editor();
    boardEditor->setModal(false);

    setupStatusBar();
    setupKAction();

    gameTimer = new KGameClock(this);

    connect(gameTimer, &KGameClock::timeChanged, this, &KMahjongg::displayTime);
    connect(m_pGameView, &GameView::statusTextChanged, this, &KMahjongg::showStatusText);
    connect(m_pGameView, &GameView::itemNumberChanged, this, &KMahjongg::showItemNumber);
    connect(m_pGameView, &GameView::gameOver, this, &KMahjongg::gameOver);
    connect(m_pGameView, &GameView::demoOrMoveListAnimationOver, this, &KMahjongg::demoOrMoveListAnimationOver);
    connect(m_pGameView, &GameView::noMovesAvailable, this, &KMahjongg::noMovesAvailable);
    connect(m_pGameScene, &GameScene::rotateCW, m_pGameView, &GameView::angleSwitchCW);
    connect(m_pGameScene, &GameScene::rotateCCW, m_pGameView, &GameView::angleSwitchCCW);

    m_bLastRandomSetting = Prefs::randomLayout();

    loadSettings();

    boardEditor->setTilesetFromSettings();

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
    KStandardGameAction::gameNew(this, SLOT(startNewGame()), actionCollection());
    KStandardGameAction::load(this, SLOT(loadGame()), actionCollection());
    KStandardGameAction::save(this, SLOT(saveGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    KStandardGameAction::restart(this, SLOT(restartGame()), actionCollection());

    QAction *newNumGame = actionCollection()->addAction(QStringLiteral("game_new_numeric"));
    newNumGame->setText(i18n("New Numbered Game..."));
    connect(newNumGame, &QAction::triggered, this, &KMahjongg::startNewNumeric);

    QAction *action = KStandardGameAction::hint(m_pGameView, SLOT(helpMove()), this);
    actionCollection()->addAction(action->objectName(), action);

    QAction *shuffle = actionCollection()->addAction(QStringLiteral("move_shuffle"));
    shuffle->setText(i18n("Shu&ffle"));
    shuffle->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    connect(shuffle, &QAction::triggered, m_pGameView, &GameView::shuffle);

    QAction *angleccw = actionCollection()->addAction(QStringLiteral("view_angleccw"));
    angleccw->setText(i18n("Rotate View Counterclockwise"));
    angleccw->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-left")));
    angleccw->setShortcut(Qt::Key_F);
    connect(angleccw, &QAction::triggered, m_pGameView, &GameView::angleSwitchCCW);

    QAction *anglecw = actionCollection()->addAction(QStringLiteral("view_anglecw"));
    anglecw->setText(i18n("Rotate View Clockwise"));
    anglecw->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-right")));
    anglecw->setShortcut(Qt::Key_G);
    connect(anglecw, &QAction::triggered, m_pGameView, &GameView::angleSwitchCW);

    demoAction = KStandardGameAction::demo(this, SLOT(demoMode()), actionCollection());

    KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    pauseAction = KStandardGameAction::pause(this, SLOT(pause()), actionCollection());

    // move
    undoAction = KStandardGameAction::undo(this, SLOT(undo()), actionCollection());
    redoAction = KStandardGameAction::redo(this, SLOT(redo()), actionCollection());

    // edit
    QAction *boardEdit = actionCollection()->addAction(QStringLiteral("game_board_editor"));
    boardEdit->setText(i18n("&Board Editor"));
    connect(boardEdit, &QAction::triggered, this, &KMahjongg::slotBoardEditor);

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

    statusLabel = new QLabel(QStringLiteral("Kmahjongg"), statusBar());
    statusBar()->addWidget(statusLabel);
}

void KMahjongg::displayTime(const QString& timestring)
{
    gameTimerLabel->setText(i18n("Time: ") + timestring);
}

void KMahjongg::startNewNumeric()
{
    bool ok;
    int s = QInputDialog::getInt(this, i18n("New Game"), i18n("Enter game number:"), 0, 0, INT_MAX, 1, &ok);

    if (ok) {
        startNewGame(s);
    }
}

void KMahjongg::undo()
{
    m_pGameView->undo();
    updateUndoAndRedoStates();
}

void KMahjongg::redo()
{
    m_pGameView->redo();
    updateUndoAndRedoStates();
}

void KMahjongg::showSettings()
{
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }

    //Use the classes exposed by LibKmahjongg for our configuration dialog
    KMahjonggConfigDialog *dialog = new KMahjonggConfigDialog(this, QStringLiteral("settings"), Prefs::self());

    //The Settings class is ours
    dialog->addPage(new Settings(0), i18n("General"), QStringLiteral("games-config-options"));
    dialog->addPage(new KMahjonggLayoutSelector(0, Prefs::self()), i18n("Board Layout"), QStringLiteral("games-config-board"));
    dialog->addTilesetPage();
    dialog->addBackgroundPage();

    connect(dialog, &KMahjonggConfigDialog::settingsChanged, this, &KMahjongg::loadSettings);
    connect(dialog, &KMahjonggConfigDialog::settingsChanged, boardEditor, &Editor::setTilesetFromSettings);

    dialog->show();
}

void KMahjongg::loadLayout()
{
    if (!m_pBoardLayout->load(Prefs::layout())) {
        qCDebug(KMAHJONGG_LOG) << "Error loading the layout. Try to load the default layout.";

        m_pBoardLayout->loadDefault();
    }
}

void KMahjongg::saveSettings()
{
    Prefs::setLayout(m_pBoardLayout->path());
    Prefs::setTileSet(m_pGameView->getTilesetPath());
    Prefs::setBackground(m_pGameView->getBackgroundPath());
    Prefs::setAngle(m_pGameView->getAngle());
    Prefs::self()->save();
}

void KMahjongg::loadSettings()
{
    // Set the blink-matching-tiles option.
    m_pGameView->setMatch(Prefs::showMatchingTiles());

    // Load the tileset.
    if (!m_pGameView->setTilesetPath(Prefs::tileSet())) {
        qCDebug(KMAHJONGG_LOG) << "An error occurred when loading the tileset " << Prefs::tileSet() <<
                    " KMahjongg will continue with the default tileset.";
    }

    // Load the background
    if (!m_pGameView->setBackgroundPath(Prefs::background())) {
        qCDebug(KMAHJONGG_LOG) << "An error occurred when loading the background " << Prefs::background() <<
                    " KMahjongg will continue with the default background.";
    }

    // Maybe load a new layout and start a new game if the layout or random mode has changed.
    if (m_pBoardLayout->path() != Prefs::layout() || m_bLastRandomSetting != Prefs::randomLayout()) {

        // The boardlayout path will likely not be the same as the preference setting if
        // random layouts are set. If they are and were last time we don't want to load
        // a new layout or start a new game when the user may have just changed the
        // tileset, background or other settings.
        // Also, if no saved layout setting, avoid endless recursion via startNewGame.
        if ((!m_bLastRandomSetting || !Prefs::randomLayout()) && !Prefs::layout().isEmpty()) {
            // The user has changed the layout, or the random setting.

            // If random layouts are set a new layout will be loaded when we call
            // startNewGame, so no need to do so here.
            if (!Prefs::randomLayout()) {
                loadLayout();

                delete m_pGameData;
                m_pGameData = new GameData(m_pBoardLayout->board());
                m_pGameView->setGameData(m_pGameData);
            }

            // Track the last random setting.
            m_bLastRandomSetting = Prefs::randomLayout();

            startNewGame();
        }
    }

    saveSettings();
}

void KMahjongg::demoMode()
{
    if (demoAction->isChecked()) {
        loadSettings(); // In case loadGame() has changed the settings.
        updateState(GameState::Demo);
        gameTimer->setTime(0);
        gameTimer->pause();
        m_pGameView->startDemo();
    } else {
        startNewGame();
    }
}

void KMahjongg::pause()
{
    if (pauseAction->isChecked()) {
        gameTimer->pause();
        updateState(GameState::Paused);
        m_pGameView->pause(true);
    } else {
        gameTimer->resume();
        updateState(GameState::Gameplay);
        m_pGameView->pause(false);
    }
}

void KMahjongg::showHighscores()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    const QString layoutName = m_pBoardLayout->authorProperty("Name");
    ksdialog.setConfigGroup(qMakePair(QByteArray(layoutName.toUtf8()), layoutName));
    ksdialog.exec();
}

void KMahjongg::slotBoardEditor()
{
    boardEditor->setVisible(true);

    // Set the default size.
    boardEditor->setGeometry(Prefs::editorGeometry());
}

void KMahjongg::noMovesAvailable()
{
    gameTimer->pause();
    int answer = KMessageBox::questionYesNoCancel(
                 this,
                 i18n("Game Over: You have no moves left."),
                 i18n("Game Over"),
                 KGuiItem(i18n("New Game"), QIcon(actionCollection()->action(KStandardGameAction::name(KStandardGameAction::New))->icon())),
                 KGuiItem(i18n("Restart"), QIcon(actionCollection()->action(KStandardGameAction::name(KStandardGameAction::Restart))->icon())));
    if (answer == KMessageBox::Yes) {
        startNewGame();
    } else if (answer == KMessageBox::No) {
        restartGame();
    }
}

void KMahjongg::startNewGame(int item)
{
    loadSettings(); // In case loadGame() has changed the settings.

    // Only load new layout in random mode if we are not given a game number.
    // Use same layout if restarting game or starting a numbered game.
    if (Prefs::randomLayout() && item == -1) {
        QStringList availableLayouts;
        const QStringList layoutDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("kmahjongg/layouts/"), QStandardPaths::LocateDirectory);
        Q_FOREACH (const QString& dir, layoutDirs) {
            const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
            Q_FOREACH (const QString& file, fileNames) {
                availableLayouts.append(dir + '/' + file);
            }
        }
        const QString layout = availableLayouts.at(qrand() % availableLayouts.size());

        if (m_pBoardLayout->path() != layout) {
            // Try to load the random layout.
            if (!m_pBoardLayout->load(layout)) {
                // Or load the default.
                m_pBoardLayout->loadDefault();
            }

            delete m_pGameData;
            m_pGameData = new GameData(m_pBoardLayout->board());
            m_pGameView->setGameData(m_pGameData);
        }
    }

    m_pGameView->createNewGame(item);

    gameTimer->restart();

    if (m_pGameView->gameGenerated()) {
        updateState(GameState::Gameplay);
        setCaption(m_pBoardLayout->layoutName());
    } else {
        updateState(GameState::Finished);
        gameTimer->pause();
        showItemNumber(0, 0, 0);
    }
}

void KMahjongg::demoOrMoveListAnimationOver(bool bDemoGameLost)
{
    if (bDemoGameLost) {
        KMessageBox::information(this, i18n("Your computer has lost the game."));
    }

    startNewGame();
}

void KMahjongg::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        const QWindowStateChangeEvent *stateEvent = (QWindowStateChangeEvent *) event;
        const Qt::WindowStates oldMinimizedState  = stateEvent->oldState() & Qt::WindowMinimized;

        // N.B. KMahjongg::pause() is not used here, because it is irrelevant to
        // hide the tiles and change the Pause button's state when minimizing.
        if (isMinimized() && oldMinimizedState != Qt::WindowMinimized && m_gameState == GameState::Gameplay) {
            // If playing a game and not paused, stop the clock during minimise.
            gameTimer->pause();
        }
        else if (!isMinimized() && oldMinimizedState == Qt::WindowMinimized && m_gameState == GameState::Gameplay) {
            // If playing a game, start the clock when restoring the window.
            gameTimer->resume();
        }
    }
}

void KMahjongg::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void KMahjongg::gameOver(unsigned short numRemoved, unsigned short cheats)
{
    gameTimer->pause();

    updateState(GameState::Finished);

    KMessageBox::information(this, i18n("You have won!"));

    // get the time in milli secs
    // subtract from 20 minutes to get bonus. if longer than 20 then ignore
    int time = (60 * 20) - gameTimer->seconds();
    if (time < 0) {
        time = 0;
    }
    // conv back to  secs (max bonus = 60*20 = 1200

    // points per removed tile bonus (for deragon max = 144*10 = 1440
    int score = (numRemoved * 20);
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
    const QString layoutName = m_pBoardLayout->authorProperty("Name");
    ksdialog.setConfigGroup(qMakePair(QByteArray(layoutName.toUtf8()), layoutName));
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Score].setNum(score);
    scoreInfo[KScoreDialog::Time] = gameTimer->timeString();
    if (ksdialog.addScore(scoreInfo, KScoreDialog::AskName)) {
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
    QString szBuffer = i18n("Removed: %1/%2  Combinations left: %3", iMaximum - iCurrent, iMaximum, iLeft);
    tilesLeftLabel->setText(szBuffer);

    updateUndoAndRedoStates();
}

void KMahjongg::updateState(GameState state)
{
    m_gameState = state;
    // KXMLGUIClient::stateChanged() sets action-states def. by kmahjonggui.rc.
    switch (state) {
    case GameState::Demo:
        stateChanged("demo_state");
        break;
    case GameState::Paused:
        stateChanged("paused_state");
        break;
    case GameState::Finished:
        stateChanged("finished_state");
        break;
    default:
        stateChanged("gameplay_state");
        updateUndoAndRedoStates();
        break;
    }

    demoAction->setChecked(state == GameState::Demo);
    pauseAction->setChecked(state == GameState::Paused);
}

void KMahjongg::updateUndoAndRedoStates()
{
    undoAction->setEnabled(m_pGameView->checkUndoAllowed());
    redoAction->setEnabled(m_pGameView->checkRedoAllowed());
}

void KMahjongg::restartGame()
{
    if (m_pGameView->gameGenerated()) {
        m_pGameView->createNewGame(m_pGameView->getGameNumber());
        gameTimer->restart();
        updateState(GameState::Gameplay);
    }
}

void KMahjongg::loadGame()
{
    const QString filename = QFileDialog::getOpenFileName(this, i18n("Load Game"), QString(), i18n("KMahjongg Game (*.kmgame)"));

    if (filename.isEmpty()) {
        return;
    }

    QFile infile(filename);

    if (!infile.open(QFile::ReadOnly)) {
        KMessageBox::sorry(this, i18n("Could not read from file. Aborting."));
        return;
    }

    QDataStream in(&infile);

    // verify that it is a kmahjongg game file
    QString magic;
    in >> magic;

    if (QString::compare(magic, gameMagic, Qt::CaseSensitive) != 0) {
        KMessageBox::sorry(this, i18n("File is not a KMahjongg game."));
        infile.close();

        return;
    }

    // verify data version of saved data
    qint32 version;
    in >> version;

    if (version == gameDataVersion) {
        in.setVersion(QDataStream::Qt_4_0);
    } else {
        KMessageBox::sorry(this, i18n("File format not recognized."));
        infile.close();
        return;
    }

    QString tileSetName;
    in >> tileSetName;
    m_pGameView->setTilesetPath(tileSetName);

    QString backgroundName;
    in >> backgroundName;
    m_pGameView->setBackgroundPath(backgroundName);

    QString boardLayoutName;
    in >> boardLayoutName;
    m_pBoardLayout->load(boardLayoutName);

    //GameTime
    uint seconds;
    in >> seconds;
    gameTimer->setTime(seconds);

    delete m_pGameData;
    m_pGameData = new GameData(m_pBoardLayout->board());
    m_pGameData->loadFromStream(in);
    m_pGameView->setGameData(m_pGameData);

    // Get GameNumber (used not to be saved, so might evaluate to zero).
    qint64 gameNum = 0;
    in >> gameNum;
    if (gameNum > 0) {
        m_pGameView->setGameNumber(gameNum);
    }

    infile.close();
    updateState(GameState::Gameplay);
}

void KMahjongg::saveGame()
{
    gameTimer->pause();

    const QString filename = QFileDialog::getSaveFileName(this, i18n("Save Game"), QString(), i18n("KMahjongg Game (*.kmgame)"));

    if (filename.isEmpty()) {
        gameTimer->resume();
        return;
    }

    QFile outfile(filename);

    if (!outfile.open(QFile::WriteOnly)) {
        KMessageBox::sorry(this, i18n("Could not open file for saving."));
        gameTimer->resume();
        return;
    }

    QDataStream out(&outfile);

    // Write a header with a "magic number" and a version
    out << QString(gameMagic);
    out << static_cast<qint32>(gameDataVersion);
    out.setVersion(QDataStream::Qt_4_0);

    out << m_pGameView->getTilesetPath();
    out << m_pGameView->getBackgroundPath();
    out << m_pBoardLayout->path();

    // GameTime
    out << gameTimer->seconds();

    // GameData
    m_pGameData->saveToStream(out);

    // GameNumber
    // write game number after game data to obtain backwards compatibility
    out << static_cast<qint64>(m_pGameView->getGameNumber());

    outfile.close();
    gameTimer->resume();
}
