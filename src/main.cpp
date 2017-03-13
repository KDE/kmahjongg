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

// Qt
#include <QApplication>
#include <QCommandLineParser>

// KDE
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>

// KMahjongg
#include "kmahjongg.h"
#include "version.h"

static const char description[] = I18N_NOOP("Mahjongg Solitaire by KDE");

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kmahjongg");
    KAboutData aboutData(QStringLiteral("kmahjongg"), i18n("KMahjongg"),
                         KMAHJONGG_VERSION, i18n(description), KAboutLicense::GPL,
                         i18n("(c) 1997, Mathias Mueller\n(c) 2006, Mauricio Piacentini\n(c) 2011, Christian Krippendorf"));
    aboutData.setHomepage(QStringLiteral("http://games.kde.org/kmahjongg"));
    aboutData.addAuthor(i18n("Mathias Mueller"), i18n("Original Author"), QStringLiteral("in5y158@public.uni-hamburg.de"));
    aboutData.addAuthor(i18n("Christian Krippendorf"), i18n("Current maintainer"), QStringLiteral("Coding@Christian-Krippendorf.de"));
    aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Bug fixes"), QStringLiteral("aacid@kde.org"));
    aboutData.addAuthor(i18n("David Black"), i18n("KDE 3 rewrite and Extension"), QStringLiteral("david.black@lutris.com"));
    aboutData.addAuthor(i18n("Michael Haertjens"), i18n("Solvable game generation\nbased on algorithm by Michael Meeks in GNOME mahjongg"), QStringLiteral("mhaertjens@modusoperandi.com"));
    aboutData.addCredit(i18n("Raquel Ravanini"), i18n("SVG Tileset for KDE4"), QStringLiteral("raquel@tabuleiro.com"));
    aboutData.addCredit(i18n("Richard Lohman"), i18n("Tile set contributor and current web page maintainer"), QStringLiteral("richardjlohman@yahoo.com"));
    aboutData.addCredit(i18n("Osvaldo Stark"), i18n("Tile set contributor and original web page maintainer"), QStringLiteral("starko@dnet.it"));
    aboutData.addCredit(i18n("Benjamin Meyer"), i18n("Code cleanup"), QStringLiteral("ben+kmahjongg@meyerhome.net"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // Migrate pre-existing (4.x) configuration
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kmahjongg"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kmahjonggrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kmahjonggui.rc"));
    migrate.migrate();

    KDBusService service;

    if (app.isSessionRestored()) {
        RESTORE(KMahjongg)
    } else {
        KMahjongg * window = new KMahjongg();
        window->show();
    }

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kmahjongg")));

    return app.exec();
}
