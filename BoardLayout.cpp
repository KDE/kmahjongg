
#include "BoardLayout.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>





BoardLayout::BoardLayout()
{
	filename="";
	clearBoardLayout();
}

BoardLayout::~BoardLayout()
{
}

void BoardLayout::clearBoardLayout() {
	loadedBoard="";
	initialiseBoard();	
}

bool BoardLayout::saveBoardLayout(const QString where) {
	QFile f(where);
	if (!f.open(IO_ReadWrite)) {
	    return false;
	}

	QCString tmp = layoutMagic1_0.utf8();
	if (f.writeBlock(tmp, tmp.length()) == -1) {
	    return(false);	
	}

	for (int z=0; z<depth; z++) {
	    for(int y=0; y<height; y++) {
		if(f.putch('\n')==-1)
		    return(false);
		for (int x=0; x<width; x++) {
		if (board[z][y][x]) {
		    if(f.putch(board[z][y][x])==-1)
		       return false;
		} else {
		    if(f.putch('.')==-1)
			return false;
		}
		}
	    }
	}
	if(f.putch('\n')!=-1)
	    return true;
	else
	    return false;
}


bool BoardLayout::loadBoardLayout(const QString from)
{
    
    if (from == filename) {
	return true;	
    } 

    QFile f(from);
    QString all = "";

    if ( f.open(IO_ReadOnly) ) {    
 	QTextStream t( &f );
        t.setCodec(QTextCodec::codecForName("UTF-8"));
	QString s;
	s = t.readLine();
	if (s != layoutMagic1_0) {
	    f.close();
	    return(false);
	}
	int lines = 0;
	while ( !t.eof() ) {        
	    s = t.readLine();
	    if (s[0] == '#')
		continue;
	    all += s;
	    lines++;
	}
	f.close();
	if (lines == height*depth) {
	    loadedBoard = all;
	    initialiseBoard();
	    filename = from;
	    return(true);
	} else {
	    return(false);
	}
	return(true);
    } else {
	return(false);
    }
}

void BoardLayout::initialiseBoard() {

    short z=0;
    short x=0;          // Rand lassen.
    short y=0;
    maxTileNum = 0;

    const char *pos = (const char *) loadedBoard.ascii();

    memset( &board, 0, sizeof( board) );

    if (loadedBoard.isEmpty())
	return;

    // loop will be left by break or return
    while( true )
    {
        BYTE c = *pos++;

        switch( c )
        {
            case (UCHAR)'1': maxTileNum++;
            case (UCHAR)'3':
            case (UCHAR)'2':
            case (UCHAR)'4': board[z][y][x] = c;
                             break;

            default:         break;
        }
        if( ++x == width)
        {
            x=0;
            if( ++y == height)
            {
                y=0;
                if( ++z == depth)
                {
                    // number of tiles have to be even
                    if( maxTileNum & 1 ) break;
                    return;
                }
            }
        }
    }
}

void BoardLayout::copyBoardLayout(UCHAR *to , unsigned short &n){

    memcpy(to, board, sizeof(board));
    n = maxTileNum;
}

const char* BoardLayout::getBoardLayout()
{
	return loadedBoard.ascii();
}    



void BoardLayout::shiftLeft() {
    for (int z=0; z<depth; z++) {
	for (int y=0; y<height; y++) {
	    UCHAR keep=board[z][y][0];
	    if (keep == '1') {
	        // tile going off board, delete it
	        board[z][y][0]=0;
	        board[z][y][1]=0;
	        board[z][y+1][0]=0;
	        board[z][y+1][1]=0;
	    }
	    for (int x=0; x<width-1; x++) {
		board[z][y][x] = board[z][y][x+1];
	    }
	    board[z][y][width-1] = 0;
	}
    }
}


void BoardLayout::shiftRight() {
    for (int z=0; z<depth; z++) {
	for (int y=0; y<height; y++) {
	    UCHAR keep=board[z][y][width-2];
	    if (keep == '1') {
	        // tile going off board, delete it
	        board[z][y][width-2]=0;
	        board[z][y][width-1]=0;
	        board[z][y+1][width-2]=0;
	        board[z][y+1][width-1]=0;
	    }
	    for (int x=width-1; x>0; x--) {
		board[z][y][x] = board[z][y][x-1];
	    }
	    board[z][y][0] = 0;
	}
    }
}
void BoardLayout::shiftUp() {
    for (int z=0; z<depth; z++) {
	// remove tiles going off the top
	for (int x=0; x<width; x++) {
	    if (board[z][0][x] == '1') {
		board[z][0][x] = 0;
		board[z][0][x+1] = 0;
		board[z][1][x] = 0;
		board[z][1][x+1] = 0;
	    }
	}
    }
    for (int z=0; z<depth;z++) {
	for (int y=0; y<height-1; y++) {
	    for (int x=0; x<width; x++) {
		board[z][y][x]=board[z][y+1][x];
		if (y == height-2)
			board[z][y+1][x]=0;
	    }
        }
    }
}


void BoardLayout::shiftDown() {
    for (int z=0; z<depth; z++) {
	// remove tiles going off the top
	for (int x=0; x<width; x++) {
	    if (board[z][height-2][x] == '1') {
		board[z][height-2][x] = 0;
		board[z][height-2][x+1] = 0;
		board[z][height-1][x] = 0;
		board[z][height-1][x+1] = 0;
	    }
	}
    }
    for (int z=0; z<depth;z++) {
	for (int y=height-1; y>0; y--) {
	    for (int x=0; x<width; x++) {
		board[z][y][x]=board[z][y-1][x];
		if (y == 1)
			board[z][y-1][x]=0;
	    }
        }
    }
}





// is there a tile anywhere above here (top left to bot right quarter)
bool BoardLayout::tileAbove(short z, short y, short x) {
    if (z >= depth -1)
        return false;
    if( board[z+1][y][x]   || board[z+1][y+1][x] || board[z+1][y][x+1] || board[z+1][y+1][x+1] ) {
        return true;
    }
    return false;
}  


bool BoardLayout::blockedLeftOrRight(short z, short y, short x) {
     return( (board[z][y][x-1] || board[z][y+1][x-1]) &&
             (board[z][y][x+2] || board[z][y+1][x+2]) );
}

void BoardLayout::deleteTile(POSITION &p) {
    if ( p.e <depth && board[p.e][p.y][p.x] == '1') {
        board[p.e][p.y][p.x]=0;
        board[p.e][p.y][p.x+1]=0;
        board[p.e][p.y+1][p.x]=0;
        board[p.e][p.y+1][p.x+1]=0;
        maxTileNum--;
    }
} 

bool BoardLayout::anyFilled(POSITION &p) {
    return(board[p.e][ p.y][ p.x] != 0 ||
           board[p.e][ p.y][ p.x+1] != 0 ||
           board[p.e][ p.y+1][ p.x] != 0 ||
           board[p.e][ p.y+1][ p.x+1] != 0);
}
bool BoardLayout::allFilled(POSITION &p) {
    return(board[p.e][ p.y][ p.x] != 0 &&
           board[p.e][p.y][ p.x+1] != 0 &&
           board[p.e][p.y+1][ p.x] != 0 &&
           board[p.e][p.y+1][ p.x+1] != 0);
}
void BoardLayout::insertTile(POSITION &p) {
    board[p.e][p.y][p.x] = '1';
    board[p.e][p.y][p.x+1] = '2';
    board[p.e][p.y+1][p.x+1] = '3';
    board[p.e][p.y+1][p.x] = '4';
}








