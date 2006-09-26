#ifndef _TILE_SET_H_
#define _TILE_SET_H_

#include <qbitmap.h>
#include "ksvgrenderer.h"

typedef struct tilesetmetricsdata {
    short ind;   // 3D indentation
    short w;    // tile width ( +border +shadow)
    short h;    // tile height ( +border +shadow)
    short qw;   // quarter tile width used in 3d rendering
    short qh; 
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
     short levelOffset() {return scaleddata.ind;};
     short qWidth() {return scaleddata.qw;};
     short qHeight() {return scaleddata.qh;};

     QPixmap *selectedPixmaps(int num) {
		return &(selectedPix[num]);
	};

     QPixmap *unselectedPixmaps(int num) {
		return &(unselectedPix[num]);
	};

  protected:

     enum { maxTiles=45 };
	void createTilePixmap(short x, short y, QPixmap& alltiles, QPixmap &dest, bool selected);
	void updateScaleInfo(short tilew, short tileh);
  

  private:
    QPixmap selectedPix[maxTiles]; // selected tiles
    QPixmap unselectedPix[maxTiles]; // selected tiles

    TILESETMETRICSDATA originaldata;
    TILESETMETRICSDATA scaleddata;
    QString filename;  // cache the last file loaded to save reloading it

    KSvgRenderer svg;
    bool isSVG;
};


#endif


