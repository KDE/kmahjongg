/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

    Kmahjongg is free software; you can redistribute it and/or modify
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
#include "version.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kimageio.h>
#include <klocale.h>
#include <kglobal.h>

static const char description[] = I18N_NOOP("Mahjongg Solitaire for KDE");

int main( int argc, char** argv )
{
    KAboutData aboutData( "kmahjongg", 0, ki18n("KMahjongg"),
      KMAHJONGG_VERSION, ki18n(description), KAboutData::License_GPL,
      ki18n("(c) 1997, Mathias Mueller\n(c) 2006, Mauricio Piacentini"), KLocalizedString(), "http://games.kde.org/kmahjongg" );
    aboutData.addAuthor(ki18n("Mathias Mueller"), ki18n("Original Author"), "in5y158@public.uni-hamburg.de");
    aboutData.addAuthor(ki18n("Mauricio Piacentini"), ki18n("Current maintainer"), "mauricio@tabuleiro.com");
    aboutData.addAuthor(ki18n("Albert Astals Cid"), ki18n("Bug fixes"), "aacid@kde.org");
    aboutData.addAuthor(ki18n("David Black"), ki18n("KDE 3 rewrite and Extension"), "david.black@lutris.com");
    aboutData.addAuthor(ki18n("Michael Haertjens"), ki18n("Solvable game generation\nbased on algorithm by Michael Meeks in GNOME mahjongg"), "mhaertjens@modusoperandi.com");
    aboutData.addCredit(ki18n("Raquel Ravanini"), ki18n("SVG Tileset for KDE4"), "raquel@tabuleiro.com");
    aboutData.addCredit(ki18n("Richard Lohman"), ki18n("Tile set contributor and current web page maintainer"),"richardjlohman@yahoo.com");
    aboutData.addCredit(ki18n("Osvaldo Stark"), ki18n("Tile set contributor and original web page maintainer"), "starko@dnet.it");
    aboutData.addCredit(ki18n("Benjamin Meyer"), ki18n("Code cleanup"), "ben+kmahjongg@meyerhome.net");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication application;
    KGlobal::locale()->insertCatalog("libkdegames");
    KGlobal::locale()->insertCatalog("libkmahjongg");
    

    if (application.isSessionRestored())
        RESTORE(KMahjongg)
    else {
        KMahjongg *window = new KMahjongg;
        window->show();
    }
    return application.exec();
}

