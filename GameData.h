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
#include "BoardLayout.h"

class GameData {

public:
    GameData ();
    ~GameData ();

    int      allow_undo;
    int      allow_redo;
    UCHAR    Board[BoardLayout::depth][BoardLayout::height][BoardLayout::width];
    USHORT   TileNum;
    USHORT   MaxTileNum;

    QByteArray Mask;
    QByteArray Highlight;
    POSITION MoveList[BoardLayout::maxTiles];
    void putTile( short e, short y, short x, UCHAR f )
    {
        Board[e][y][x] = Board[e][y+1][x] =
		   Board[e][y+1][x+1] = Board[e][y][x+1] = f;
    }
    void putTile( POSITION& pos )
    {
        putTile( pos.e, pos.y, pos.x, pos.f );
    }


    bool tilePresent(int z, int y, int x) {
	if ((y<0)||(x<0)||(z<0)||(y>BoardLayout::height-1)||(x>BoardLayout::width-1)||(z>BoardLayout::depth-1)) return false;
	return(Board[z][y][x]!=0 && MaskData(z,y,x) == '1');
    }

    bool partTile(int z, int y, int x) {
	return (Board[z][y][x] != 0);
    }

    UCHAR MaskData(short z, short y, short x);
    UCHAR HighlightData(short z, short y, short x);
    void setHighlightData(short z, short y, short x, UCHAR value);

    short m_width;
    short m_height;
    short m_depth;
    short m_maxTiles;

};

#endif // GAMEDATA_H
