/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

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

#define TILE_OFFSET 2
#define TILE_CHARACTER (0 + TILE_OFFSET)
#define TILE_BAMBOO (9 + TILE_OFFSET)
#define TILE_ROD (18 + TILE_OFFSET)
#define TILE_SEASON (27 + TILE_OFFSET)
#define TILE_WIND (31 + TILE_OFFSET)
#define TILE_DRAGON (35 + TILE_OFFSET)
#define TILE_FLOWER (38 + TILE_OFFSET)

#endif
