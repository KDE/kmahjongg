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

#include "kmahjongglayoutselector.h"
#include "kmahjongglayout.h"
#include "GameView.h"
#include "GameScene.h"
#include "GameData.h"
#include "prefs.h"

#include <KLocalizedString>

#include <QDir>
#include <QPainter>


KMahjonggLayoutSelector::KMahjonggLayoutSelector( QWidget* parent, KConfigSkeleton * aconfig )
        : QWidget( parent ),
        m_pGameData(NULL)
{
    setupUi(this);
    m_pGameScene = new GameScene();
    m_pGameView = new GameView(m_pGameScene, NULL, layoutPreview);
    m_pGameView->resize(layoutPreview->size());
    m_pGameView->setInteractive(false);
    setupData(aconfig);
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

    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("kmahjongg/layouts"), QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString& dir, dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.desktop"));
        Q_FOREACH (const QString& file, fileNames) {
            tilesAvailable.append(dir + '/' + file);
        }
    }
    tilesAvailable.sort();
    QString namestr(QStringLiteral("Name"));
    int numvalidentries = 0;
    for (int i = 0; i < tilesAvailable.size(); ++i)
    {
        KMahjonggLayout * aset = new KMahjonggLayout();
        QString atileset = tilesAvailable.at(i);
        if (aset->load(atileset)) {
            layoutMap.insert(aset->authorProperty(namestr), aset);
            layoutList->addItem(aset->authorProperty(namestr));
            //Find if this is our currently configured Tileset
            if (atileset==initialGroup) {
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
    if (selLayout->path()==kcfg_Layout->text()) {
        return;
    }
    QString authstr(QStringLiteral("Author"));
    QString contactstr(QStringLiteral("AuthorEmail"));
    QString descstr(QStringLiteral("Description"));
    kcfg_Layout->setText(selLayout->path());
    layoutAuthor->setText(selLayout->authorProperty("Author"));
    layoutContact->setText(selLayout->authorProperty("AuthorEmail"));
    layoutDescription->setText(selLayout->authorProperty("Description"));

    //If settings for tiles/background have been applied, update our preview
    if (m_pGameView->getTilesetPath() != Prefs::tileSet()) {
        m_pGameView->setTilesetPath(Prefs::tileSet());
    }
    if (m_pGameView->getBackgroundPath() != Prefs::background()) {
        m_pGameView->setBackgroundPath(Prefs::background());
    }

    //Now load the boardLayout temporarily
    GameData *pGameDataOld = m_pGameData;
    m_pGameData = new GameData(selLayout->board());
    m_pGameView->setGameData(m_pGameData);

    delete pGameDataOld;

    m_pGameView->createNewGame();

    /* //Let the tileset calculate its ideal size for the preview area, but reduce the margins a bit (pass oversized drawing area)
    QSize tilesize = selTileset->preferredTileSize(tilesetPreview->size()*1.3, 1, 1);
    selTileset->reloadTileset(tilesize);
    //Draw the preview
    QImage qiRend(tilesetPreview->size(),QImage::Format_ARGB32_Premultiplied);
    qiRend.fill(0);
    QPainter p(&qiRend);
    //Calculate the margins to center the tile
    QSize margin = tilesetPreview->size() - tilesize;
    //Draw unselected tile and first tileface
    p.drawPixmap(margin.width()/2, margin.height()/2, selTileset->unselectedTile(1));
    p.drawPixmap(margin.width()/2, margin.height()/2, selTileset->tileface(0));
    tilesetPreview->setPixmap(QPixmap::fromImage(qiRend)); */
}

void KMahjonggLayoutSelector::useRandomLayoutToggled(bool active)
{
    widgetNoRandom->setEnabled(!active);
}
