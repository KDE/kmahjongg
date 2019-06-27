/*
    KMahjonggLayoutSelector
    Part of kmahjongg, the classic mahjongg game for KDE

    Copyright (C) 2007 Mauricio Piacentini   <mauricio@tabuleiro.com>

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

    //The lineEdit widget holds our tileset path, but the user does not manipulate it directly
    kcfg_Layout->hide();

    //No new stuff yet
    getNewButton->hide();

    //Now get our tilesets into a list
    QStringList tilesAvailable;

    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("layouts/"), QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString & dir, dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
        Q_FOREACH (const QString & file, fileNames) {
            tilesAvailable.append(dir + '/' + file);
        }
    }
    tilesAvailable.sort();
    QString namestr(QStringLiteral("Name"));
    int numvalidentries = 0;
    for (int i = 0; i < tilesAvailable.size(); ++i) {
        KMahjonggLayout * aset = new KMahjonggLayout();
        QString atileset = tilesAvailable.at(i);
        if (aset->load(atileset)) {
            layoutMap.insert(aset->authorProperty(namestr), aset);
            layoutList->addItem(aset->authorProperty(namestr));
            //Find if this is our currently configured Tileset
            if (atileset == initialGroup) {
                //Select current entry
                layoutList->setCurrentRow(numvalidentries);
                layoutChanged();
            }
            ++numvalidentries;
        } else {
            delete aset;
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
    layoutAuthor->setText(selLayout->authorProperty(QStringLiteral("Author")));
    layoutContact->setText(selLayout->authorProperty(QStringLiteral("AuthorEmail")));
    layoutDescription->setText(selLayout->authorProperty(QStringLiteral("Description")));

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
