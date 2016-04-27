/*
    kmahjongg, the classic mahjongg game for KDE
    Copyright (C) 2007 Mauricio Piacentini   <mauricio@tabuleiro.com>

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

#include "kmahjongglayout.h"
#include "BoardLayout.h"
#include "kmahjongg_debug.h"

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <QFile>
#include <QMap>
#include <QStandardPaths>

const int KMahjonggLayout::kLayoutVersionFormat = 1;

class KMahjonggLayoutPrivate
{
public:
    KMahjonggLayoutPrivate()
    {
        board = new BoardLayout();
    }
    ~KMahjonggLayoutPrivate()
    {
        delete board;
    }

    BoardLayout * board;
    QMap<QString, QString> authorproperties;
    QString filename;
};

KMahjonggLayout::KMahjonggLayout()
    : d(new KMahjonggLayoutPrivate)
{
    static bool s_inited = false;
    if (s_inited) {
        return;
    }

    s_inited = true;
}

KMahjonggLayout::~KMahjonggLayout()
{
    delete d;
}

bool KMahjonggLayout::loadDefault()
{
    const QString subdir = QStringLiteral("/layouts/");
    const QString layoutFileName = QStringLiteral("default.desktop");

    const QString layoutPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, subdir + layoutFileName);
    qCDebug(KMAHJONGG_LOG) << "Inside LoadDefault(), located layout at " << layoutPath;
    if (layoutPath.isEmpty()) {
        return false;
    }
    return load(layoutPath);
}

bool KMahjonggLayout::load(const QString &file) {
    // verify if it is a valid file first and if we can open it
    QFile bgfile(file);
    if (!bgfile.open(QIODevice::ReadOnly)) {
        return false;
    }
    bgfile.close();

    KConfig bgconfig(file, KConfig::SimpleConfig);
    KConfigGroup group = bgconfig.group("KMahjonggLayout");

    d->authorproperties.insert(QStringLiteral("Name"), group.readEntry("Name")); // Returns translated data
    d->authorproperties.insert(QStringLiteral("Author"), group.readEntry("Author"));
    d->authorproperties.insert(QStringLiteral("Description"), group.readEntry("Description"));
    d->authorproperties.insert(QStringLiteral("AuthorEmail"), group.readEntry("AuthorEmail"));

    // Version control
    const int bgversion = group.readEntry("VersionFormat", 0);
    // Format is increased when we have incompatible changes, meaning that older clients
    // are not able to use the remaining information safely
    if (bgversion > kLayoutVersionFormat) {
        return false;
    }

    const QString subdir = QStringLiteral("/layouts/");
    const QString layoutFileName = group.readEntry("FileName");

    const QString layoutPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, subdir + layoutFileName);
    qCDebug(KMAHJONGG_LOG) << "Using layout at" << layoutPath;
    d->filename = layoutPath;

    if (layoutPath.isEmpty()) {
        return false;
    }

    if (!d->board->loadBoardLayout(d->filename)) {
        return false;
    }

    filename = file;

    m_layoutName = group.readEntry("Name");

    return true;
}

BoardLayout * KMahjonggLayout::board() const
{
    return d->board;
}

QString KMahjonggLayout::path() const
{
    return filename;
}

QString KMahjonggLayout::authorProperty(const QString &key) const
{
    return d->authorproperties[key];
}

QString KMahjonggLayout::layoutName() const
{
    return m_layoutName;
}
