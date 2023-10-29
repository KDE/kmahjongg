/*
    SPDX-FileCopyrightText: 2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// own
#include "kmahjongglayoutselector.h"

// Qt
#include <QDir>

// KMahjongg
#include "gamedata.h"
#include "gamescene.h"
#include "gameview.h"
#include "kmahjongglayout.h"
#include "prefs.h"

KMahjonggLayoutSelector::KMahjonggLayoutSelector(QWidget * parent, KConfigSkeleton * aconfig)
    : QWidget(parent)
    , m_gameData(nullptr)
{
    setupUi(this);
    m_gameScene = new GameScene(this);
    m_gameView = new GameView(m_gameScene, nullptr, layoutPreview);
    m_gameView->resize(layoutPreview->size());
    m_gameView->setInteractive(false);
    setupData(aconfig);
}

KMahjonggLayoutSelector::~KMahjonggLayoutSelector()
{
    qDeleteAll(layoutMap);
}

void KMahjonggLayoutSelector::setupData(KConfigSkeleton * aconfig)
{
    //Get our currently configured Layout entry
    KConfig * config = aconfig->config();
    KConfigGroup group = config->group("General");
    QString initialGroup = group.readEntry("Layout_file");

    //The lineEdit widget holds our layout path, but the user does not manipulate it directly
    kcfg_Layout->hide();

    //No new stuff yet
    getNewButton->hide();

    //Now get our layouts into a list
    QStringList layoutsAvailable;

    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("layouts"), QStandardPaths::LocateDirectory);
    for (const QString & dir : dirs) {
        const QStringList fileNames = QDir(dir).entryList({QStringLiteral("*.desktop")});
        for (const QString & file : fileNames) {
            layoutsAvailable.append(dir + QLatin1Char('/') + file);
        }
    }
    layoutsAvailable.sort();

    int numvalidentries = 0;
    for (const QString &layoutPath : std::as_const(layoutsAvailable)) {
        auto *alayout = new KMahjonggLayout();
        if (alayout->load(layoutPath)) {
            const QString name = alayout->name();
            layoutMap.insert(name, alayout);
            layoutList->addItem(name);
            //Find if this is our currently configured layout
            if (layoutPath == initialGroup) {
                //Select current entry
                layoutList->setCurrentRow(numvalidentries);
                layoutChanged();
            }
            ++numvalidentries;
        } else {
            delete alayout;
        }
    }

    connect(layoutList, &QListWidget::currentItemChanged, this, &KMahjonggLayoutSelector::layoutChanged);
}

void KMahjonggLayoutSelector::layoutChanged()
{
    KMahjonggLayout * selLayout = layoutMap.value(layoutList->currentItem()->text());
    //Sanity checkings. Should not happen.
    if (!selLayout) {
        return;
    }
    if (selLayout->path() == kcfg_Layout->text()) {
        return;
    }

    kcfg_Layout->setText(selLayout->path());
    layoutAuthor->setText(selLayout->authorName());
    layoutContact->setText(selLayout->authorEmailAddress());
    layoutDescription->setText(selLayout->description());

    //If settings for tiles/background have been applied, update our preview
    if (m_gameView->getTilesetPath() != Prefs::tileSet()) {
        m_gameView->setTilesetPath(Prefs::tileSet());
    }
    if (m_gameView->getBackgroundPath() != Prefs::background()) {
        m_gameView->setBackgroundPath(Prefs::background());
    }

    //Now load the boardLayout
    delete m_gameData;
    m_gameData = new GameData(selLayout->board());
    m_gameView->setGameData(m_gameData);

    m_gameView->createNewGame();
}

void KMahjonggLayoutSelector::useRandomLayoutToggled(bool active)
{
    widgetNoRandom->setEnabled(!active);
    m_gameView->setVisible(!active);
}

#include "moc_kmahjongglayoutselector.cpp"
