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
#include <kstandarddirs.h>
#include <KLocalizedString>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <QFile>
#include <QMap>
#include <QDebug>

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
    static bool _inited = false;
    if (_inited)
        return;
    _inited = true;
}

KMahjonggLayout::~KMahjonggLayout() {
    delete d;
}

bool KMahjonggLayout::loadDefault()
{
    QString idx = "default.desktop";

    QString layoutPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kmahjongg/layouts/" + idx);
    qDebug() << "Inside LoadDefault(), located layout at" << layoutPath;
    if (layoutPath.isEmpty()) {
		return false;
    }
    return load(layoutPath);
}

#define kLayoutVersionFormat 1

bool KMahjonggLayout::load(const QString &file) {
    //qDebug() << "Layout loading";

    QString layoutPath;
    //qDebug() << "Attempting to load .desktop at" << file;

    // verify if it is a valid file first and if we can open it
    QFile bgfile(file);
    if (!bgfile.open(QIODevice::ReadOnly)) {
      return (false);
    }
    bgfile.close();

    KConfig bgconfig(file, KConfig::SimpleConfig);
    KConfigGroup group = bgconfig.group("KMahjonggLayout");

    d->authorproperties.insert("Name", group.readEntry("Name"));// Returns translated data
    d->authorproperties.insert("Author", group.readEntry("Author"));
    d->authorproperties.insert("Description", group.readEntry("Description"));
    d->authorproperties.insert("AuthorEmail", group.readEntry("AuthorEmail"));

    //Version control
    int bgversion = group.readEntry("VersionFormat",0);
    //Format is increased when we have incompatible changes, meaning that older clients are not able to use the remaining information safely
    if (bgversion > kLayoutVersionFormat) {
        return false;
    }

    QString layoutName = group.readEntry("FileName");

    layoutPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kmahjongg/layouts/" + layoutName);
    qDebug() << "Using layout at" << layoutPath;
    d->filename = layoutPath;

    if (layoutPath.isEmpty()) return (false);
    
    if (!d->board->loadBoardLayout(d->filename)) return (false);
    
    filename = file;

   return true;
}

BoardLayout * KMahjonggLayout::board() { 
  return d->board; 
}

QString KMahjonggLayout::path() const {
    return filename;
}

QString KMahjonggLayout::authorProperty(const QString &key) const {
    return d->authorproperties[key];
}
