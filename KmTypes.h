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

#ifndef _KM_TYPES_
#define _KM_TYPES_

//----------------------------------------------------------
// TYPEDEFS
//----------------------------------------------------------
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;


typedef struct pos {
    pos() : e(0), y(0), x(0), f(0) { }
    USHORT e,y,x,f;
} POSITION;

typedef struct dep {
    int turn_dep[4];   // Turn dependencies
    int place_dep[4];  // Placing dependencies
    int lhs_dep[2];    // Left side dependencies, same level
    int rhs_dep[2];    // Right side dependencies, same level
    bool filled;       // True if this tile has been placed.
    bool free;         // True if this tile can be removed?
} DEPENDENCY;

#endif
