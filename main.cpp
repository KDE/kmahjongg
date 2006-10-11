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

static const char description[] = I18N_NOOP("Mahjongg for KDE");

int main( int argc, char** argv )
{
    KAboutData aboutData( "kmahjongg", I18N_NOOP("KMahjongg"),
      KMAHJONGG_VERSION, description, KAboutData::License_GPL,
      "(c) 1997, Mathias Mueller");
    aboutData.addAuthor("Mathias Mueller", I18N_NOOP("Original Author"), "in5y158@public.uni-hamburg.de");
    aboutData.addAuthor("Mauricio Piacentini", I18N_NOOP("Current maintainer"), "mauricio@tabuleiro.com");
    aboutData.addAuthor("Albert Astals Cid", I18N_NOOP("Bug fixes"), "astals11@terra.es");
    aboutData.addAuthor("David Black", I18N_NOOP("KDE 3 rewrite and Extension"), "david.black@lutris.com");
    aboutData.addAuthor("Michael Haertjens", I18N_NOOP("Solvable game generation\nbased on algorithm by Michael Meeks in GNOME mahjongg"), "mhaertjens@modusoperandi.com");
    aboutData.addCredit("Raquel Ravanini", I18N_NOOP("SVG Tileset for KDE4"), "raquel@tabuleiro.com");
    aboutData.addCredit("Richard Lohman", I18N_NOOP("Tile set contributor and current web page maintainer"),"richardjlohman@yahoo.com");
    aboutData.addCredit("Osvaldo Stark", I18N_NOOP("Tile set contributor and original web page maintainer"), "starko@dnet.it");
    aboutData.addCredit("Benjamin Meyer", I18N_NOOP("Code cleanup"), "ben+kmahjongg@meyerhome.net");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication application;
    KGlobal::locale()->insertCatalog("libkdegames");
    

    if (application.isSessionRestored())
        RESTORE(KMahjongg)
    else {
        KMahjongg *window = new KMahjongg;
        window->show();
    }
    return application.exec();
}

