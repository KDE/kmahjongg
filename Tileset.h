#ifndef _TILE_SET_H_
#define _TILE_SET_H_

#include <qbitmap.h>
#include "ksvgrenderer.h"

class Tileset {
   public:
     Tileset();	     
     ~Tileset();	

     bool loadTileset(const QString &filesetPath);
     bool reloadTileset( QSize newTilesize);
     QSize preferredTileSize(QSize boardsize, int horizontalCells, int verticalCells);

     short width() {return w;};
     short height() {return h;};
     short shadowSize() {return ss;};
     short size() {return s;};
     short qWidth() {return qw;};
     short qHeight() {return qh;};

     QPixmap *selectedPixmaps(int num) {
		return &(selectedPix[num]);
	};

     QPixmap *unselectedPixmaps(int num) {
		return &(unselectedPix[num]);
	};

  protected:

     enum { maxTiles=45 };
	void createTilePixmap(short x, short y, QPixmap& alltiles, QPixmap &dest, bool selected);
	void initStorage(short tilew, short tileh, short tileshadow, short tileborder,
bool tileisSVG);
  

  private:
    QPixmap selectedPix[maxTiles]; // selected tiles
    QPixmap unselectedPix[maxTiles]; // selected tiles

    short ss;   // left/bottom shadow width
    short bs;   // width of the border around a tile
    short w;    // tile width ( +border +shadow)
    short h;    // tile height ( +border +shadow)
    short qw;   // quarter tile width used in 3d rendering
    short qh;   // quarter tile height used in 3d rendering
    short s;	// buffer size for tile (width*height)

    QString filename;  // cache the last file loaded to save reloading it

    KSvgRenderer svg;
    bool isSVG;
};


#endif


