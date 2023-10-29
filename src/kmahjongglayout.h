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

    QString name() const;
    QString description() const;
    QString authorName() const;
    QString authorEmailAddress() const;

    BoardLayout * board() const;

private:
    friend class KMahjonggLayoutPrivate;
    KMahjonggLayoutPrivate * const d;
    QString filename;

    QString m_name;
    QString m_description;
    QString m_authorName;
    QString m_authorEmailAddress;

    Q_DISABLE_COPY(KMahjonggLayout)
};

#endif
