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
/*#define TILE_OFFSET      2

#define TILE_CHARACTER  ( 0 + TILE_OFFSET)
#define TILE_BAMBOO     ( 9 + TILE_OFFSET)
#define TILE_ROD        (18 + TILE_OFFSET)
#define TILE_SEASON     (27 + TILE_OFFSET)
#define TILE_WIND       (31 + TILE_OFFSET)
#define TILE_DRAGON     (36 + TILE_OFFSET)
#define TILE_FLOWER     (39 + TILE_OFFSET)*/

/**
* @short This class implements
* 
* longer description
*
* @author Mauricio Piacentini  <mauricio@tabuleiro.com>
*/
class GameData {

public:
    /**
     * Constructor
     * @param boardlayout 
     * @see BoardLayout
     */
    explicit GameData (BoardLayout * boardlayout);
    /**
     * Default Destructor */
    ~GameData ();

    int      allow_undo;/**< Member Description */
    int      allow_redo;/**< Member Description */

    USHORT   TileNum;   /**< Member Description */
    USHORT   MaxTileNum;/**< Member Description */

    /**
     * Method Description

       @param e
       @param y
       @param x
       @param f
     */
    void putTile( short e, short y, short x, UCHAR f );
    /**
     * Method Description @param pos @ref pos */
    void putTile( POSITION& pos ) { putTile( pos.e, pos.y, pos.x, pos.f );}
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @return @c true if ...
     * @return @c false if ...
     */
    bool tilePresent(int z, int y, int x);
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @return @c true if ...
     * @return @c false if ...
     */
    bool partTile(int z, int y, int x);

    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @return UCHAR
     */
    UCHAR BoardData(short z, short y, short x);
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @param value
     */
    void setBoardData(short z, short y, short x, UCHAR value);
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @return UCHAR
     */
    UCHAR MaskData(short z, short y, short x);
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @return UCHAR
     */
    UCHAR HighlightData(short z, short y, short x);
    /**
     * Method Description
     * @param z
     * @param y
     * @param x
     * @param value
     */
    void setHighlightData(short z, short y, short x, UCHAR value);
    /**
     * Method Description
     * @param i 
     * @return POSITION @ref pos
     */
    POSITION& MoveListData(short i);
    /**
     * Method Description
     * @param i 
     * @param value @ref pos
     */
    void setMoveListData(short i, POSITION& value);
    /**
     * Method Description
     * 
     * @return *char blah blah
     */
    char * getMaskBytes(){ return Mask.data(); }

//Board Layout dimensions
    short m_width; /**< Board width */
    short m_height;/**< Board height */
    short m_depth; /**< Board depth */
    short m_maxTiles;/**< maxTiles on Board */

    // new bits for new game generation, with solvability
    int numTilesToGenerate; /**< Member Description */
    QVector<POSITION> tilePositions; /**< Member Description */
    QVector<DEPENDENCY> positionDepends; /**< Member Description */

    /**
     * Method Description */
    void generateTilePositions();
    /**
     * Method Description */
    void generatePositionDepends();
    /**
     * Method Description 
       @param x
       @param y
       @param z
       @return int
     */
    int tileAt(int x, int y, int z);
    /**
     * Method Description
     *
     * @return @c true if ...
     * @return @c false if ...
     */
    bool generateSolvableGame();
    /**
     * Method Description
     * @param position
     * @return @c true if ...
     * @return @c false if ...
     */
    bool onlyFreeInLine(int position);
    /**
     * Method Description
     * @param lastPosition
     * @return int blah blah
     */
    int selectPosition(int lastPosition);
    /**
     * Method Description
     * @param position
     * @param tile
     */
    void placeTile(int position, int tile);
    /**
     * Method Description
     * @param position
     */
    void updateDepend(int position);

    //other generation bits
    /**
     * Method Description
     *
     * @return @c true if ...
     * @return @c false if ...
     */
    bool generateStartPosition2();
    KRandomSequence random; /**< Member Description */

    // and more bits for game generation
    /**
     * Method Description */
    void randomiseFaces();
    int tilesAllocated; /**< Member Description */
    int tilesUsed;      /**< Member Description */
    
    /**
     * Method Description
     * @param a @ref pos
     * @param b @ref pos
     */
    void getFaces(POSITION &a, POSITION &b);
    UCHAR tilePair[144];/**< Member Description */

    //postable
    QVector<POSITION> PosTable;  /**< Table of all possible positions */
    /**
     * Method Description
     * @param posA
     * @param posB
     * @return @c true if ...
     * @return @c false if ...
     */
    bool findMove( POSITION& posA, POSITION& posB );
    /**
     * Method Description @return int */
    int  moveCount( );
    /**
     * Method Description
     @param posA @ref pos
     @return short
     */
    short findAllMatchingTiles( POSITION& posA);

    /**
     * Method Description */
    void initialiseRemovedTiles();
    /**
     * Method Description 
       @param a
       @param b
     */
    void setRemovedTilePair(POSITION &a, POSITION &b);
    /**
     * Method Description 
       @param a
       @param b
     */
    void clearRemovedTilePair(POSITION &a, POSITION &b);

    // storage to keep track of removed tiles
    unsigned char removedCharacter[9];  /**< Member Description */
    unsigned char removedBamboo[9];     /**< Member Description */
    unsigned char removedRod[9];        /**< Member Description */
    unsigned char removedDragon[3];     /**< Member Description */
    unsigned char removedWind[9];       /**< Member Description */
    unsigned char removedFlower[4];     /**< Member Description */
    unsigned char removedSeason[4];     /**< Member Description */

    /**
     * Method Description
     * @param Pos1
     * @param Pos2
     * @return @c true if ...
     * @return @c false if ...
     */
    bool isMatchingTile( POSITION& Pos1, POSITION& Pos2 );

private:
    QByteArray Board;
    QByteArray Mask;
    QByteArray Highlight;
    QVector<POSITION> MoveList;

};

#endif // GAMEDATA_H
