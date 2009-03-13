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

#include "boardwidget.h"
#include "prefs.h"

#include <klocale.h>
#include <kstandarddirs.h>
#include <QPainter>

#include "kmahjongglayout.h"

KMahjonggLayoutSelector::KMahjonggLayoutSelector( QWidget* parent, KConfigSkeleton * aconfig )
        : QWidget( parent )
{
    setupUi(this);
    bw = new BoardWidget( layoutPreview );
    bw->resize(layoutPreview->size());
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

    //This will also load our resourcedir if it is not done already
    KMahjonggLayout tile;

    //Now get our tilesets into a list
    QStringList tilesAvailable = KGlobal::dirs()->findAllResources("kmahjongglayout", QString("*.desktop"), KStandardDirs::Recursive);
    tilesAvailable.sort();
    QString namestr("Name");
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
    
    connect(layoutList, SIGNAL(currentItemChanged ( QListWidgetItem * , QListWidgetItem * )), this, SLOT(layoutChanged()));
}

void KMahjonggLayoutSelector::layoutChanged()
{
    KMahjonggLayout * selLayout = layoutMap.value(layoutList->currentItem()->text());
        //Sanity checkings. Should not happen.
    if (!selLayout) return;
    if (selLayout->path()==kcfg_Layout->text()) {
        return;
    }
    QString authstr("Author");
    QString contactstr("AuthorEmail");
    QString descstr("Description");
    kcfg_Layout->setText(selLayout->path());
    layoutAuthor->setText(selLayout->authorProperty(authstr));
    layoutContact->setText(selLayout->authorProperty(contactstr));
    layoutDescription->setText(selLayout->authorProperty(descstr));
    
    //If settings for tiles/background have been applied, update our preview
    if (bw->theTiles.path()!=Prefs::tileSet()) 
      bw->loadTileset(Prefs::tileSet());
    if (bw->theBackground.path()!=Prefs::background())
      bw->loadBackground(Prefs::background());
          
    //Now load the boardLayout temporarily
    bw->loadBoardLayout(selLayout->path());
    bw->calculateNewGame();

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
    tilesetPreview->setPixmap(QPixmap::fromImage(qiRend));*/

}

#include "kmahjongglayoutselector.moc"
