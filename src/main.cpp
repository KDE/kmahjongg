/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt
#include <QApplication>
#include <QCommandLineParser>

// KF
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

// KMahjongg
#include "kmahjongg.h"
#include "kmahjongg_version.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kmahjongg"));

    KAboutData aboutData(QStringLiteral("kmahjongg"), i18n("KMahjongg"),
                         QStringLiteral(KMAHJONGG_VERSION_STRING),
                         i18n("Mahjongg Solitaire by KDE"),
                         KAboutLicense::GPL,
                         i18n("(c) 1997, Mathias Mueller\n(c) 2006, Mauricio Piacentini\n(c) 2011, Christian Krippendorf"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/kmahjongg"));
    aboutData.addAuthor(i18n("Mathias Mueller"), i18n("Original Author"), QStringLiteral("in5y158@public.uni-hamburg.de"));
    aboutData.addAuthor(i18n("Christian Krippendorf"), i18n("Current maintainer"), QStringLiteral("Coding@Christian-Krippendorf.de"));
    aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Bug fixes"), QStringLiteral("aacid@kde.org"));
    aboutData.addAuthor(i18n("David Black"), i18n("KDE 3 rewrite and Extension"), QStringLiteral("david.black@lutris.com"));
    aboutData.addAuthor(i18n("Michael Haertjens"), i18n("Solvable game generation\nbased on algorithm by Michael Meeks in GNOME mahjongg"), QStringLiteral("mhaertjens@modusoperandi.com"));
    aboutData.addCredit(i18n("Raquel Ravanini"), i18n("SVG Tileset for KDE4"), QStringLiteral("raquel@tabuleiro.com"));
    aboutData.addCredit(i18n("Richard Lohman"), i18n("Tile set contributor and current web page maintainer"), QStringLiteral("richardjlohman@yahoo.com"));
    aboutData.addCredit(i18n("Osvaldo Stark"), i18n("Tile set contributor and original web page maintainer"), QStringLiteral("starko@dnet.it"));
    aboutData.addCredit(i18n("Benjamin Meyer"), i18n("Code cleanup"), QStringLiteral("ben+kmahjongg@meyerhome.net"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kmahjongg")));

    KCrash::initialize();

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service;

    if (app.isSessionRestored()) {
        kRestoreMainWindows<KMahjongg>();
    } else {
        KMahjongg * window = new KMahjongg();
        window->show();
    }

    return app.exec();
}
