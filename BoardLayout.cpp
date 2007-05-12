/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>

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

#include "BoardLayout.h"
#include <QFile>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <QtDebug>

BoardLayout::BoardLayout()
{
	filename="";
	m_width = 32;
	m_height = 16;
	m_depth = 5;
	board = QByteArray(m_width*m_height*m_depth, 0);
	clearBoardLayout();
}

BoardLayout::~BoardLayout()
{
}

void BoardLayout::clearBoardLayout() {
	loadedBoard="";
	initialiseBoard();	
}

bool BoardLayout::saveBoardLayout(const QString &where) {
	QFile f(where);
	if (!f.open(QIODevice::ReadWrite)) {
	    return false;
	}

	QByteArray tmp = layoutMagic1_1.toUtf8();
	if (f.write(tmp, tmp.length()) == -1) {
	    return(false);	
	}

	tmp = QString("\nw%1").arg(m_width).toUtf8();
	if (f.write(tmp, tmp.length()) == -1) {
	    return(false);	
	}

	tmp = QString("\nh%1").arg(m_height).toUtf8();
	if (f.write(tmp, tmp.length()) == -1) {
	    return(false);	
	}

	tmp = QString("\nd%1").arg(m_depth).toUtf8();
	if (f.write(tmp, tmp.length()) == -1) {
	    return(false);	
	}

	for (int z=0; z<m_depth; z++) {
	    for(int y=0; y<m_height; y++) {
		if(!f.putChar('\n'))
		    return(false);
		for (int x=0; x<m_width; x++) {
		if (getBoardData(z,y,x)) {
		    if(!f.putChar(getBoardData(z,y,x)))
		       return false;
		} else {
		    if(!f.putChar('.'))
			return false;
		}
		}
	    }
	}
        return f.putChar('\n');
}

bool BoardLayout::loadBoardLayout_10(const QString &from)
{
    if (from == filename) {
	return true;	
    } 

    QFile f(from);
    QString all = "";

    if ( f.open(QIODevice::ReadOnly) ) {    
 	QTextStream t( &f );
        t.setCodec(QTextCodec::codecForName("UTF-8"));
	QString s;
	s = t.readLine();
	if (s != layoutMagic1_0) {
	    f.close();
	    return(false);
	}
	//version 1.0 layouts used hardcoded board dimensions
	m_width = 32;
	m_height = 16;
	m_depth = 5;
	int lines = 0;
	while ( !t.atEnd() ) {        
	    s = t.readLine();
	    if (s[0] == '#')
		continue;
	    all += s;
	    lines++;
	}
	f.close();
	if (lines == m_height*m_depth) {
	    loadedBoard = all.toLatin1();
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

bool BoardLayout::loadBoardLayout(const QString &from)
{
    if (from == filename) {
	return true;	
    } 

    QFile f(from);
    QString all = "";
    if ( f.open(QIODevice::ReadOnly) ) {    
 	QTextStream t( &f );
        t.setCodec(QTextCodec::codecForName("UTF-8"));
	QString s;
	s = t.readLine();
	if (s != layoutMagic1_1) {
	    f.close();
	    //maybe a version 1_0 layout?
	    return(loadBoardLayout_10(from));
	}
	int lines = 0;
	m_width = m_height = m_depth = 0;
	while ( !t.atEnd() ) {        
	    s = t.readLine();
	    if (s[0] == '#')
		continue;
	    if (s[0] == 'w') {
		m_width = s.mid(1).toInt();
		continue;
	    }
	    if (s[0] == 'h') {
		m_height = s.mid(1).toInt();
		continue;
	    }
	    if (s[0] == 'd') {
		m_depth = s.mid(1).toInt();
		continue;
	    }
	    all += s;
	    lines++;
	}
	f.close();
	if ((lines == m_height*m_depth)&&(m_width>0)&&(m_height>0)&&(m_depth>0)) {
	    loadedBoard = all.toLatin1();
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
    short x=0;
    short y=0;
    maxTileNum = 0;

    if (loadedBoard.isEmpty())
	return;

    m_maxTiles = (m_width*m_height*m_depth)/4;
    board.resize(m_width*m_height*m_depth);
    board.fill(0);

    int idx = 0;
    // loop will be left by break or return
    while( true )
    {
        BYTE c = loadedBoard.at(idx++);
        switch( c )
        {
            case (UCHAR)'1': maxTileNum++;
            case (UCHAR)'3':
            case (UCHAR)'2':
            case (UCHAR)'4': setBoardData(z,y,x,c);
                             break;

            default: setBoardData(z,y,x,0); 
		break;
        }
        if( ++x == m_width)
        {
            x=0;
            if( ++y == m_height)
            {
                y=0;
                if( ++z == m_depth)
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
    memcpy(to, board.data(), m_width*m_height*m_depth);
    n = maxTileNum;
}

void BoardLayout::shiftLeft() {
    for (int z=0; z<m_depth; z++) {
	for (int y=0; y<m_height; y++) {
	    UCHAR keep=getBoardData(z,y,0);
	    if (keep == '1') {
	        // tile going off board, delete it
	        setBoardData(z,y,0,0);
	        setBoardData(z,y,1,0);
		if (y<m_height-1) {
	           setBoardData(z,y+1,0,0);
	           setBoardData(z,y+1,1,0);
		}
	    }
	    for (int x=0; x<m_width-1; x++) {
		setBoardData(z,y,x,getBoardData(z,y,x+1));
	    }
	    setBoardData(z,y,m_width-1,0);
	}
    }
}


void BoardLayout::shiftRight() {
    for (int z=0; z<m_depth; z++) {
	for (int y=0; y<m_height; y++) {
	    UCHAR keep=getBoardData(z,y,m_width-2);
	    if (keep == '1') {
	        // tile going off board, delete it
	        setBoardData(z,y,m_width-2,0);
	        setBoardData(z,y,m_width-1,0);
		if (y < m_height-1) {
		        setBoardData(z,y+1,m_width-2,0);
		        setBoardData(z,y+1,m_width-1,0);
		}
	    }
	    for (int x=m_width-1; x>0; x--) {
		setBoardData(z,y,x,getBoardData(z,y,x-1));
	    }
	    setBoardData(z,y,0,0);
	}
    }
}
void BoardLayout::shiftUp() {
    for (int z=0; z<m_depth; z++) {
	// remove tiles going off the top
	for (int x=0; x<m_width; x++) {
	    if (getBoardData(z,0,x) == '1') {
		setBoardData(z,0,x,0);
		if (x<m_width-1) {
			setBoardData(z,0,x+1,0);
			setBoardData(z,1,x+1,0);
		}
		setBoardData(z,1,x,0);
	    }
	}
    }
    for (int z=0; z<m_depth;z++) {
	for (int y=0; y<m_height-1; y++) {
	    for (int x=0; x<m_width; x++) {
		setBoardData(z,y,x,getBoardData(z,y+1,x));
		if (y == m_height-2)
			setBoardData(z,y+1,x,0);
	    }
        }
    }
}


void BoardLayout::shiftDown() {
    for (int z=0; z<m_depth; z++) {
	// remove tiles going off the top
	for (int x=0; x<m_width; x++) {
	    if (getBoardData(z,m_height-2,x) == '1') {
		setBoardData(z,m_height-2,x,0);
		if (x<m_width-1) {
			setBoardData(z,m_height-2,x+1,0);
			setBoardData(z,m_height-1,x+1,0);
		}
		setBoardData(z,m_height-1,x,0);
	    }
	}
    }
    for (int z=0; z<m_depth;z++) {
	for (int y=m_height-1; y>0; y--) {
	    for (int x=0; x<m_width; x++) {
		setBoardData(z,y,x,getBoardData(z,y-1,x));
		if (y == 1)
			setBoardData(z,y-1,x,0);
	    }
        }
    }
}


// is there a tile anywhere above here (top left to bot right quarter)
bool BoardLayout::tileAbove(short z, short y, short x) {
    if (z >= m_depth -1)
        return false;
    if( getBoardData(z+1,y,x)   || getBoardData(z+1,y+1,x) || getBoardData(z+1,y,x+1) || getBoardData(z+1,y+1,x+1) ) {
        return true;
    }
    return false;
}  


bool BoardLayout::blockedLeftOrRight(short z, short y, short x) {
     return( (getBoardData(z,y,x-1) || getBoardData(z,y+1,x-1)) &&
             (getBoardData(z,y,x+2) || getBoardData(z,y+1,x+2)) );
}

void BoardLayout::deleteTile(POSITION &p) {
    if ( p.e <m_depth && getBoardData(p.e,p.y,p.x) == '1') {
        setBoardData(p.e,p.y,p.x,0);
	setBoardData(p.e,p.y,p.x+1,0);
	setBoardData(p.e,p.y+1,p.x,0);
        setBoardData(p.e,p.y+1,p.x+1,0);
        maxTileNum--;
    }
} 

bool BoardLayout::anyFilled(POSITION &p) {
    return(getBoardData(p.e, p.y, p.x) != 0 ||
           getBoardData(p.e, p.y, p.x+1) != 0 ||
           getBoardData(p.e, p.y+1, p.x) != 0 ||
           getBoardData(p.e, p.y+1, p.x+1) != 0);
}
bool BoardLayout::allFilled(POSITION &p) {
    return(getBoardData(p.e, p.y, p.x) != 0 &&
           getBoardData(p.e, p.y, p.x+1) != 0 &&
           getBoardData(p.e, p.y+1, p.x) != 0 &&
           getBoardData(p.e, p.y+1, p.x+1) != 0);
}
void BoardLayout::insertTile(POSITION &p) {
    setBoardData(p.e,p.y,p.x,'1');
    setBoardData(p.e,p.y,p.x+1,'2');
    setBoardData(p.e,p.y+1,p.x+1,'3');
    setBoardData(p.e,p.y+1,p.x,'4');
}

UCHAR BoardLayout::getBoardData(short z, short y, short x) {
    return board.at((z*m_width*m_height)+(y*m_width)+x);
}

void BoardLayout::setBoardData(short z, short y, short x, UCHAR value) {
    board[(z*m_width*m_height)+(y*m_width)+x] = value;
}








