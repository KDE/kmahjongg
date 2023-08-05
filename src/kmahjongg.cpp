/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>
    SPDX-FileCopyrightText: 2006-2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// own
#include "kmahjongg.h"

// STL
#include <limits.h>

// Qt
#include <QAction>
#include <QFileDialog>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QPixmapCache>
#include <QScreen>
#include <QShortcut>
#include <QStatusBar>
#include <QWindowStateChangeEvent>
#include <QRandomGenerator>

// KF
#include <kwidgetsaddons_version.h>
#include <KActionCollection>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>

// KDEGames
#include <KGameClock>
#include <KScoreDialog>
#include <KStandardGameAction>

// LibKMahjongg
#include <KMahjonggConfigDialog>

// KMahjongg
#include "editor.h"
#include "gamedata.h"
#include "gamescene.h"
#include "gameview.h"
#include "kmahjongg_debug.h"
#include "kmahjongglayout.h"
#include "kmahjongglayoutselector.h"
#include "prefs.h"
#include "ui_settings.h"

namespace {
inline QString gameMagic() { return QStringLiteral("kmahjongg-gamedata"); }
constexpr int gameDataVersion = 1;
}

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class Settings : public QWidget, public Ui::Settings
{
public:
    explicit Settings(QWidget * parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

KMahjongg::KMahjongg(QWidget * parent)
    : KXmlGuiWindow(parent)
    , m_gameState(GameState::Gameplay)
    , m_gameView(nullptr)
    , m_gameData(nullptr)
    , m_boardLayout(new KMahjonggLayout())
{
    //Use up to 3MB for global application pixmap cache
    QPixmapCache::setCacheLimit(3 * 1024);

    // minimum area required to display the field
    setMinimumSize(320, 320);

    // init board widget
    m_gameScene = new GameScene();

    loadLayout();

    // init game data
    m_gameData = new GameData(m_boardLayout->board());

    // init view and add to window
    m_gameView = new GameView(m_gameScene, m_gameData, this);
    setCentralWidget(m_gameView);

    m_boardEditor = new Editor();
    m_boardEditor->setModal(false);

    setupStatusBar();
    setupKAction();

    m_gameTimer = new KGameClock(this);

    connect(m_gameTimer, &KGameClock::timeChanged, this, &KMahjongg::displayTime);
    connect(m_gameView, &GameView::statusTextChanged, this, &KMahjongg::showStatusText);
    connect(m_gameView, &GameView::itemNumberChanged, this, &KMahjongg::showItemNumber);
    connect(m_gameView, &GameView::gameOver, this, &KMahjongg::gameOver);
    connect(m_gameView, &GameView::demoOrMoveListAnimationOver, this, &KMahjongg::demoOrMoveListAnimationOver);
    connect(m_gameView, &GameView::noMovesAvailable, this, &KMahjongg::noMovesAvailable);
    connect(m_gameScene, &GameScene::rotateCW, m_gameView, &GameView::angleSwitchCW);
    connect(m_gameScene, &GameScene::rotateCCW, m_gameView, &GameView::angleSwitchCCW);

    m_bLastRandomSetting = Prefs::randomLayout();

    loadSettings();

    m_boardEditor->setTilesetFromSettings();

    startNewGame();
}

KMahjongg::~KMahjongg()
{
    delete m_gameView;
    delete m_gameScene;
    delete m_boardLayout;
    delete m_boardEditor;
    delete m_gameData;
}

void KMahjongg::setupKAction()
{
    KStandardGameAction::gameNew(this, &KMahjongg::startNewGame, actionCollection());
    KStandardGameAction::load(this, &KMahjongg::loadGame, actionCollection());
    KStandardGameAction::save(this, &KMahjongg::saveGame, actionCollection());
    KStandardGameAction::quit(this, &KMahjongg::close, actionCollection());
    KStandardGameAction::restart(this, &KMahjongg::restartGame, actionCollection());

    QAction * newNumGame = actionCollection()->addAction(QStringLiteral("game_new_numeric"));
    newNumGame->setText(i18n("New Numbered Game..."));
    connect(newNumGame, &QAction::triggered, this, &KMahjongg::startNewNumeric);

    QAction * action = KStandardGameAction::hint(m_gameView, &GameView::helpMove, this);
    actionCollection()->addAction(action->objectName(), action);

    QAction * shuffle = actionCollection()->addAction(QStringLiteral("move_shuffle"));
    shuffle->setText(i18n("Shu&ffle"));
    shuffle->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    connect(shuffle, &QAction::triggered, m_gameView, &GameView::shuffle);

    QAction * angleccw = actionCollection()->addAction(QStringLiteral("view_angleccw"));
    angleccw->setText(i18n("Rotate View Counterclockwise"));
    angleccw->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-left")));
    actionCollection()->setDefaultShortcut(angleccw, Qt::Key_F);
    connect(angleccw, &QAction::triggered, m_gameView, &GameView::angleSwitchCCW);

    QAction * anglecw = actionCollection()->addAction(QStringLiteral("view_anglecw"));
    anglecw->setText(i18n("Rotate View Clockwise"));
    anglecw->setIcon(QIcon::fromTheme(QStringLiteral("object-rotate-right")));
    actionCollection()->setDefaultShortcut(anglecw, Qt::Key_G);
    connect(anglecw, &QAction::triggered, m_gameView, &GameView::angleSwitchCW);

    m_fullscreenAction = KStandardAction::fullScreen(
        this, &KMahjongg::toggleFullscreen, this, actionCollection()
    );

    m_demoAction = KStandardGameAction::demo(this, &KMahjongg::demoMode, actionCollection());

    KStandardGameAction::highscores(this, &KMahjongg::showHighscores, actionCollection());
    m_pauseAction = KStandardGameAction::pause(this, &KMahjongg::pause, actionCollection());

    // move
    m_undoAction = KStandardGameAction::undo(this, &KMahjongg::undo, actionCollection());
    m_redoAction = KStandardGameAction::redo(this, &KMahjongg::redo, actionCollection());

    // edit
    QAction * boardEdit = actionCollection()->addAction(QStringLiteral("game_board_editor"));
    boardEdit->setText(i18n("&Board Editor"));
    connect(boardEdit, &QAction::triggered, this, &KMahjongg::slotBoardEditor);

    // settings
    KStandardAction::preferences(this, &KMahjongg::showSettings, actionCollection());
    setupGUI(qApp->primaryScreen()->geometry().size() * 0.7);
}

void KMahjongg::toggleFullscreen(bool fullscreen)
{
    if (fullscreen) {
        setWindowState(Qt::WindowState::WindowFullScreen);
    } else {
        setWindowState(Qt::WindowState::WindowNoState);
    }
}

void KMahjongg::setupStatusBar()
{
    m_gameTimerLabel = new QLabel(i18n("Time: 0:00:00"), statusBar());
    statusBar()->addWidget(m_gameTimerLabel);

    QFrame * timerDivider = new QFrame(statusBar());
    timerDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(timerDivider);

    m_tilesLeftLabel = new QLabel(i18n("Removed: 0000/0000"), statusBar());
    statusBar()->addWidget(m_tilesLeftLabel, 1);

    QFrame * tileDivider = new QFrame(statusBar());
    tileDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(tileDivider);

    m_gameNumLabel = new QLabel(i18n("Game: 000000000000000000000"), statusBar());
    statusBar()->addWidget(m_gameNumLabel);

    QFrame * gameNumDivider = new QFrame(statusBar());
    gameNumDivider->setFrameStyle(QFrame::VLine);
    statusBar()->addWidget(gameNumDivider);

    m_statusLabel = new QLabel(QStringLiteral("Kmahjongg"), statusBar());
    statusBar()->addWidget(m_statusLabel);
}

void KMahjongg::displayTime(const QString & timestring)
{
    m_gameTimerLabel->setText(i18n("Time: ") + timestring);
}

void KMahjongg::startNewNumeric()
{
    bool ok;
    int s = QInputDialog::getInt(this, i18nc("@title:window", "New Game"), i18n("Enter game number:"), 0, 0, INT_MAX, 1, &ok);

    if (ok) {
        startNewGameWithNumber(s);
    }
}

void KMahjongg::undo()
{
    // If the game got stuck (no more matching tiles), the game timer is paused.
    // So resume timer if the player decides to undo moves from that state.
    if (m_gameState == GameState::Stuck) {
        m_gameTimer->resume();
    }
    m_gameView->undo();
    updateState(GameState::Gameplay);
    updateUndoAndRedoStates();
}

void KMahjongg::redo()
{
    m_gameView->redo();
    updateUndoAndRedoStates();
}

void KMahjongg::showSettings()
{
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }

    //Use the classes exposed by LibKmahjongg for our configuration dialog
    KMahjonggConfigDialog * dialog = new KMahjonggConfigDialog(this, QStringLiteral("settings"), Prefs::self());

    //The Settings class is ours
    dialog->addPage(new Settings(dialog), i18n("General"), QStringLiteral("games-config-options"));
    dialog->addPage(new KMahjonggLayoutSelector(dialog, Prefs::self()), i18n("Board Layout"), QStringLiteral("games-config-board"));
    dialog->addTilesetPage();
    dialog->addBackgroundPage();

    connect(dialog, &KMahjonggConfigDialog::settingsChanged, this, &KMahjongg::loadSettings);
    connect(dialog, &KMahjonggConfigDialog::settingsChanged, m_boardEditor, &Editor::setTilesetFromSettings);

    dialog->show();
}

void KMahjongg::loadLayout()
{
    if (!m_boardLayout->load(Prefs::layout())) {
        qCDebug(KMAHJONGG_LOG) << "Error loading the layout. Try to load the default layout.";

        m_boardLayout->loadDefault();
        Prefs::setLayout(m_boardLayout->path());
    }
}

void KMahjongg::saveSettings()
{
    Prefs::setLayout(m_boardLayout->path());
    Prefs::setTileSet(m_gameView->getTilesetPath());
    Prefs::setBackground(m_gameView->getBackgroundPath());
    Prefs::setAngle(m_gameView->getAngle());
    Prefs::self()->save();
}

void KMahjongg::loadSettings()
{
    // Set the blink-matching-tiles option.
    m_gameView->setMatch(Prefs::showMatchingTiles());

    // Load the tileset.
    if (!m_gameView->setTilesetPath(Prefs::tileSet())) {
        qCDebug(KMAHJONGG_LOG) << "An error occurred when loading the tileset " << Prefs::tileSet() << " KMahjongg will continue with the default tileset.";
    }

    // Load the background
    if (!m_gameView->setBackgroundPath(Prefs::background())) {
        qCDebug(KMAHJONGG_LOG) << "An error occurred when loading the background " << Prefs::background() << " KMahjongg will continue with the default background.";
    }

    // Set whether removed tiles should be shown.
    m_gameView->showRemovedTiles(Prefs::removedTiles());

    // Maybe load a new layout and start a new game if the layout or random mode has changed.
    if (m_boardLayout->path() != Prefs::layout() || m_bLastRandomSetting != Prefs::randomLayout()) {
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

                delete m_gameData;
                m_gameData = new GameData(m_boardLayout->board());
                m_gameView->setGameData(m_gameData);
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
    // Test if a game is already running and whether the user wants to save the game.
    if (m_gameChanged && m_demoAction->isChecked()) {
        if (!askSaveGame(QStringLiteral("kmahjongg_start_demo_ask_for_saving"))) {
            // The user canceled the action, so don't go further with demo mode.
            m_demoAction->setChecked(false);
            return;
        }
    }

    if (m_demoAction->isChecked()) {
        loadSettings(); // In case loadGame() has changed the settings.
        updateState(GameState::Demo);
        m_gameTimer->setTime(0);
        m_gameTimer->pause();
        m_gameView->startDemo();
    } else {
        startNewGame();
    }
}

void KMahjongg::pause()
{
    if (m_pauseAction->isChecked()) {
        m_gameTimer->pause();
        updateState(GameState::Paused);
        m_gameView->pause(true);
    } else {
        m_gameTimer->resume();
        updateState(GameState::Gameplay);
        m_gameView->pause(false);
    }
}

void KMahjongg::showHighscores()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Time, this);
    const QString layoutName = m_boardLayout->authorProperty(QStringLiteral("Name"));
    ksdialog.setConfigGroup(qMakePair(QByteArray(layoutName.toUtf8()), layoutName));
    ksdialog.exec();
}

void KMahjongg::slotBoardEditor()
{
    m_boardEditor->setVisible(true);

    // Set the default size.
    m_boardEditor->setGeometry(Prefs::editorGeometry());
}

void KMahjongg::noMovesAvailable()
{
    m_gameTimer->pause();
    updateState(GameState::Stuck);

    const QIcon newGameIcon = actionCollection()->action(QString::fromLatin1(KStandardGameAction::name(KStandardGameAction::New)))->icon();
    const QIcon restartIcon = actionCollection()->action(QString::fromLatin1(KStandardGameAction::name(KStandardGameAction::Restart)))->icon();
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    int answer = KMessageBox::questionTwoActionsCancel(
#else
    int answer = KMessageBox::questionYesNoCancel(
#endif
        this,
        i18n("Game Over: You have no moves left."),
        i18n("Game Over"),
        KGuiItem(i18n("New Game"), newGameIcon),
        KGuiItem(i18n("Restart"), restartIcon));
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (answer == KMessageBox::PrimaryAction) {
#else
    if (answer == KMessageBox::Yes) {
#endif
        startNewGame();
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    } else if (answer == KMessageBox::SecondaryAction) {
#else
    } else if (answer == KMessageBox::No) {
#endif
        restartGame();
    }
}

void KMahjongg::startNewGame()
{
    startNewGameWithNumber(-1);
}

void KMahjongg::startNewGameWithNumber(int item)
{
    if (!testForGameChangeSave(QStringLiteral("kmahjongg_start_new_game_ask_for_saving"))) {
        return;
    }

    loadSettings(); // In case loadGame() has changed the settings.

    // Only load new layout in random mode if we are not given a game number.
    // Use same layout if restarting game or starting a numbered game.
    if (Prefs::randomLayout() && item == -1) {
        QStringList availableLayouts;
        const QStringList layoutDirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("layouts/"), QStandardPaths::LocateDirectory);
        for (const QString & dir : layoutDirs) {
            const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
            for (const QString & file : fileNames) {
                availableLayouts.append(dir + QLatin1Char('/') + file);
            }
        }
        const QString layout = availableLayouts.at(QRandomGenerator::global()->bounded(availableLayouts.size()));

        if (m_boardLayout->path() != layout) {
            // Try to load the random layout.
            if (!m_boardLayout->load(layout)) {
                // Or load the default.
                m_boardLayout->loadDefault();
            }

            delete m_gameData;
            m_gameData = new GameData(m_boardLayout->board());
            m_gameView->setGameData(m_gameData);
        }
    }

    m_gameView->createNewGame(item);

    m_gameTimer->restart();

    if (m_gameView->gameGenerated()) {
        updateState(GameState::Gameplay);
        setCaption(m_boardLayout->layoutName());
    } else {
        updateState(GameState::Finished);
        m_gameTimer->pause();
        showItemNumber(0, 0, 0);
    }

    // At the beginning, the game has not been changed.
    m_gameChanged = false;
}

void KMahjongg::demoOrMoveListAnimationOver(bool demoGameLost)
{
    if (demoGameLost) {
        KMessageBox::information(this, i18n("Your computer has lost the game."));
    }

    startNewGame();
}

void KMahjongg::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::WindowStateChange) {
        const QWindowStateChangeEvent * stateEvent = static_cast<QWindowStateChangeEvent *>(event);
        const Qt::WindowStates oldMinimizedState = stateEvent->oldState() & Qt::WindowMinimized;

        // N.B. KMahjongg::pause() is not used here, because it is irrelevant to
        // hide the tiles and change the Pause button's state when minimizing.
        if (isMinimized() && oldMinimizedState != Qt::WindowMinimized && m_gameState == GameState::Gameplay) {
            // If playing a game and not paused, stop the clock during minimise.
            m_gameTimer->pause();
        } else if (!isMinimized() && oldMinimizedState == Qt::WindowMinimized && m_gameState == GameState::Gameplay) {
            // If playing a game, start the clock when restoring the window.
            m_gameTimer->resume();
        }
    }
}

void KMahjongg::closeEvent(QCloseEvent * event)
{
    saveSettings();
    event->accept();
}

void KMahjongg::gameOver(unsigned short numRemoved, unsigned short cheats)
{
    m_gameTimer->pause();
    updateState(GameState::Finished);

    // get the time in milli secs
    // subtract from 20 minutes to get bonus. if longer than 20 then ignore
    int time = (60 * 20) - m_gameTimer->seconds();
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

    const QString infoGameWon = i18n("You have won with a final time of %1 and a score of %2!", m_gameTimer->timeString(), score);
    KMessageBox::information(this, infoGameWon);

    //TODO: add gameNum as a Custom KScoreDialog field?
    //int elapsed = gameTimer->seconds();
    //long gameNum = m_pGameView->getGameNumber();
    //theHighScores->checkHighScore(score, elapsed, gameNum, m_pGameView->getBoardName());
    QPointer<KScoreDialog> ksdialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Time, this);
    const QString layoutName = m_boardLayout->authorProperty(QStringLiteral("Name"));
    ksdialog->setConfigGroup(qMakePair(QByteArray(layoutName.toUtf8()), layoutName));
    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Score].setNum(score);
    scoreInfo[KScoreDialog::Time] = m_gameTimer->timeString();
    if (ksdialog->addScore(scoreInfo, KScoreDialog::AskName)) {
        ksdialog->exec();
    }

    m_gameView->startMoveListAnimation();
}

void KMahjongg::showStatusText(const QString & msg, long board)
{
    m_statusLabel->setText(msg);
    const QString str = i18n("Game number: %1", board);
    m_gameNumLabel->setText(str);
}

void KMahjongg::showItemNumber(int maximum, int current, int left)
{
    // This method will be called, if the number of tiles change. If there is a lower number of tiles than the
    // maximum, some tiles have been removed. If the game is not in demo mode, mark the game as changed.
    m_gameChanged = current < maximum && !m_demoAction->isChecked();

    const QString szBuffer = i18n("Removed: %1/%2  Combinations left: %3", maximum - current, maximum, left);
    m_tilesLeftLabel->setText(szBuffer);

    updateUndoAndRedoStates();
}

void KMahjongg::updateState(GameState state)
{
    m_gameState = state;
    // KXMLGUIClient::stateChanged() sets action-states def. by kmahjonggui.rc.
    switch (state) {
        case GameState::Demo:
            stateChanged(QStringLiteral("demo_state"));
            break;
        case GameState::Paused:
            stateChanged(QStringLiteral("paused_state"));
            break;
        case GameState::Finished:
            stateChanged(QStringLiteral("finished_state"));
            break;
        case GameState::Stuck:
            stateChanged(QStringLiteral("stuck_state"));
            break;
        default:
            stateChanged(QStringLiteral("gameplay_state"));
            updateUndoAndRedoStates();
            break;
    }

    m_demoAction->setChecked(state == GameState::Demo);
    m_pauseAction->setChecked(state == GameState::Paused);
}

void KMahjongg::updateUndoAndRedoStates()
{
    m_undoAction->setEnabled(m_gameView->checkUndoAllowed());
    m_redoAction->setEnabled(m_gameView->checkRedoAllowed());
}

void KMahjongg::restartGame()
{
    if (!testForGameChangeSave(QStringLiteral("kmahjongg_restart_game_ask_for_saving"))) {
        return;
    }

    if (m_gameView->gameGenerated()) {
        m_gameView->createNewGame(m_gameView->getGameNumber());
        m_gameTimer->restart();
        updateState(GameState::Gameplay);
    }

    // The game has not been changed at the beginning.
    m_gameChanged = false;
}

void KMahjongg::loadGame()
{
    if (!testForGameChangeSave(QStringLiteral("kmahjongg_load_game_ask_for_saving"))) {
        return;
    }

    const QString filename = QFileDialog::getOpenFileName(this, i18nc("@title:window", "Load Game"), QString(), i18n("KMahjongg Game (*.kmgame)"));

    if (filename.isEmpty()) {
        return;
    }

    QFile infile(filename);

    if (!infile.open(QFile::ReadOnly)) {
        KMessageBox::error(this, i18n("Could not read from file. Aborting."));
        return;
    }

    QDataStream in(&infile);

    // verify that it is a kmahjongg game file
    QString magic;
    in >> magic;

    if (QString::compare(magic, gameMagic(), Qt::CaseSensitive) != 0) {
        KMessageBox::error(this, i18n("File is not a KMahjongg game."));
        infile.close();

        return;
    }

    // verify data version of saved data
    qint32 version;
    in >> version;

    if (version == gameDataVersion) {
        in.setVersion(QDataStream::Qt_4_0);
    } else {
        KMessageBox::error(this, i18n("File format not recognized."));
        infile.close();
        return;
    }

    QString tileSetName;
    in >> tileSetName;
    m_gameView->setTilesetPath(tileSetName);

    QString backgroundName;
    in >> backgroundName;
    m_gameView->setBackgroundPath(backgroundName);

    QString boardLayoutName;
    in >> boardLayoutName;
    m_boardLayout->load(boardLayoutName);

    //GameTime
    uint seconds;
    in >> seconds;
    m_gameTimer->setTime(seconds);

    delete m_gameData;
    m_gameData = new GameData(m_boardLayout->board());
    m_gameData->loadFromStream(in);
    m_gameView->setGameData(m_gameData);

    // Get GameNumber (used not to be saved, so might evaluate to zero).
    qint64 gameNum = 0;
    in >> gameNum;
    if (gameNum > 0) {
        m_gameView->setGameNumber(gameNum);
    }

    infile.close();
    updateState(GameState::Gameplay);
}

void KMahjongg::saveGame()
{
    m_gameTimer->pause();

    const QString filename = QFileDialog::getSaveFileName(this, i18nc("@title:window", "Save Game"), QString(), i18n("KMahjongg Game (*.kmgame)"));

    if (filename.isEmpty()) {
        m_gameTimer->resume();
        return;
    }

    QFile outfile(filename);

    if (!outfile.open(QFile::WriteOnly)) {
        KMessageBox::error(this, i18n("Could not open file for saving."));
        m_gameTimer->resume();
        return;
    }

    QDataStream out(&outfile);

    // Write a header with a "magic number" and a version
    out << gameMagic();
    out << static_cast<qint32>(gameDataVersion);
    out.setVersion(QDataStream::Qt_4_0);

    out << m_gameView->getTilesetPath();
    out << m_gameView->getBackgroundPath();
    out << m_boardLayout->path();

    // GameTime
    out << m_gameTimer->seconds();

    // GameData
    m_gameData->saveToStream(out);

    // GameNumber
    // write game number after game data to obtain backwards compatibility
    out << static_cast<qint64>(m_gameView->getGameNumber());

    outfile.close();
    m_gameTimer->resume();

    // If the game has been saved, there has been no changed done to ask for saving.
    m_gameChanged = false;
}

bool KMahjongg::askSaveGame(const QString &dontAskAgainName)
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    const KMessageBox::ButtonCode ret = KMessageBox::questionTwoActionsCancel(this,
#else
    const KMessageBox::ButtonCode ret = KMessageBox::questionYesNoCancel(this, 
#endif
            i18n("Do you want to save your game?"), i18n("Save game?"), KStandardGuiItem::save(), 
            KStandardGuiItem::dontSave(), KStandardGuiItem::cancel(), dontAskAgainName);

    switch (ret) {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    case KMessageBox::PrimaryAction:
#else
    case KMessageBox::ButtonCode::Yes:
#endif
        saveGame();
        break;
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    case KMessageBox::SecondaryAction:
#else
    case KMessageBox::ButtonCode::No:
#endif
        break;
    case KMessageBox::ButtonCode::Cancel:
        return false;
    default:
        return false;
    }

    return true;
}

bool KMahjongg::testForGameChangeSave(const QString &dontAskAgainName)
{
    // Ask to save the game when it has been changed. The question is also only relevant, if the user is in gameplay
    // or the game has been paused.
    if (m_gameChanged && (m_gameState == GameState::Gameplay || m_gameState == GameState::Paused)) {
        if (!askSaveGame(dontAskAgainName)) {
            // The user wants to cancel the action.
            return false;
        }
    }

    return true;
}

#include "moc_kmahjongg.cpp"
