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

#ifndef BOARD_LAYOUT_H
#define BOARD_LAYOUT_H

#include <QString>
#include "KmTypes.h"

const QString layoutMagic1_0 = "kmahjongg-layout-v1.0";

class BoardLayout {

public:
    BoardLayout();
    ~BoardLayout();

    bool loadBoardLayout(const QString from);
    bool saveBoardLayout(const QString where);
    UCHAR getBoardData(short z, short y, short x);
    void setBoardData(short z, short y, short x, UCHAR value);

    // is there a tile anywhere above here (top left to bot right quarter)
    bool tileAbove(short z, short y, short x); 
    bool tileAbove(POSITION &p) { return(tileAbove(p.e, p.y, p.x));	}	

    // is this tile blocked to the left or right
    bool blockedLeftOrRight(short z, short y, short x);

    void deleteTile(POSITION &p);

    bool anyFilled(POSITION &p);
    bool allFilled(POSITION &p);
    void insertTile(POSITION &p); 
    bool isTileAt(POSITION &p) { return getBoardData(p.e, p.y, p.x) == '1'; }

    const char *getBoardLayout();
    void copyBoardLayout(UCHAR *to , unsigned short &numTiles); 
    void clearBoardLayout();
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

 
     enum { width = 32,
            height = 16,
            depth = 5 };
     enum { maxTiles = (depth*width*height)/4 };

     int m_width;
     int m_height;
     int m_depth;
     int m_maxTiles;

     QString &getFilename() {return filename;};
     
protected:

    void initialiseBoard(); 

private:
    QString filename;
    QString loadedBoard;
    UCHAR * board;
    unsigned short maxTileNum;
};

#endif

