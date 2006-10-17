/*
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

#include "GameData.h"
#include <QtDebug>


GameData::GameData () {
    //Fixed for now, TODO add new constructor with dynamic board sizes
    m_width = 32;
    m_height = 16;
    m_depth = 5;
    m_maxTiles = (m_width*m_height*m_depth)/4;

    Highlight = QByteArray(m_width*m_height*m_depth, 0);
    Mask = QByteArray(m_width*m_height*m_depth, 0);
    Board = QByteArray(m_width*m_height*m_depth, 0);
    POSITION e; //constructor initializes it to 0
    MoveList = QVector<POSITION>(m_maxTiles, e);
}

GameData::~GameData () {

}

void GameData::putTile( short e, short y, short x, UCHAR f ){
    setBoardData(e,y,x,f);
    setBoardData(e,y+1,x,f);
    setBoardData(e,y+1,x+1,f);
    setBoardData(e,y,x+1,f);
}

bool GameData::tilePresent(int z, int y, int x) {
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return false;
    return(BoardData(z,y,x)!=0 && MaskData(z,y,x) == '1');
}

bool GameData::partTile(int z, int y, int x) {
    return (BoardData(z,y,x) != 0);
}

UCHAR GameData::MaskData(short z, short y, short x){
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return 0;
    return Mask.at((z*m_width*m_height)+(y*m_width)+x);
}

UCHAR GameData::HighlightData(short z, short y, short x){
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return 0;
    return Highlight.at((z*m_width*m_height)+(y*m_width)+x);
}

void GameData::setHighlightData(short z, short y, short x, UCHAR value) {
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return ;
    Highlight[(z*m_width*m_height)+(y*m_width)+x] = value;
}

UCHAR GameData::BoardData(short z, short y, short x){
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return 0;
    return Board.at((z*m_width*m_height)+(y*m_width)+x);
}

void GameData::setBoardData(short z, short y, short x, UCHAR value) {
    if ((y<0)||(x<0)||(z<0)||(y>m_height-1)||(x>m_width-1)||(z>m_depth-1)) return ;
    Board[(z*m_width*m_height)+(y*m_width)+x] = value;
}

POSITION& GameData::MoveListData(short i) {
    if ((i>=MoveList.size())|| (i<0)) {
      qDebug() << "Attempt to access GameData::MoveListData with invalid index";
      i=0 ;
    }
    return MoveList[i];
}

void GameData::setMoveListData(short i, POSITION& value){
    if ((i>=MoveList.size()) || (i<0)) return ;
    MoveList[i] = value;
}
