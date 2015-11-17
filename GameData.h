/* kmahjongg, the classic mahjongg game for KDE project
 *
 * Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
 * Copyright (C) 2006-2007 Mauricio Piacentini   <mauricio@tabuleiro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "KmTypes.h"

#include <QVector>

#include <KRandomSequence>

class BoardLayout;

/**
 * @short This class implements
 *
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com> */
class GameData
{
public:
    /**
     * Constructor
     *
     * @param boardlayout
     * @see BoardLayout */
    explicit GameData(BoardLayout *boardlayout);

    /**
     * Default Destructor */
    ~GameData();

    /**
     * Method Description
     *
     * @param e
     * @param y
     * @param x
     * @param f */
    void putTile(short e, short y, short x, UCHAR f);

    /**
     * Method Description
     *
     * @param z
     * @param y
     * @param x
     * @return @c true if ...
     * @return @c false if ... */
    bool tilePresent(int z, int y, int x) const;

    /**
     * Method Description
     *
     * @param z
     * @param y
     * @param x
     * @return UCHAR */
    UCHAR BoardData(short z, short y, short x) const;

    /**
     * Method Description
     *
     * @param z
     * @param y
     * @param x
     * @return UCHAR */
    UCHAR HighlightData(short z, short y, short x) const;

    /**
     * Method Description
     *
     * @param i
     * @return POSITION
     * @ref pos */
    POSITION& MoveListData(short i);

    /**
     * Method Description
     *
     * @param i
     * @param value
     * @ref pos */
    void setMoveListData(short i, POSITION &value);

    /**
     * Method Description
     *
     * @return *char blah blah */
    char* getMaskBytes() {return Mask.data();}

    bool saveToStream(QDataStream &out) const;
    bool loadFromStream(QDataStream &in);

    void generatePositionDepends();
    void generateTilePositions();

    bool generateStartPosition2();

    bool findMove(POSITION &posA, POSITION &posB);
    int moveCount();
    short findAllMatchingTiles(POSITION &posA);
    void setRemovedTilePair(POSITION &a, POSITION &b);
    void clearRemovedTilePair(POSITION &a, POSITION &b);
    bool isMatchingTile(POSITION &Pos1, POSITION &Pos2) const;
    void shuffle();
    POSITION& getFromPosTable(int index) {return PosTable[index];}

    int allow_undo;
    int allow_redo;

    USHORT TileNum;
    USHORT MaxTileNum;

    //Board Layout dimensions
    short m_width;
    short m_height;
    short m_depth;
    short m_maxTiles;

    KRandomSequence random;

private:
    /**
     * Method Description
     *
     * @param pos
     * @ref pos */
    void putTile(POSITION &pos) {putTile(pos.e, pos.y, pos.x, pos.f);}

    /**
     * Method Description
     *
     * @param z
     * @param y
     * @param x
     * @param value */
    void setBoardData(short z, short y, short x, UCHAR value);

    /**
     * Method Description
     *
     * @param z
     * @param y
     * @param x
     * @return UCHAR */
    UCHAR MaskData(short z, short y, short x) const;

    int tileAt(int x, int y, int z) const;
    bool generateSolvableGame();
    bool onlyFreeInLine(int position) const;
    int selectPosition(int lastPosition);
    void placeTile(int position, int tile);
    void updateDepend(int position);

    //other generation bits
    void randomiseFaces();
    void getFaces(POSITION &a, POSITION &b);

    int tilesUsed;

    UCHAR tilePair[144];

    // storage to keep track of removed tiles
    unsigned char removedCharacter[9];
    unsigned char removedBamboo[9];
    unsigned char removedRod[9];
    unsigned char removedDragon[3];
    unsigned char removedWind[9];
    unsigned char removedFlower[4];
    unsigned char removedSeason[4];

    // new bits for new game generation, with solvability. Scratch storage
    int numTilesToGenerate;

    QByteArray Board;
    QByteArray Mask;
    QByteArray Highlight;
    QVector<POSITION> MoveList;

    QVector<POSITION> tilePositions;
    QVector<DEPENDENCY> positionDepends;

    //PosTable, scratch storage used for highlighting matching tiles
    QVector<POSITION> PosTable;
};


#endif // GAMEDATA_H
