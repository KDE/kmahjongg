/*
    SPDX-FileCopyrightText: 2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// own
#include "kmahjongglayout.h"

// Qt
#include <QFile>
#include <QStandardPaths>

// KF
#include <KConfig>
#include <KConfigGroup>

// KMahjongg
#include "boardlayout.h"
#include "kmahjongg_debug.h"

namespace {
constexpr int kLayoutVersionFormat = 1;
}

KMahjonggLayout::KMahjonggLayout()
    : m_board(new BoardLayout())
{
    static bool s_inited = false;
    if (s_inited) {
        return;
    }

    s_inited = true;
}

KMahjonggLayout::~KMahjonggLayout() = default;

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

bool KMahjonggLayout::load(const QString & file)
{
    // verify if it is a valid file first and if we can open it
    QFile bgfile(file);
    if (!bgfile.open(QIODevice::ReadOnly)) {
        m_name.clear();
        m_description.clear();
        m_authorName.clear();
        m_authorEmailAddress.clear();
        return false;
    }
    bgfile.close();

    KConfig bgconfig(file, KConfig::SimpleConfig);
    KConfigGroup group = bgconfig.group(QStringLiteral("KMahjonggLayout"));

    m_name = group.readEntry("Name"); // Returns translated data
    m_description = group.readEntry("Description");
    m_authorName = group.readEntry("Author");
    m_authorEmailAddress = group.readEntry("AuthorEmail");

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

    if (layoutPath.isEmpty()) {
        return false;
    }

    if (!m_board->loadBoardLayout(layoutPath)) {
        return false;
    }

    m_fileName = file;

    return true;
}

BoardLayout * KMahjonggLayout::board() const
{
    return m_board.get();
}

QString KMahjonggLayout::path() const
{
    return m_fileName;
}

QString KMahjonggLayout::name() const
{
    return m_name;
}

QString KMahjonggLayout::description() const
{
    return m_description;
}

QString KMahjonggLayout::authorName() const
{
    return m_authorName;
}

QString KMahjonggLayout::authorEmailAddress() const
{
    return m_authorEmailAddress;
}
