/*
    SPDX-FileCopyrightText: 2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMAHJONGGLAYOUT_H
#define KMAHJONGGLAYOUT_H

// Qt
#include <QString>

class BoardLayout;
class KMahjonggLayoutPrivate;

class KMahjonggLayout
{
public:
    KMahjonggLayout();
    ~KMahjonggLayout();

    bool load(const QString & file);
    bool loadDefault();
    QString path() const;
    QString authorProperty(const QString & key) const;
    BoardLayout * board() const;
    QString layoutName() const;

private:
    friend class KMahjonggLayoutPrivate;
    KMahjonggLayoutPrivate * const d;
    QString filename;
    QString m_layoutName;

    Q_DISABLE_COPY(KMahjonggLayout)
};

#endif
