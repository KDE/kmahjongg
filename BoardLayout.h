#ifndef __BOARD__LAYOUT_
#define __BOARD__LAYOUT_

#include <qstring.h>
#include "KmTypes.h"

const QString layoutMagic1_0 = "kmahjongg-layout-v1.0";

class BoardLayout {

public:
    BoardLayout();
    ~BoardLayout();

    bool loadBoardLayout(const QString from);
    bool saveBoardLayout(const QString where);
    UCHAR getBoardData(short z, short y, short x) {return board[z][y][x];};

    // is there a tile anywhere above here (top left to bot right quarter)
    bool tileAbove(short z, short y, short x); 
    bool tileAbove(POSITION &p) { return(tileAbove(p.e, p.y, p.x));	}	

    // is this tile blocked to the left or right
    bool blockedLeftOrRight(short z, short y, short x);

    void deleteTile(POSITION &p);

    bool anyFilled(POSITION &p);
    bool allFilled(POSITION &p);
    void insertTile(POSITION &p); 
    bool isTileAt(POSITION &p) { return board[p.e][p.y][p.x] == '1'; }
  


    const char *getBoardLayout(void);
    void copyBoardLayout(UCHAR *to , unsigned short &numTiles); 
    void clearBoardLayout(void);
    void shiftLeft(void);
    void shiftRight(void);
    void shiftUp(void);
    void shiftDown(void);

 
     static const int width = 32;
     static const int height = 16;
     static const int depth = 5;
     static const int maxTiles = (depth*width*height)/4;

     QString &getFilename(void) {return filename;};
     
protected:

    void initialiseBoard(void); 

private:
    QString filename;
    QString loadedBoard;
    UCHAR board[depth][height][width];
    unsigned short maxTileNum;
};

#endif
