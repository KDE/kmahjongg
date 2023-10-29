/*
    SPDX-FileCopyrightText: 2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMAHJONGGLAYOUT_H
#define KMAHJONGGLAYOUT_H

// Qt
#include <QString>
// Std
#include <memory>

class BoardLayout;

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
    QString m_fileName;

    QString m_name;
    QString m_description;
    QString m_authorName;
    QString m_authorEmailAddress;

    std::unique_ptr<BoardLayout> const m_board;

    Q_DISABLE_COPY(KMahjonggLayout)
};

#endif
