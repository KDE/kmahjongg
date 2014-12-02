/* Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
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
#include "version.h"



#include <kimageio.h>
#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <Kdelibs4ConfigMigrator>

static const char description[] = I18N_NOOP("Mahjongg Solitaire for KDE");

int main(int argc, char** argv)
{
    KAboutData aboutData("kmahjongg", i18n("KMahjongg"), KMAHJONGG_VERSION, i18n(description),
        KAboutLicense::GPL, i18n("(c) 1997, Mathias Mueller\n(c) 2006, Mauricio Piacentini\n("
        "c) 2011, Christian Krippendorf"));
    aboutData.setHomepage("http://games.kde.org/kmahjongg");
    aboutData.addAuthor(i18n("Mathias Mueller"), i18n("Original Author"), "in5y158@public.uni-ham"
        "burg.de");
    aboutData.addAuthor(i18n("Christian Krippendorf"), i18n("Current maintainer"), "Coding@Christ"
        "ian-Krippendorf.de");
    aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Bug fixes"), "aacid@kde.org");
    aboutData.addAuthor(i18n("David Black"), i18n("KDE 3 rewrite and Extension"), "david.black@lu"
        "tris.com");
    aboutData.addAuthor(i18n("Michael Haertjens"), i18n("Solvable game generation\nbased on algor"
        "ithm by Michael Meeks in GNOME mahjongg"), "mhaertjens@modusoperandi.com");
    aboutData.addCredit(i18n("Raquel Ravanini"), i18n("SVG Tileset for KDE4"), "raquel@tabuleiro."
        "com");
    aboutData.addCredit(i18n("Richard Lohman"), i18n("Tile set contributor and current web page m"
        "aintainer"),"richardjlohman@yahoo.com");
    aboutData.addCredit(i18n("Osvaldo Stark"), i18n("Tile set contributor and original web page m"
        "aintainer"), "starko@dnet.it");
    aboutData.addCredit(i18n("Benjamin Meyer"), i18n("Code cleanup"), "ben+kmahjongg@meyerhome.ne"
        "t");

    QApplication app(argc, argv);
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // Migrate pre-existing (4.x) configuration
    QStringList configFiles;
    configFiles.append(QLatin1String("kmahjonggrc"));
    configFiles.append(QLatin1String("konversation.notifyrc"));

    Kdelibs4ConfigMigrator migrate(QLatin1String("kmahjongg"));
    migrate.setConfigFiles(configFiles);
    migrate.setUiFiles(QStringList() << QLatin1String("kmahjonggui.rc"));
    migrate.migrate();


    if (app.isSessionRestored()) {
        RESTORE(KMahjongg)
    } else {
        KMahjongg *window = new KMahjongg();
        window->show();
    }

    return app.exec();
}
