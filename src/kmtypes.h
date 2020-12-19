/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMTYPES_H
#define KMTYPES_H

//----------------------------------------------------------
// TYPEDEFS
//----------------------------------------------------------
typedef unsigned char UCHAR;
typedef unsigned char BYTE;
typedef unsigned short USHORT;

/**
 * @short struct pos POSITION
 */
typedef struct pos
{
    pos()
        : z(0)
        , y(0)
        , x(0)
        , f(0)
    {
    }
    USHORT z;
    USHORT y;
    USHORT x;
    USHORT f; /**< face id of the tile */
} POSITION;

/**
 * @short struct dep DEPENDENCY
 */
typedef struct dep
{
    int turn_dep[4]; /**< Turn dependencies */
    int place_dep[4]; /**< Placing dependencies */
    int lhs_dep[2]; /**< Left side dependencies, same level */
    int rhs_dep[2]; /**< Right side dependencies, same level */
    bool filled; /**< True if this tile has been placed. */
    bool free; /**< True if this tile can be removed? */
} DEPENDENCY;

/**
 * @short Tile angles for face composition
 */
enum TileViewAngle {
    NW, /**< North West */
    NE, /**< North East */
    SE, /**< South East */
    SW /**< South West */
};

constexpr int TILE_OFFSET = 2;
constexpr int TILE_CHARACTER = 0 + TILE_OFFSET;
constexpr int TILE_BAMBOO = 9 + TILE_OFFSET;
constexpr int TILE_ROD = 18 + TILE_OFFSET;
constexpr int TILE_SEASON = 27 + TILE_OFFSET;
constexpr int TILE_WIND = 31 + TILE_OFFSET;
constexpr int TILE_DRAGON = 35 + TILE_OFFSET;
constexpr int TILE_FLOWER = 38 + TILE_OFFSET;

#endif
