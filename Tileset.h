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

#ifndef _TILE_SET_H_
#define _TILE_SET_H_

#include <qbitmap.h>
#include "ksvgrenderer.h"
#include "KmTypes.h"

typedef struct tilesetmetricsdata {
    short lvloff;   // used for 3D indentation
    short w;    // tile width ( +border +shadow)
    short h;    // tile height ( +border +shadow)
    short fw;   // face width
    short fh;   // face height
} TILESETMETRICSDATA;

class Tileset {
   public:
     Tileset();	     
     ~Tileset();	

     bool loadDefault();
     bool loadTileset(const QString &filesetPath);
     bool reloadTileset( QSize newTilesize);
     QSize preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells);

     short width() {return scaleddata.w;};
     short height() {return scaleddata.h;};
     short levelOffset() {return scaleddata.lvloff;};
     short qWidth() {return (short) (scaleddata.fw / 2.0);};
     short qHeight() {return (short) (scaleddata.fh / 2.0);};

     QPixmap selectedTile(int num);
     QPixmap unselectedTile(int num);
     QPixmap tileface(int num);

  protected:

	void updateScaleInfo(short tilew, short tileh);
	void buildElementIdTable(void);
	QString pixmapCacheNameFromElementId(QString & elementid);
	QPixmap renderElement(short width, short height, QString & elementid);
  

  private:
    QList<QString> elementIdTable;

    TILESETMETRICSDATA originaldata;
    TILESETMETRICSDATA scaleddata;
    QString filename;  // cache the last file loaded to save reloading it

    KSvgRenderer svg;
    bool isSVG;
};


#endif


