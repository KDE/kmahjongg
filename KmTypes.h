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
