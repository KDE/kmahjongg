/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
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
#include <krandomsequence.h>
#include <QByteArray>
#include <QVector>
#include "BoardLayout.h"

// tiles symbol names:
#define TILE_OFFSET      2

#define TILE_CHARACTER  ( 0 + TILE_OFFSET)
#define TILE_BAMBOO     ( 9 + TILE_OFFSET)
#define TILE_ROD        (18 + TILE_OFFSET)
#define TILE_SEASON     (27 + TILE_OFFSET)
#define TILE_WIND       (31 + TILE_OFFSET)
#define TILE_DRAGON     (36 + TILE_OFFSET)
#define TILE_FLOWER     (39 + TILE_OFFSET)

class GameData {

public:
    GameData (BoardLayout * boardlayout);
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
    POSITION& MoveListData(short i);
    void setMoveListData(short i, POSITION& value);
    char * getMaskBytes(){ return Mask.data(); }

//Board Layout dimensions
    short m_width;
    short m_height;
    short m_depth;
    short m_maxTiles;

    // new bits for new game generation, with solvability
    int numTilesToGenerate;
    QVector<POSITION> tilePositions;
    QVector<DEPENDENCY> positionDepends;
    void generateTilePositions();
    void generatePositionDepends();
    int tileAt(int x, int y, int z);
    bool generateSolvableGame();
    bool onlyFreeInLine(int position);
    int selectPosition(int lastPosition);
    void placeTile(int position, int tile);
    void updateDepend(int position);

    //other generation bits
    bool generateStartPosition2();
    KRandomSequence random;

    // and more bits for game generation
    void randomiseFaces();
    int tilesAllocated;
    int tilesUsed;
    void getFaces(POSITION &a, POSITION &b);
    UCHAR tilePair[144];

    //postable
    QVector<POSITION> PosTable;   // Table of all possible positions

    void initialiseRemovedTiles();
    void setRemovedTilePair(POSITION &a, POSITION &b);
    void clearRemovedTilePair(POSITION &a, POSITION &b);

    // storage to keep track of removed tiles
    unsigned char removedCharacter[9];
    unsigned char removedBamboo[9];
    unsigned char removedRod[9];
    unsigned char removedDragon[3];
    unsigned char removedWind[9];
    unsigned char removedFlower[4];
    unsigned char removedSeason[4];

    bool isMatchingTile( POSITION&, POSITION& );

private:
    QByteArray Board;
    QByteArray Mask;
    QByteArray Highlight;
    QVector<POSITION> MoveList;

};

#endif // GAMEDATA_H
