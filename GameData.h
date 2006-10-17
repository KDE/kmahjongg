/*
    Copyright (C) 2006 Mauricio Piacentini  <mauricio@tabuleiro.com>

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

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "KmTypes.h"
#include <QByteArray>
#include <QVector>

class GameData {

public:
    GameData ();
    ~GameData ();

    int      allow_undo;
    int      allow_redo;

    USHORT   TileNum;
    USHORT   MaxTileNum;

    void putTile( short e, short y, short x, UCHAR f );
    void putTile( POSITION& pos ) { putTile( pos.e, pos.y, pos.x, pos.f );}
    bool tilePresent(int z, int y, int x);
    bool partTile(int z, int y, int x);

    UCHAR BoardData(short z, short y, short x);
    void setBoardData(short z, short y, short x, UCHAR value);
    UCHAR MaskData(short z, short y, short x);
    UCHAR HighlightData(short z, short y, short x);
    void setHighlightData(short z, short y, short x, UCHAR value);
    POSITION MoveListData(short i);
    void setMoveListData(short i, POSITION value);
    char * getMaskBytes(){ return Mask.data(); }

    short m_width;
    short m_height;
    short m_depth;
    short m_maxTiles;

//TODO make MoveList private?? Need to implement get/set and change parameters of several methods in boardwidget
    QVector<POSITION> MoveList;

private:
    QByteArray Board;
    QByteArray Mask;
    QByteArray Highlight;
    
};

#endif // GAMEDATA_H
