/*
    kmahjongg, the classic mahjongg game for KDE
    Copyright (C) 2007 Mauricio Piacentini   <mauricio@tabuleiro.com>

    KMahjongg is free software; you can redistribute it and/or modify
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

#ifndef _KMAHJONGGLAYOUT_H_
#define _KMAHJONGGLAYOUT_H_

#include <QString>

class BoardLayout;
class KMahjonggLayoutPrivate;

class KMahjonggLayout
{
public:
    KMahjonggLayout();
    ~KMahjonggLayout();

    bool load(const QString &file);
    bool loadDefault();
    QString path() const;
    QString authorProperty(const QString &key) const;
    BoardLayout * board() const;

private:
   friend class KMahjonggLayoutPrivate;
   KMahjonggLayoutPrivate *const d;
   QString filename;
   static const int kLayoutVersionFormat;

   Q_DISABLE_COPY(KMahjonggLayout)
};

#endif
