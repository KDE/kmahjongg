/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>
    SPDX-FileCopyrightText: 2006-2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMEDATA_H
#define GAMEDATA_H

// Qt
#include <QRandomGenerator>
#include <QVector>

// KMahjongg
#include "kmtypes.h"

class BoardLayout;

/**
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com> */
class GameData
{
public:
    explicit GameData(BoardLayout * boardlayout);
    ~GameData();

    void putTile(short e, short y, short x, UCHAR f);

    bool tilePresent(int z, int y, int x) const;

    UCHAR BoardData(short z, short y, short x) const;

    UCHAR HighlightData(short z, short y, short x) const;

    POSITION & MoveListData(short i);

    void setMoveListData(short i, POSITION & value);

    char * getMaskBytes()
    {
        return m_mask.data();
    }

    bool saveToStream(QDataStream & out) const;
    bool loadFromStream(QDataStream & in);

    void generatePositionDepends();
    void generateTilePositions();

    bool generateStartPosition2();

    bool findMove(POSITION & posA, POSITION & posB);
    int moveCount();
    short findAllMatchingTiles(POSITION & posA);
    void setRemovedTilePair(POSITION & a, POSITION & b);
    void clearRemovedTilePair(POSITION & a, POSITION & b);
    bool isMatchingTile(POSITION & pos1, POSITION & pos2) const;
    void shuffle();
    POSITION & getFromPosTable(int index)
    {
        return m_posTable[index];
    }

    int m_allowUndo;
    int m_allowRedo;

    USHORT m_tileNum;
    USHORT m_maxTileNum;

    //Board Layout dimensions
    short m_width;
    short m_height;
    short m_depth;
    short m_maxTiles;

    QRandomGenerator random;

private:
    void putTile(POSITION & pos)
    {
        putTile(pos.z, pos.y, pos.x, pos.f);
    }
    void setBoardData(short z, short y, short x, UCHAR value);
    UCHAR MaskData(short z, short y, short x) const;

    int tileAt(int x, int y, int z) const;
    bool generateSolvableGame();
    bool onlyFreeInLine(int position) const;
    int selectPosition(int lastPosition);
    void placeTile(int position, int tile);
    void updateDepend(int position);

    //other generation bits
    void randomiseFaces();
    void getFaces(POSITION & a, POSITION & b);

    int m_tilesUsed;

    UCHAR m_tilePair[144];

    // storage to keep track of removed tiles
    unsigned char m_removedCharacter[9];
    unsigned char m_removedBamboo[9];
    unsigned char m_removedRod[9];
    unsigned char m_removedDragon[3];
    unsigned char m_removedWind[9];
    unsigned char m_removedFlower[4];
    unsigned char m_removedSeason[4];

    // new bits for new game generation, with solvability. Scratch storage
    int m_numTilesToGenerate;

    QByteArray m_board;
    QByteArray m_mask;
    QByteArray m_highlight;
    QVector<POSITION> m_moveList;

    QVector<POSITION> m_tilePositions;
    QVector<DEPENDENCY> m_positionDepends;

    //PosTable, scratch storage used for highlighting matching tiles
    QVector<POSITION> m_posTable;
};

#endif // GAMEDATA_H
